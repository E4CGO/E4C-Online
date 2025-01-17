//! @file OnlineController.cpp
//! @note 


#include "OnlineController.h"
#include <fstream>
#include "TCPCommand/TCPToken.h"
#include "TCPCommand/TCPLogin.h"
#include "TCPCommand/TCPClientData.h"
#include "TCPCommand/TCPEnemy.h"
#include "TCPCommand/TCPRoom.h"
#include "TCPCommand/TCPChat.h"
#include "TCPCommand/TCPMatching.h"

#include "UDPCommand/UDPSync.h"
#include "GameObject/Props/Teleporter.h"
#include "Scene/GameLoop/SceneGame/Stage/StageDungeon_E4C.h"	

#include "Scene/SceneManager.h"

namespace Online
{
	static OnlineController* instance = nullptr;

	/**************************************************************************//**
		@brief		コンストラクタ
		@param[in]	なし
		@return		なし
	*//***************************************************************************/
	OnlineController::OnlineController() : m_id(NULL), m_state(NULL)
	{
		instance = this;

		// TCP処理を実装
		m_tcpCommands[TCP_CMD::TOKEN] = new TCPToken(this, TCP_CMD::TOKEN);
		m_tcpCommands[TCP_CMD::LOGIN] = new TCPLogin(this, TCP_CMD::LOGIN);
		m_tcpCommands[TCP_CMD::CLIENT_DATA] = new TCPClientData(this, TCP_CMD::CLIENT_DATA);

		m_tcpCommands[TCP_CMD::MATCHING_START] = new TCPMatchingStart(this, TCP_CMD::MATCHING_START);
		m_tcpCommands[TCP_CMD::MATCHING_UPDATE] = new TCPMatchingUpdate(this, TCP_CMD::MATCHING_UPDATE);
		m_tcpCommands[TCP_CMD::MATCHING_END] = new TCPMatchingEnd(this, TCP_CMD::MATCHING_END);
		m_tcpCommands[TCP_CMD::MATCHING_READY] = new TCPMatchingReady(this, TCP_CMD::MATCHING_READY);

		m_tcpCommands[TCP_CMD::ROOM_NEW] = new TCPRoomNew(this, TCP_CMD::ROOM_NEW);
		m_tcpCommands[TCP_CMD::ROOM_IN] = new TCPRoomIn(this, TCP_CMD::ROOM_IN);
		m_tcpCommands[TCP_CMD::ROOM_OUT] = new TCPRoomOut(this, TCP_CMD::ROOM_OUT);

		m_tcpCommands[TCP_CMD::ENEMY_NEW] = new TCPEnemyNew(this, TCP_CMD::ENEMY_NEW);
		m_tcpCommands[TCP_CMD::ENEMY_SYNC] = new TCPEnemySync(this, TCP_CMD::ENEMY_SYNC);
		m_tcpCommands[TCP_CMD::ENEMY_OWNER] = new TCPEnemyOwner (this, TCP_CMD::ENEMY_OWNER);
		//m_tcpCommands[TCP_CMD::ENEMY_DESTROY] = new (this, TCP_CMD::ENEMY_DESTROY);


		m_tcpCommands[TCP_CMD::CHAT] = new TCPChat(this, TCP_CMD::CHAT);
		m_tcpCommands[TCP_CMD::PING] = new TCPNone(this, TCP_CMD::PING);



		// UDP処理
		m_udpCommands[UDP_CMD::SYNC] = new UDPSync(this, UDP_CMD::SYNC);
	}

	/**************************************************************************//**
		@brief	インスタンス取得
		@return	オンラインコントローラーインスタンス参照
	*//***************************************************************************/
	OnlineController* OnlineController::Instance()
	{
		return instance;
	}

	/**************************************************************************//**
		@brief		初期化処理
		@param[in]	なし
		@return		成功判断
	*//***************************************************************************/
	bool OnlineController::Initialize()
	{
		std::string server_ip = "";
		std::ifstream file("ip.txt");
		if (file)
		{
			server_ip = std::string((std::istreambuf_iterator<char>(file)),
				std::istreambuf_iterator<char>());
		}
		else
		{
			server_ip = Online::SV_IP;
		}


		m_ptcpSocket = new TCPClientSocket;
		if (!m_ptcpSocket->Connect(server_ip.c_str(), Online::port))
		{
			m_state = Online::OnlineController::STATE::OFFLINE;
			delete m_ptcpSocket;
			m_ptcpSocket = nullptr;
			return false;
		}
		m_pudpSocket = new UDPClientSocket;
		if (!m_pudpSocket->Connect(server_ip.c_str(), Online::port))
		{
			m_state = Online::OnlineController::STATE::OFFLINE;
			delete m_pudpSocket;
			m_pudpSocket = nullptr;
			return false;
		}

		m_tcpRecvThread = std::thread(&OnlineController::TCPRecvThread, this);

		m_state = Online::OnlineController::STATE::INIT;

		return true;
	}

	/**************************************************************************//**
		@brief		TCPの受信処理スレッド
		@param[in]	なし
		@return		なし
	*//***************************************************************************/
	void OnlineController::TCPRecvThread()
	{
		srand(static_cast <unsigned> (time(NULL)));
		DATA_HEADER header;
		size_t size = sizeof(DATA_HEADER);
		while (m_ptcpSocket->IsConnecting())
		{
			if (m_ptcpSocket->Receive(&header, size) > 0)
			{
				UINT temp = GetConsoleOutputCP();
				SetConsoleOutputCP(CP_UTF8);
				if (
					m_tcpCommands.find(header.cmd) == m_tcpCommands.end() ||
					!m_tcpCommands[header.cmd]->Receive(header.size))
				{
					std::cout << "TCP Recv Error:" << (int)header.cmd << std::endl;
					// 廃棄処理
					char buf;
					while (m_ptcpSocket->Receive(&buf, 1) > 0)
					{
						std::cout << buf;
					}
					std::cout << std::endl;
				}
				SetConsoleOutputCP(temp);
			}
		}
	}

	/**************************************************************************//**
		@brief		UDPの受信処理スレッド
		@param[in]	なし
		@return		なし
	*//***************************************************************************/
	void OnlineController::UDPRecvThread()
	{
		const size_t buffer_size = 5120;
		uint8_t buffer[buffer_size];
		DATA_HEADER header;
		int header_size = sizeof(DATA_HEADER);
		while (m_udpFlag)
		{
			ZeroMemory(&buffer, buffer_size);
			int size = m_pudpSocket->Receive(&buffer, buffer_size);
			if (size > header_size)
			{
				std::memcpy(&header, buffer, header_size);
				if (
					m_udpCommands.find(header.cmd) == m_udpCommands.end() ||
					!m_udpCommands[header.cmd]->Receive(&buffer[header_size], header.size))
				{
					std::cout << "UDP Recv Error:" << (int)header.cmd << std::endl;
				}
			}
		}
	}

	/**************************************************************************//**
		@brief		UDPの送信処理スレッド
		@param[in]	なし
		@return		なし
	*//***************************************************************************/
	void OnlineController::UDPSendThread()
	{
		clock_t last = clock();
		const float frequency = 0.25f;
		while (m_udpFlag)
		{
			if ((static_cast<float>(clock() - last) / CLOCKS_PER_SEC) > frequency)
			{
				last = clock();
				m_udpCommands[UDP_CMD::SYNC]->Send(nullptr);
			}
		}
	}

	/**************************************************************************//**
		@brief		名前でログイン（送信）
		@return		成功判断
		@note		名前は皆テスト
	*//***************************************************************************/
	bool OnlineController::Login()
	{
		TCPLogin::CHARA_DATA data;
		const PlayerCharacterData::CharacterInfo& info = PlayerCharacterData::Instance().GetCurrentCharacter();
		//data.name = info.name;
		data.name = info.name;
		for (int i = 0; i < PlayerCharacterData::APPEARANCE_PATTERN::NUM; i++)
		{
			data.appearance[i] = info.pattern[i];
		}

		return m_tcpCommands[TCP_CMD::LOGIN]->Send(&data);
	}

	/**************************************************************************//**
		@brief		チャットを送信
		@param[in]	message	チャットメッセージの参照
		@return		なし
	*//***************************************************************************/
	void OnlineController::Chat(std::string& message)
	{
		m_tcpCommands[TCP_CMD::CHAT]->Send(&message);
	}


	/**************************************************************************//**
		@brief		マッチング開始
		@param[in]	なし
		@return		なし
	*//***************************************************************************/
	void OnlineController::StartMatching(WidgetMatching* ui)
	{
		m_pMatchingUI = ui;
		m_tcpCommands[TCP_CMD::MATCHING_START]->Send(nullptr);
	}
	/**************************************************************************//**
		@brief		マッチング終了
		@param[in]	なし
		@return		なし
	*//***************************************************************************/
	void OnlineController::EndMatching()
	{
		if (m_state == STATE::OFFLINE) return;
		if (m_ptcpSocket == nullptr) return;
		m_tcpCommands[TCP_CMD::MATCHING_END]->Send(nullptr);
		m_pMatchingUI = nullptr;
	}
	/**************************************************************************//**
		@brief		マッチング準備完了
		@param[in]	なし
		@return		なし
	*//***************************************************************************/
	void OnlineController::ReadyMatching()
	{
		if (m_state == STATE::OFFLINE) return;
		if (m_ptcpSocket == nullptr) return;
		m_tcpCommands[TCP_CMD::MATCHING_READY]->Send(nullptr);
	}

	/**************************************************************************//**
		@brief		ダンジョンを生成する(ホスト)
		@param[in]	なし
		@return		なし
	*//***************************************************************************/
	void OnlineController::NewRoom()
	{
		if (m_pMatchingUI)
		{
			EndSync();
			Stage* stage = m_pMatchingUI->GetTeleporter()->GetStage();
			if (auto dungeon = dynamic_cast<StageDungeon_E4C*>(stage))
			{
				dungeon->GenerateDungeon();
				std::vector<uint8_t> roomOrder = dungeon->GetRoomOrder();
				m_tcpCommands[TCP_CMD::ROOM_NEW]->Send(&roomOrder);
			}
		}
	}
	/**************************************************************************//**
		@brief		ダンジョンを生成する
		@param[in]	roomOrder 部屋シード参照
		@return		なし
	*//***************************************************************************/
	void OnlineController::NewRoom(const std::vector<uint8_t>& roomOrder)
	{
		if (m_pMatchingUI)
		{
			Stage* stage = m_pMatchingUI->GetTeleporter()->GetStage();
			if (auto dungeon = dynamic_cast<StageDungeon_E4C*>(stage))
			{
				dungeon->SetRoomOrder(roomOrder);
				m_pMatchingUI->GetTeleporter()->Teleport();

				PlayerCharacterManager::Instance().ClearOtherPlayers();
				std::cout << "Clean PlayerCharacterManager" << std::endl;
				EndSync();
				m_pMatchingUI = nullptr;
			}
		}
	}
	/**************************************************************************//**
		@brief	入室送信
	*//***************************************************************************/
	void OnlineController::RoomIn()
	{
		m_tcpCommands[TCP_CMD::ROOM_IN]->Send(nullptr);
	}

	/**************************************************************************//**
		@brief		エネミーの生成送信処理
		@param[in]	enemyType	エネミータイプ
		@param[in]	spawnerId	スポナーID
		@param[in]	count		敵の数
	*//***************************************************************************/
	void OnlineController::NewEnemy(const uint8_t enemyType, uint8_t spawnerId, uint8_t count)
	{
		TCPEnemyNew::SEND_DATA data {
			spawnerId,
			enemyType,
			count,
		};
		m_tcpCommands[TCP_CMD::ENEMY_NEW]->Send(&data);
	}

	/**************************************************************************//**
	 	@brief		エネミーの同期送信
		@param[in]	data
	*//***************************************************************************/
	void OnlineController::SyncEnemy(std::vector<Enemy::SYNC_DATA>& data)
	{
		m_tcpCommands[TCP_CMD::ENEMY_SYNC]->Send(&data);
	}

	/**************************************************************************//**
		@brief		終了処理
		@param[in]	なし
		@return		なし
	*//***************************************************************************/
	void OnlineController::Finalize()
	{
		for (const std::pair<uint8_t, TCPCommand*>& command : m_tcpCommands)
		{
			delete command.second;
		}
		m_tcpCommands.clear();

		for (const std::pair<uint8_t, UDPCommand*>& command : m_udpCommands)
		{
			delete command.second;
		}
		m_udpCommands.clear();

		GAME_DATA.SetClientId(0);
	}

	/**************************************************************************//**
		@brief		デストラクタ
		@param[in]	なし
		@return		なし
	*//***************************************************************************/
	OnlineController::~OnlineController()
	{
		instance = nullptr;

		if (m_ptcpSocket != nullptr)
		{
			if (m_ptcpSocket->IsConnecting()) m_ptcpSocket->Disconnect();
			if (m_tcpRecvThread.joinable()) m_tcpRecvThread.join();
			delete m_ptcpSocket;
			m_ptcpSocket = nullptr;
		}
		if (m_pudpSocket != nullptr)
		{
			m_udpFlag = false;
			if (m_pudpSocket->IsConnecting()) m_pudpSocket->Disconnect();
			if (m_udpRecvThread.joinable()) m_udpRecvThread.join();
			if (m_udpSendThread.joinable()) m_udpSendThread.join();
			delete m_pudpSocket;
			m_pudpSocket = nullptr;
		}

		Finalize();
	}
}
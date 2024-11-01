//! @file OnlineController.cpp
//! @note 

#include <iostream>

#include "OnlineController.h"

#include "TCPCommand/TCPToken.h"
#include "TCPCommand/TCPLogin.h"
#include "TCPCommand/TCPClientData.h"
#include "TCPCommand/TCPRoom.h"
#include "TCPCommand/TCPChat.h"

#include "UDPCommand/UDPSync.h"

namespace Online
{
	/**************************************************************************//**
		@brief		コンストラクタ
		@param[in]	なし
		@return		なし
	*//***************************************************************************/
	OnlineController::OnlineController() : m_id(NULL), m_state(NULL)
	{
		// TCP処理を実装
		m_tcpCommands[TCP_CMD::TOKEN] = new TCPToken(this, TCP_CMD::TOKEN);
		m_tcpCommands[TCP_CMD::LOGIN] = new TCPLogin(this, TCP_CMD::LOGIN);
		m_tcpCommands[TCP_CMD::CLIENT_DATA] = new TCPClientData(this, TCP_CMD::CLIENT_DATA);
		m_tcpCommands[TCP_CMD::ROOM_IN] = new TCPRoomIn(this, TCP_CMD::ROOM_IN);
		m_tcpCommands[TCP_CMD::ROOM_OUT] = new TCPRoomOut(this, TCP_CMD::ROOM_OUT);
		m_tcpCommands[TCP_CMD::CHAT] = new TCPChat(this, TCP_CMD::CHAT);
		m_tcpCommands[TCP_CMD::PING] = new TCPNone(this, TCP_CMD::PING);

		// UDP処理
		m_udpCommands[UDP_CMD::SYNC] = new UDPSync(this, UDP_CMD::SYNC);
	}

	/**************************************************************************//**
		@brief		初期化処理
		@param[in]	なし
		@return		成功判断
	*//***************************************************************************/
	bool OnlineController::Initialize()
	{
		m_ptcpSocket = new TCPClientSocket;
		if (!m_ptcpSocket->Connect(Online::SV_IP, Online::port))
		{
			m_state = Online::OnlineController::STATE::OFFLINE;
			delete m_ptcpSocket;
			m_ptcpSocket = nullptr;
			return false;
		}
		m_pudpSocket = new UDPClientSocket;
		if (!m_pudpSocket->Connect(Online::SV_IP, Online::port))
		{
			m_state = Online::OnlineController::STATE::OFFLINE;
			delete m_pudpSocket;
			m_pudpSocket = nullptr;
			return false;
		}

		m_tcpRecvThread = std::thread(&OnlineController::TCPRecvThread, this);

		m_state = Online::OnlineController::STATE::INIT;


		clock_t start_time = clock();
		while (true)
		{
			if ((static_cast<float>(clock() - start_time) / CLOCKS_PER_SEC) > 5.0f)
			{
				return false;
			}
			if (m_state == Online::OnlineController::STATE::CONNNETED)
			{
				return true;
			}
		}
	}

	/**************************************************************************//**
		@brief		TCPの受信処理スレッド
		@param[in]	なし
		@return		なし
	*//***************************************************************************/
	void OnlineController::TCPRecvThread()
	{
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
						buf;
					}
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
		const size_t buffer_size = 1024;
		uint8_t buffer[buffer_size];
		DATA_HEADER header;
		int header_size = sizeof(DATA_HEADER);
		while (m_pudpSocket->IsConnecting())
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
		uint64_t sync_count_id = 0;
		while (m_pudpSocket->IsConnecting())
		{
			if ((static_cast<float>(clock() - last) / CLOCKS_PER_SEC) > frequency)
			{
				last = clock();
				m_udpCommands[UDP_CMD::SYNC]->Send(nullptr);
				sync_count_id++;
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
		data.name = "テスト";
		for (int i = 0; i < PlayerCharacterData::APPEARANCE_PATTERN::NUM; i++)
		{
			data.appearance[i] = info.Character.pattern[i];
		}

		m_state = Online::OnlineController::STATE::LOGIN;
		return m_tcpCommands[TCP_CMD::LOGIN]->Send(&data);
	}

	/**************************************************************************//**
		@brief		チャットを送信
		@param[in]	なし
		@return		なし
	*//***************************************************************************/
	void OnlineController::Chat(std::string& message)
	{
		m_tcpCommands[TCP_CMD::CHAT]->Send(&message);
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
	}

	/**************************************************************************//**
		@brief		デストラクタ
		@param[in]	なし
		@return		なし
	*//***************************************************************************/
	OnlineController::~OnlineController()
	{
		if (m_ptcpSocket != nullptr)
		{
			if (m_ptcpSocket->IsConnecting()) m_ptcpSocket->Disconnect();
			if (m_tcpRecvThread.joinable()) m_tcpRecvThread.join();
			delete m_ptcpSocket;
			m_ptcpSocket = nullptr;
		}
		if (m_pudpSocket != nullptr)
		{

			if (m_pudpSocket->IsConnecting()) m_pudpSocket->Disconnect();
			if (m_udpRecvThread.joinable()) m_udpRecvThread.join();
			if (m_udpSendThread.joinable()) m_udpSendThread.join();
			delete m_pudpSocket;
			m_pudpSocket = nullptr;
		}

		Finalize();
	}
}
//! @file OnlineController.h
//! @note 
#ifndef __ONLINE_ONLINE_CONTROLLER_H__
#define __ONLINE_ONLINE_CONTROLLER_H__

#include "TAKOEngine/Network/WinSock2Wrapper.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <thread>
#include <mutex>
#include "TAKOEngine/Network/TCPSocket.h"
#include "TAKOEngine/Network/UDPSocket.h"

#include "UI/Widget/WidgetMatching.h"

#include "GameObject/Character/Enemy/Enemy.h"

namespace Online
{

#ifdef _DEBUG
	const static char* SV_IP = "127.0.0.1";
#else
	const static char* SV_IP = "34.82.222.201";
#endif // _DEBUG

	const static char* port = "9000";
	/**************************************************************************//**
		@enum	TCP_CMD
		@brief	TCP接続コマンド
		@par	[説明]
			コマンドによって送信・受信処理分け
	*//***************************************************************************/
	enum TCP_CMD : uint8_t
	{
		TOKEN = 0,			/*!< 接続トークンの受信	*/
		LOGIN,				/*!< キャラデータ（名前）を送信 */

		CLIENT_DATA,		/*!< 同室キャラクターのデータ受信 */

		MATCHING_START,		/*!< マッチング開始 */
		MATCHING_READY,		/*!< マッチング準備完了 */
		MATCHING_UPDATE,	/*!< マッチング更新 */
		MATCHING_END,		/*!< マッチング中断 */

		ROOM_NEW,			/*!< 部屋の作成*/
		ROOM_IN,			/*!< 入室命令処理 */
		ROOM_OUT,			/*!< クライアント退室処理 */
		ROOM_NEXT,			/*!< 次の部屋 */

		ENEMY_NEW,			/*!< エネミー生成処理 */
		ENEMY_SYNC,			/*!< エネミー同期処理 */
		ENEMY_OWNER,		/*!< エネミー所有者更新処理 */
		ENEMY_DESTROY,		/*!< エネミー削除処理 */

		CHAT,				/*!< チャット */

		PING = UINT8_MAX,	/*!< 接続チェック */
	};
	/**************************************************************************//**
		@enum	UDP_CMD
		@brief	UDP接続コマンド
		@par	[説明]
			コマンドによって送信・受信処理分け
	*//***************************************************************************/
	enum UDP_CMD : uint8_t
	{
		SYNC = 0,		/*!< 基本同期通信 */
		HIT_SEND,		/*!< クライアントダメージ判定送信 */
		HIT_ACCEPT,		/*!< ホストダメージ判定承認 */
	};

	class TCPCommand;
	class UDPCommand;

	/**************************************************************************//**
		@class		OnlineController
		@brief		OnlineGameのコントロールクラス
		@par		[説明]
						クラスサ
	*//***************************************************************************/
	class OnlineController
	{
	public:
		// コンストラク
		OnlineController();
		// デストラクタ
		~OnlineController();

		/**************************************************************************//**
			@enum	STATE
			@brief	オンライン接続のステート
			@par	[説明]
		*//***************************************************************************/
		enum STATE : uint8_t
		{
			DEFAULT = 0,	/*!< 初期状態 */
			INIT,			/*!< 初期化 */
			OFFLINE,		/*!< オフライン */
			CONNNETED,		/*!< 接続完了 */
			LOGIN,			/*!< ログイン開始 */
			LOGINED,		/*!< ログイン完了 */
			SYNC,			/*!< 同期中 */
			END,			/*!< 終了 */
			ERR,			/*!< エラー */
		};
#pragma pack(push, 1)
		struct DATA_HEADER {
			uint8_t cmd;
			uint32_t size;
		};

		// 攻撃同期
		struct HIT_DATA
		{
			uint32_t hit_id;
			uint32_t enemy_id;
			uint16_t damage;
		};
#pragma pack(pop)
		// インスタンス取得
		static OnlineController* Instance();

		// 接続開始
		bool Initialize();
		// 接続終了
		void Finalize();

		// 接続状態取得
		const uint8_t GetState() const { return m_state; }
		// 接続状態更新
		void SetState(const uint8_t& state) { m_state = state; }
		// 接続ID取得
		const uint32_t GetId() const { return m_id; }
		// 接続トークン取得
		const std::string& GetToken() const { return m_token; }

		//// TCP処理取得
		//TCPCommand*& GetTcpCommand(uint8_t cmd) { return m_tcpCommands[cmd]; }
		// TCPソケット取得
		TCPClientSocket*& GetTcpSocket() { return m_ptcpSocket; }
		// UDPソケット取得
		UDPClientSocket*& GetUdpSocket() { return m_pudpSocket; }

		// IDとトークン更新
		void SignIn(const uint32_t& id, std::string token)
		{
			m_id = id;
			m_token = token;
			m_state = STATE::CONNNETED;
		};

		// 名前で登録
		bool Login();
		// チャットを送信
		void Chat(std::string& message);

		// マッチング開始
		void StartMatching(WidgetMatching* ui);
		// マッチング終了
		void EndMatching();
		// マッチング準備完了
		void ReadyMatching();
		// ダンジョンを生成する(ホスト)
		void NewRoom(); 
		// ダンジョンを生成する
		void NewRoom(const std::vector<uint8_t>& roomOrder);
		// 入室送信
		void RoomIn();
		// 次の部屋
		void NextRoom(std::vector<uint8_t>& roomOrder);

		// エネミーの生成送信
		void NewEnemy(const uint8_t enemyType, uint8_t spawnerId, uint8_t count = 1);
		// エネミーの同期送信
		void SyncEnemy(std::vector<Enemy::SYNC_DATA>& data);

		void RegisterHit(const uint32_t& enemy_id, const uint16_t& damage) {
			std::lock_guard<std::mutex> lock(m_hit_mtx);
			m_hitList.push_back({
				++m_hitDataCount,
				enemy_id,
				damage
			});
		}
		void RemoveHit(const uint32_t& hit_id)
		{
			std::lock_guard<std::mutex> lock(m_hit_mtx);
			std::erase_if(m_hitList, [&](const HIT_DATA& hit) { return hit.hit_id == hit_id; });
		}
		void RemoveHit(const std::vector<uint32_t>& hit_ids)
		{
			std::lock_guard<std::mutex> lock(m_hit_mtx);
			std::erase_if(m_hitList, [&](const HIT_DATA& hit) { return std::find(hit_ids.begin(), hit_ids.end(), hit.hit_id) != hit_ids.end(); });
		}

		// 同期開始
		void BeginSync()
		{
			if (m_state != STATE::LOGINED) return;
			if (!m_udpSendThread.joinable())
			{
				m_udpSendThread = std::thread(&OnlineController::UDPSendThread, this);
			}
			if (!m_udpRecvThread.joinable())
			{
				m_udpRecvThread = std::thread(&OnlineController::UDPRecvThread, this);
			}
			m_state = STATE::SYNC;
			m_udpFlag = true;
			std::cout << "BeginSync" << std::endl;
		}
		void EndSync()
		{
			m_udpFlag = false;
			if (m_udpSendThread.joinable())
			{
				m_udpSendThread.join();
			}
			if (m_udpRecvThread.joinable())
			{
				m_udpRecvThread.join();
			}
			m_state = STATE::LOGINED;
			std::cout << "EndSync" << std::endl;
		}

		WidgetMatching* GetMathcingUI() { return m_pMatchingUI; }
	private:
		// TCP受信
		void TCPRecvThread();
		// UDP受信
		void UDPRecvThread();
		// UDP送信
		void UDPSendThread();
	private:
		uint8_t m_state;		// 接続状態
		uint32_t m_id;			// 接続ID
		std::string m_token;	// 接続用ランダムトークン

		TCPClientSocket* m_ptcpSocket = nullptr;	// TCPソケット
		std::thread m_tcpRecvThread;				// TCPスレッド

		bool m_udpFlag = false;						// UDPフラグ
		UDPClientSocket* m_pudpSocket = nullptr;	// UDPソケット
		std::thread m_udpSendThread;				// UDP送信スレッド
		std::thread m_udpRecvThread;				// UDP受信スレッド

		std::unordered_map<uint8_t, TCPCommand*> m_tcpCommands;	// TCP処理
		std::unordered_map<uint8_t, UDPCommand*> m_udpCommands;	// UDP処理

		//std::mutex m_client_mtx; //	クライアント用ミューテックス

		WidgetMatching* m_pMatchingUI = nullptr;
		
		std::vector<HIT_DATA> m_hitList;	// 同期待ちのヒットデータ
		uint32_t m_hitDataCount = 0;		// ヒットデータ
		std::mutex m_hit_mtx;				// ヒット用ミューテックス
	};

	typedef OnlineController Controller;
	typedef OnlineController::STATE State;
}
#define ONLINE_CONTROLLER Online::Controller::Instance()
#endif


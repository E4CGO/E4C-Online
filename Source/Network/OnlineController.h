//! @file OnlineController.h
//! @note 
#ifndef __ONLINE_ONLINE_CONTROLLER_H__
#define __ONLINE_ONLINE_CONTROLLER_H__

#include "TAKOEngine/Network/WinSock2Wrapper.h"

#include <string>
#include <unordered_map>
#include <thread>
#include <mutex>
#include "TAKOEngine/Network/TCPSocket.h"
#include "TAKOEngine/Network/UDPSocket.h"

namespace Online
{
	const static char* SV_IP = "127.0.0.1";
	//const static char* SV_IP = "34.82.222.201";

	const static char* port = "9000";
	/**************************************************************************//**
		@enum	TCP_CMD
		@brief	TCP接続コマンド
		@par	[説明]
			コマンドによって送信・受信処理分け
	*//***************************************************************************/
	enum TCP_CMD : uint8_t
	{
		TOKEN = 0,		/*!< 接続トークンの受信	*/
		LOGIN,			/*!< キャラデータ（名前）を送信 */

		CLIENT_DATA,	/*!< 同室キャラクターのデータ受信 */
		ROOM_NEW,		/*!< 部屋の作成（仮）*/
		ROOM_IN,		/*!< 入室処理 */
		ROOM_OUT,		/*!< 退室処理 */

		CHAT,			/*!< チャット */

		PING = 255,		/*!< 接続チェック */
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
		DAMAGE			/*!< ダメージ判定 */
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
#pragma pack(pop)
		// 接続開始
		bool Initialize();
		// 接続終了
		void Finalize();

		// 接続状態取得
		const uint8_t GetState() const { return m_state; }
		// 接続状態更新
		void SetState(const uint8_t& state) { m_state = state; }
		// 接続ID取得
		const uint64_t GetId() const { return m_id; }
		// 接続トークン取得
		const std::string& GetToken() const { return m_token; }

		//// TCP処理取得
		//TCPCommand*& GetTcpCommand(uint8_t cmd) { return m_tcpCommands[cmd]; }
		// TCPソケット取得
		TCPClientSocket*& GetTcpSocket() { return m_ptcpSocket; }
		// UDPソケット取得
		UDPClientSocket*& GetUdpSocket() { return m_pudpSocket; }

		// IDとトークン更新
		void SignIn(const uint64_t& id, std::string token)
		{
			m_id = id;
			m_token = token;
			m_state = STATE::CONNNETED;
		};

		// 名前で登録
		bool Login();
		// チャットを送信
		void Chat(std::string& message);
		// 同期開始
		void BeginSync()
		{
			if (m_state != STATE::LOGINED) return;
			if (!m_udpSendThread.joinable())
			{
				m_udpSendThread = std::thread(&OnlineController::UDPSendThread, this);
			}
			m_udpRecvThread = std::thread(&OnlineController::UDPRecvThread, this);
			m_state = STATE::SYNC;
		}
	private:
		// TCP受信
		void TCPRecvThread();
		// UDP受信
		void UDPRecvThread();
		// UDP送信
		void UDPSendThread();
	private:
		uint8_t m_state;		// 接続状態
		uint64_t m_id;			// 接続ID
		std::string m_token;	// 接続用ランダムトークン

		TCPClientSocket* m_ptcpSocket = nullptr;	// TCPソケット
		std::thread m_tcpRecvThread;				// TCPスレッド

		UDPClientSocket* m_pudpSocket = nullptr;	// UDPソケット
		std::thread m_udpSendThread;				// UDP送信スレッド
		std::thread m_udpRecvThread;				// UDP受信スレッド

		std::unordered_map<uint8_t, TCPCommand*> m_tcpCommands;	// TCP処理
		std::unordered_map<uint8_t, UDPCommand*> m_udpCommands;	// UDP処理

		std::mutex m_client_mtx; //	クライアント用ミューテックス
	};
}

#endif


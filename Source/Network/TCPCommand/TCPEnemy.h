//! @file TCPEnemy.h
//! @note 

#ifndef __ONLINE_TCP_ENEMY_H__
#define __ONLINE_TCP_ENEMY_H__

#include "TCPCommand.h"

namespace Online {

	/**************************************************************************//**
		@class		TCPEnemyNew
		@brief		エネミー生成処理クラス
		@par		[説明]
						エネミー生成を送信し、エネミーIDを受信する
	*//***************************************************************************/
	class TCPEnemyNew : public TCPCommand
	{
	public:
		TCPEnemyNew(OnlineController* controller, uint8_t cmd) : TCPCommand(controller, cmd) {};
		~TCPEnemyNew() {};

#pragma pack(push, 1)
		struct RECV_DATA
		{
			uint8_t spawnerIndex = UINT8_MAX;
			uint64_t enemyNum = 0;
		};
		struct SEND_DATA
		{
			uint8_t spawnerIndex;
			uint8_t enemyType;
			uint8_t count;
		};
#pragma pack(pop)

		// データ受信
		bool Receive(size_t size) override;
		// データ送信
		bool Send(void* data) override;
	};

	/**************************************************************************//**
		@class		TCPEnemySync
		@brief		エネミー同期処理クラス
		@par	[説明]
					エネミー同期データを受信・送信する
	*//***************************************************************************/
	class TCPEnemySync : public TCPCommand
	{
	public:
		TCPEnemySync(OnlineController* controller, uint8_t cmd) : TCPCommand(controller, cmd) {};
		~TCPEnemySync() {};

		// データ受信
		bool Receive(size_t size) override;
		// データ送信
		bool Send(void* data) override;
	};

	/**************************************************************************//**
		@class		TCPEnemyOwner
		@brief		エネミーオーナーの変更
		@par	[説明]
					サーバーから担当エネミーの指定
	*//***************************************************************************/
	class TCPEnemyOwner : public TCPCommand
	{
	public:
		TCPEnemyOwner(OnlineController* controller, uint8_t cmd) : TCPCommand(controller, cmd) {};
		~TCPEnemyOwner() {};

		// データ受信
		bool Receive(size_t size) override;
		// データ送信 (無し)
		bool Send(void* data) { return true; }
	};
}

#endif // !__ONLINE_TCP_ENEMY_H__





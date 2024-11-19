//! @file TCPMatching.h
//! @note 

#ifndef __ONLINE_TCP_MATCHIG_H__
#define __ONLINE_TCP_MATCHIG_H__


#include "TCPCommand.h"

namespace Online
{
	/**************************************************************************//**
		@class		TCPMatchingStart
		@brief		マッチング開始処理クラス
		@par		[説明]
						マッチングスタートの送信
	*//***************************************************************************/
	class TCPMatchingStart : public TCPCommand
	{
	public:
		TCPMatchingStart(OnlineController* controller, uint8_t cmd) : TCPCommand(controller, cmd) {};

		// データ送信
		bool Send(void* data) override;
	};

	/**************************************************************************//**
	@class		TCPMatchingUpdate
	@brief		マッチング更新処理クラス
	@par		[説明]
					マッチング更新データの受信処理
*//***************************************************************************/
	class TCPMatchingUpdate : public TCPCommand
	{
	public:
		TCPMatchingUpdate(OnlineController* controller, uint8_t cmd) : TCPCommand(controller, cmd) {};

		// データ受信
		bool Receive(size_t size) override;
	private:
		struct MATCHING_DATA {
			uint64_t owner_id;
			uint8_t client_num;
			uint8_t max_client_num;
		};
	};

	/**************************************************************************//**
		@class		TCPMatchingReady
		@brief		マッチング準備完了処理クラス
		@par		[説明]
						マッチング準備完了の送信
	*//***************************************************************************/
	class TCPMatchingReady : public TCPCommand
	{
	public:
		TCPMatchingReady(OnlineController* controller, uint8_t cmd) : TCPCommand(controller, cmd) {};

		// データ送信
		bool Send(void* data) override;
	};

	/**************************************************************************//**
		@class		TCPMatchingEnd
		@brief		マッチング終了処理クラス
		@par		[説明]
						マッチング終了の送信
	*//***************************************************************************/
	class TCPMatchingEnd : public TCPCommand
	{
	public:
		TCPMatchingEnd(OnlineController* controller, uint8_t cmd) : TCPCommand(controller, cmd) {};

		// データ送信
		bool Send(void* data) override;
	};
}
#endif // __ONLINE_TCP_MATCHIG_H__
//! file TCPCHAT.h
//! @note 

#ifndef __ONLINE_TCP_CHAT_H__
#define __ONLINE_TCP_CHAT_H__

#include <string>
#include "TCPCommand.h"

namespace Online
{
	/**************************************************************************//**
		@class		TCPChat
		@brief		チャット処理クラス
		@par		[説明]
						TCPによるチャット送信・受信処理
	*//***************************************************************************/
	class TCPChat : public TCPCommand
	{
	public:
		TCPChat(OnlineController* controller, uint8_t cmd) : TCPCommand(controller, cmd) {};
	
		// データ受信
		bool Receive(size_t size) override;
		// データ送信
		bool Send(void* data) override;
	private:
		// チャート用受信データ
		struct CHAT_DATA
		{
			uint64_t client_id = 0;
			std::string message;
		};
	};
}

#endif
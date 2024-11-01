//! file TCPToken.h
//! @note 

#ifndef __ONLINE_TCP_TOKEN_H__
#define __ONLINE_TCP_TOKEN_H__

#include <string>
#include "TCPCommand.h"

namespace Online
{
	/**************************************************************************//**
		@class		TCPToken
		@brief		トークン処理クラス
		@par		[説明]
						サーバー接続よう認証トークン・UDP用
	*//***************************************************************************/
	class TCPToken : public TCPCommand
	{
	public:
		TCPToken(OnlineController* controller, uint8_t cmd) : TCPCommand(controller, cmd) {};

		// データ受信
		bool Receive(size_t size) override;
	private:
		struct DATA_SET
		{
			uint64_t id = -1;
			uint64_t token_size = 0;
			char* token = nullptr;
		};
	};
}

#endif
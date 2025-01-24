//! file TCPClientData.h
//! @note 

#ifndef __ONLINE_TCP_CLIENT_DATA_H__
#define __ONLINE_TCP_CLIENT_DATA_H__

#include <string>
#include <vector>
#include "TCPCommand.h"

namespace Online
{
	/**************************************************************************//**
		@class		TCPClientData
		@brief		クライアントデータ処理クラス
		@par		[説明]
						クライアントデータ
	*//***************************************************************************/
	class TCPClientData : public TCPCommand
	{
	public:
		TCPClientData(OnlineController* controller, uint8_t cmd) : TCPCommand(controller, cmd) {};

		struct CLIENT_DATA {
			uint32_t client_id;
			char appearance[10];
		};

		// データ受信
		bool Receive(size_t size) override;
	};
}

#endif

//! @file UDPSync.h
//! @note 

#ifndef __INCLUDE_ONLINE_UDP_SYNC_H__
#define __INCLUDE_ONLINE_UDP_SYNC_H__

#include "UDPCommand.h"

namespace Online
{
	/**************************************************************************//**
		@class	UDPSync
		@brief	同期処理クラス
		@par	[説明]
	*//***************************************************************************/
	class UDPSync : public UDPCommand
	{
	public:
		UDPSync(OnlineController* controller, uint8_t cmd) : UDPCommand(controller, cmd) {};

		// データ受信
		bool Receive(uint8_t* buffer, size_t size) override;
		// データ送信
		void Send(void* data) override;
	private:
		uint32_t m_sync_count_id = 1;
	};
}

#endif // !__INCLUDE_ONLINE_UDP_SYNC_H__

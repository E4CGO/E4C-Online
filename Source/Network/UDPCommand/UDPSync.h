//! @file UDPSync.h
//! @note 

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
	};
}


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

//#pragma pack(push, 1)
//		struct SYNC_DATA {
//			uint64_t client_id;
//			uint64_t sync_count_id;
//			float position[3];
//			float velocity[3];
//			float rotate;
//			uint8_t state;
//		};
//#pragma pack(pop)

		// データ受信
		bool Receive(uint8_t* buffer, size_t size) override;
		// データ送信
		void Send(void* data) override;
	};
}


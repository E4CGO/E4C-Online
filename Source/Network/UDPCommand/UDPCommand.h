//! @file UDPCommand.h
//! @note 


#ifndef __ONLINE_UDP_COMMAND_H__
#define __ONLINE_UDP_COMMAND_H__

#include <stdint.h>
#include "Network/OnlineController.h"

namespace Online {

	/**************************************************************************//**
		@class		UDPCommand
		@brief		UDP処理の基底クラス
		@par		[説明]
						クラスサ
	*//***************************************************************************/
	class OnlineController;
	class UDPCommand
	{
	public:
		// コンストラクタ（引数付き）
		UDPCommand(OnlineController* controller, uint8_t cmd) : m_pcontroller(controller), m_cmd(cmd) {};
		// デストラクタ
		virtual ~UDPCommand() = default;

		// データ受信
		virtual bool Receive(uint8_t* buffer, size_t size) = 0;
		// データ送信
		virtual void Send(void* data) {};
	protected:
		// UDP送信用ヘッダ
		void CreateHeaderBuffer(std::vector<uint8_t>& buffer, uint8_t cmd, uint32_t size);
	protected:
		OnlineController* m_pcontroller;
		uint8_t m_cmd;
	};
}

#endif

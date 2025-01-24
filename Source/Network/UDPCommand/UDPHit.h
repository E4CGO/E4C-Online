//! @file UDPHit.h
//! @note 

#ifndef __INCLUDE_ONLINE_UDP_HIT_H__
#define __INCLUDE_ONLINE_UDP_HIT_H__

#include "UDPCommand.h"

namespace Online
{
	/**************************************************************************//**
		@class	UDPHitSend
		@brief	同期処理送信クラス
		@par	[説明]
	*//***************************************************************************/
	class UDPHitSend : public UDPCommand
	{
	public:
		UDPHitSend(OnlineController* controller, uint8_t cmd) : UDPCommand(controller, cmd) {};

		// データ受信
		bool Receive(uint8_t* buffer, size_t size) override;
		// データ送信
		void Send(void* data) override;
	};
	/**************************************************************************//**
		@class	UDPHitAccept
		@brief	ダメージ同期承認処理クラス
		@par	[説明]
	*//***************************************************************************/
	class UDPHitAccept : public UDPCommand
	{
	public:
		UDPHitAccept(OnlineController* controller, uint8_t cmd) : UDPCommand(controller, cmd) {
			m_receivedHit.clear();
		};

		// データ受信
		bool Receive(uint8_t* buffer, size_t size);
		// データ送信
		void Send(void* data) {};
	private:
		std::unordered_map<uint32_t, uint32_t> m_receivedHit;
	};
}

#endif // !__INCLUDE_ONLINE_UDP_HIT_H__

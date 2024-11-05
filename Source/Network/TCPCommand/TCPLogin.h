﻿//! file TCPLogin.h
//! @note 

#ifndef __ONLINE_TCP_LOGIN_H__
#define __ONLINE_TCP_LOGIN_H__

#include <string>
#include <vector>
#include "TCPCommand.h"
#include "PlayerCharacterData.h"

namespace Online
{
	/**************************************************************************//**
		@class		TCPLogin
		@brief		ログイン処理クラス
		@par		[説明]
						キャラクター用ログイン
	*//***************************************************************************/
	class TCPLogin : public TCPCommand
	{
	public:
		TCPLogin(OnlineController* controller, uint8_t cmd) : TCPCommand(controller, cmd) {};

		struct CHARA_DATA
		{
			std::string name;
			uint8_t appearance[PlayerCharacterData::APPEARANCE_PATTERN::NUM] = {};
		};

		// データ受信
		bool Receive(size_t size) override;
		// データ送信
		bool Send(void* data) override;
	};
}
#endif
﻿//! file TCPToken.cpp
//! @note 

#include "TCPToken.h"

#include <iostream>
#include "GameData.h"

namespace Online
{
	/**************************************************************************//**
		@brief		トークン受信関数
		@param[in]	size データサイズ
		@return		成功判定
	*//***************************************************************************/
	bool TCPToken::Receive(size_t size)
	{
		DATA_SET signinData;

		const size_t data_size = (sizeof(uint32_t) + sizeof(uint64_t));
		if (m_pcontroller->GetTcpSocket()->Receive(&signinData, data_size) > 0)
		{
			signinData.token = new char[signinData.token_size + 1];
			ZeroMemory(signinData.token, signinData.token_size + 1);
			if (m_pcontroller->GetTcpSocket()->Receive(signinData.token, signinData.token_size) > 0)
			{
				// サインイン 接続ID、トークン
				PlayerCharacterManager::Instance().GetPlayerCharacterById()->SetClientId(signinData.id);
				GAME_DATA.SetClientId(signinData.id);
				m_pcontroller->SignIn(signinData.id, signinData.token);

				std::cout << m_pcontroller->GetId() << " " << m_pcontroller->GetToken() << std::endl;
				delete signinData.token;
				return true;
			}
			delete signinData.token;
		}
		return false;
	}
}
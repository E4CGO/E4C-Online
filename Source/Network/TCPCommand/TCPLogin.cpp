//! file TCPLogin.cpp
//! @note 

#include <vector>
#include <iostream>
#include "TCPLogin.h"
#include "TAKOEngine/Tool/Encode.h"
#include "TAKOEngine/Tool/U8Buffer.h"

#include "GameObject/Character/Player/PlayerCharacterManager.h"

namespace Online
{
	/**************************************************************************//**
		@brief		ログイン受信関数
		@param[in]	size データサイズ
		@return		成功判定
	*//***************************************************************************/
	bool TCPLogin::Receive(size_t size)
	{
		m_pcontroller->SetState(OnlineController::STATE::LOGINED);
		m_loginFlag = true;
		std::cout << "LOGIN: " << PlayerCharacterManager::Instance().GetPlayerCharacterById()->GetName().c_str() << std::endl;

		return true;
	}
	/**************************************************************************//**
		@brief		ログイン受信関数
		@param[in]	data データ参照ポインタ
		@return		成功判断
	*//***************************************************************************/
	bool TCPLogin::Send(void* data)
	{
		CHARA_DATA* loginData = static_cast<CHARA_DATA*>(data);

		std::vector<uint8_t> buffer;

		CreateHeaderBuffer(buffer, m_cmd, sizeof(loginData->appearance) + static_cast<uint32_t>(loginData->name.size()));
		// 外見データ
		buffer.insert(buffer.end(), loginData->appearance, loginData->appearance + sizeof(loginData->appearance));
		// 名前データ
		U8Buffer::InsertU8(buffer, loginData->name);

		bool result = m_pcontroller->GetTcpSocket()->Send(buffer.data(), buffer.size()) >= 0;

		if (result)
		{
			m_pcontroller->SetState(OnlineController::STATE::LOGIN);
			m_loginThread = std::thread(&TCPLogin::LoginThread, this);
		}
		else
		{
			m_pcontroller->SetState(OnlineController::STATE::OFFLINE);
		}
		return result;
	}

	/**************************************************************************//**
		@brief		ログイン待ちスレッド
		@param[in]	data データ参照ポインタ
		@return		成功判断
	*//***************************************************************************/
	void TCPLogin::LoginThread()
	{
		clock_t start_time = clock();
		while (!m_loginFlag)
		{
			if ((static_cast<float>(clock() - start_time) / CLOCKS_PER_SEC) > 5.0f)
			{
				// 5秒もログインなし
				m_pcontroller->SetState(OnlineController::STATE::OFFLINE);
				break;
			}
		}
	}
}
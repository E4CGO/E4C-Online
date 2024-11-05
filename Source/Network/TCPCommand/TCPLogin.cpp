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

		std::cout << "LOGIN: " << PlayerCharacterManager::Instance().GetPlayerCharacterById()->GetName().c_str() << std::endl;

		m_pcontroller->BeginSync();

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

		std::string utf8name = Encode::shift_jis_to_utf8(loginData->name.c_str());

		CreateHeaderBuffer(buffer, m_cmd, sizeof(loginData->appearance) + static_cast<uint32_t>(utf8name.size() - 1));
		// 外見データ
		buffer.insert(buffer.end(), loginData->appearance, loginData->appearance + sizeof(loginData->appearance));
		// 名前でT-あ
		U8Buffer::InsertU8(buffer, utf8name);

		return m_pcontroller->GetTcpSocket()->Send(buffer.data(), buffer.size()) >= 0;
	}
}
//! @file TCPChat.cpp
//! @note 

#include "TCPChat.h"
#include <vector>
#include <iostream>
#include "TAKOEngine/Tool/Encode.h"
#include "TAKOEngine/Tool/U8Buffer.h"
#include "GameObject/Character/Player/PlayerCharacterManager.h"

namespace Online
{
	/**************************************************************************//**
		@brief		チャート受信関数
		@param[in]	size データサイズ
		@return		なし
	*//***************************************************************************/
	bool TCPChat::Receive(size_t size)
	{
		CHAT_DATA data;
		if (
			m_pcontroller->GetTcpSocket()->Receive(&data.client_id, sizeof(data.client_id)) > 0 &&
			ReceiveString(data.message)
			)
		{
			PlayerCharacterManager& playerCharacterManager = PlayerCharacterManager::Instance();
			PlayerCharacter* player = playerCharacterManager.GetPlayerCharacterById(data.client_id);
			if (player != nullptr)
			{
				std::cout << player->GetName() << reinterpret_cast <const char*>(u8"：") << data.message << std::endl;
			}
			return true;
		}

		return false;
	}

	/**************************************************************************//**
		@brief		チャット受信関数
		@param[in]	data データ参照ポインタ
		@return		成功判断
	*//***************************************************************************/
	bool TCPChat::Send(void* data)
	{
		std::string* message = static_cast<std::string*>(data);

		std::vector<uint8_t> buffer;

		CreateHeaderBuffer(buffer, m_cmd, static_cast<uint32_t>(message->size()));

		// チャット内容
		U8Buffer::InsertU8(buffer, *message);

		return m_pcontroller->GetTcpSocket()->Send(buffer.data(), buffer.size()) >= 0;
	}
}

//! file TCPRoom.cpp
//! @note 

#include "TCPRoom.h"
#include <iostream>

#include "GameObject/Character/Player/PlayerCharacterManager.h"

namespace Online
{
	/**************************************************************************//**
		@brief		クライアント入室受信関数
		@param[in]	size データサイズ
		@return		成功判定
	*//***************************************************************************/
	bool TCPRoomIn::Receive(size_t size)
	{
		PlayerCharacter* player = ReceiveClient();
		if (player == nullptr) return false;

		std::cout << player->GetName() << reinterpret_cast <const char*>(u8" が 入室しました。") << std::endl;
		return true;
	}


	/**************************************************************************//**
		@brief		クライアント退室受信関数
		@param[in]	size データサイズ
		@return		成功判定
	*//***************************************************************************/
	bool TCPRoomOut::Receive(size_t size)
	{
		uint64_t client_id = 0;
		if (m_pcontroller->GetTcpSocket()->Receive(&client_id, size) > 0)
		{
			PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById(client_id);
			if (player != nullptr)
			{
				std::cout << player->GetName() << reinterpret_cast <const char*>(u8" が 退室しました。") << std::endl;
				PlayerCharacterManager::Instance().Remove(client_id);
			}
			return true;
		}
		return false;
	}
}
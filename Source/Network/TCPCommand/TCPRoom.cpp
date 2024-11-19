//! file TCPRoom.cpp
//! @note 

#include "TCPRoom.h"
#include <iostream>

#include "GameObject/Character/Player/PlayerCharacterManager.h"

namespace Online
{
	/**************************************************************************//**
		@brief		入室命令受信関数
		@param[in]	size データサイズ
		@return		成功判定
	*//***************************************************************************/
	bool TCPRoomIn::Receive(size_t size)
	{
		return false;
	}

	/**************************************************************************//**
	 	@brief		入室命令送信関数
		@param[in]	data データ参照ポインタ
		@return		成功判断
	*//***************************************************************************/
	bool TCPRoomIn::Send(void* data)
	{
		return false;
	}

	/**************************************************************************//**
	 	@brief		部屋作りの受信関数
		@param[in]	size データサイズ
		@return		成功判断
	*//***************************************************************************/
	bool TCPRoomNew::Receive(size_t size)
	{
		std::cout << "NewRoom" << std::endl;
		m_pcontroller->NewRoom();

		return false;
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
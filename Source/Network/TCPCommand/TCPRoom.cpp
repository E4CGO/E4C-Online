//! file TCPRoom.cpp
//! @note 

#include "TCPRoom.h"
#include <iostream>

#include "GameObject/Character/Player/PlayerCharacterManager.h"
#include "Map/DungeonData.h"

namespace Online
{
	/**************************************************************************//**
		@brief		入室命令受信関数
		@param[in]	size データサイズ
		@return		成功判定
	*//***************************************************************************/
	bool TCPRoomIn::Receive(size_t size)
	{
		uint8_t* buffer = new uint8_t[size + 1];
		ZeroMemory(buffer, size + 1);
		if (m_pcontroller->GetTcpSocket()->Receive(buffer, size) > 0)
		{
			DUNGEONDATA.SetCurrentFloor(buffer[8]);
			std::vector<uint8_t> roomOrder;
			for (int i = 9; i < size; i++)
			{
				roomOrder.push_back(buffer[i]);
			}
			delete[] buffer;
			m_pcontroller->NewRoom(roomOrder);
			return true;
		}
		delete[] buffer;
		return false;
	}

	/**************************************************************************//**
		@brief		入室命令送信関数
		@param[in]	data データ参照ポインタ
		@return		成功判断
	*//***************************************************************************/
	bool TCPRoomIn::Send(void* data)
	{
		std::vector<uint8_t> buffer;
		CreateHeaderBuffer(buffer, m_cmd);
		return m_pcontroller->GetTcpSocket()->Send(buffer.data(), buffer.size()) >= 0;
	}

	/**************************************************************************//**
		@brief		部屋作りの送信関数
		@param[in]	data データ参照ポインタ
		@return		成功判断
	*//***************************************************************************/
	bool TCPRoomNew::Send(void* data)
	{
		std::vector<uint8_t>* roomOrder = static_cast<std::vector<uint8_t>*>(data);

		std::vector<uint8_t> buffer;
		CreateHeaderBuffer(buffer, m_cmd, roomOrder->size());
		buffer.insert(buffer.end(), roomOrder->begin(), roomOrder->end());

		return m_pcontroller->GetTcpSocket()->Send(buffer.data(), buffer.size()) >= 0;
	}

	/**************************************************************************//**
		@brief		部屋作りの受信関数
		@param[in]	size データサイズ
		@return		成功判断
	*//***************************************************************************/
	bool TCPRoomNew::Receive(size_t size)
	{
		m_pcontroller->NewRoom();

		return true;
	}


	/**************************************************************************//**
		@brief		クライアント退室受信関数
		@param[in]	size データサイズ
		@return		成功判定
	*//***************************************************************************/
	bool TCPRoomOut::Receive(size_t size)
	{
		uint32_t client_id = 0;
		if (m_pcontroller->GetTcpSocket()->Receive(&client_id, size) > 0)
		{
			if (client_id == GAME_DATA.GetClientId()) return true;
			PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById(client_id);
			if (player != nullptr)
			{
				std::cout << static_cast<int>(player->GetClientId()) << " " << player->GetName() << reinterpret_cast <const char*>(u8" が 退室しました。") << std::endl;
				PlayerCharacterManager::Instance().Remove(client_id);
			}
			return true;
		}
		return false;
	}

	bool TCPRoomNext::Send(void* data)
	{

		std::vector<uint8_t>* roomOrder = static_cast<std::vector<uint8_t>*>(data);

		std::vector<uint8_t> buffer;
		CreateHeaderBuffer(buffer, m_cmd, roomOrder->size());
		buffer.insert(buffer.end(), roomOrder->begin(), roomOrder->end());

		return m_pcontroller->GetTcpSocket()->Send(buffer.data(), buffer.size()) >= 0;
	}
}
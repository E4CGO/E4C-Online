//! @file UDPSync.cpp
//! @note 


#include "UDPSync.h"
#include <iostream>
#include "TAKOEngine/Tool/U8Buffer.h"

#include "GameObject/Character/Player/PlayerCharacterManager.h"

namespace Online
{
	/**************************************************************************//**
		@brief		UDP同期受信処理
		@param[in]	buffer	受信したバッファ参照ポインタ
		@param[in]	size	データサイズ
		@return		成功判定
	*//***************************************************************************/
	bool UDPSync::Receive(uint8_t* buffer, size_t size)
	{
		uint64_t client_num = 0;
		std::memcpy(&client_num, buffer, sizeof(uint64_t));
		buffer += sizeof(uint64_t);
		PlayerCharacter::SYNC_DATA sync_data;
		for (size_t i = 0; i < client_num; i++)
		{
			std::memcpy(&sync_data, buffer, sizeof(sync_data));
			if (sync_data.client_id != m_pcontroller->GetId())
			{
				PlayerCharacterManager::Instance().SyncPlayer(sync_data.client_id, sync_data);
			}
			//std::cout << "Client: " << sync_data.client_id << " Sync Count: " << sync_data.sync_count_id << " pos x: " << sync_data.position[0] << " y: " << sync_data.position[1] << " z: " << sync_data.position[2] << " state: " << unsigned(sync_data.state) << "+" << unsigned(sync_data.sub_state) << std::endl;
			buffer += sizeof(sync_data);
		}

		return true;
	}
	/**************************************************************************//**
		@brief		UDP同期処理
		@param[in]	data	データ参照ポインタ
		@return		なし
	*//***************************************************************************/
	void UDPSync::Send(void* data)
	{
		PlayerCharacter::SYNC_DATA syncData;
		PlayerCharacterManager::Instance().GetPlayerCharacterById()->GetSyncData(syncData);


		std::vector<uint8_t> buffer;
		CreateHeaderBuffer(buffer, m_cmd, sizeof(PlayerCharacter::SYNC_DATA));

		U8Buffer::Insert(buffer, syncData.sync_count_id);
		for (int i = 0; i < 3; i++)
		{
			U8Buffer::Insert(buffer, syncData.position[i]);
		}
		for (int i = 0; i < 3; i++)
		{
			U8Buffer::Insert(buffer, syncData.velocity[i]);
		}
		U8Buffer::Insert(buffer, syncData.rotate);
		U8Buffer::Insert(buffer, syncData.state);
		U8Buffer::Insert(buffer, syncData.sub_state);

		m_pcontroller->GetUdpSocket()->Send(buffer.data(), buffer.size());

		//std::cout << "Sync: ";
		//for (uint8_t& t : buffer)
		//{
		//	std::cout << (unsigned)t << ",";
		//}
		//std::cout << std::endl;
	}
}

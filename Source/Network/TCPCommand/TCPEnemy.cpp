//! @file TCPEnemy.cpp
//! @note 

#include "TCPEnemy.h"
#include "TAKOEngine/Tool/U8Buffer.h"
#include "GameObject/Props/SpawnerManager.h"

namespace Online
{
	bool TCPEnemyNew::Receive(size_t size)
	{
		RECV_DATA recvData;
		if (m_pcontroller->GetTcpSocket()->Receive(&recvData, sizeof(RECV_DATA)) > 0)
		{
			Spawner* spawner = SpawnerManager::Instance().Get(recvData.spawnerIndex);
			if (spawner == nullptr) return false;

			for (uint64_t i = 0; i < recvData.enemyNum; i++)
			{
				uint32_t enemy_id = 0;
				if (m_pcontroller->GetTcpSocket()->Receive(&enemy_id, sizeof(uint32_t)) > 0)
				{
					Enemy* enemy = spawner->Spawn();
					enemy->SetEnemyId(enemy_id);
				}
				else
				{
					return false;
				}
			}
			return true;
		}
		return false;
	}
	bool TCPEnemyNew::Send(void* data)
	{
		SEND_DATA* sendData = static_cast<SEND_DATA*>(data);

		std::vector<uint8_t> buffer;
		CreateHeaderBuffer(buffer, m_cmd, sizeof(sendData));
		U8Buffer::Insert(buffer, sendData->spawnerIndex);
		U8Buffer::Insert(buffer, sendData->enemyType);
		U8Buffer::Insert(buffer, sendData->count);

		return m_pcontroller->GetTcpSocket()->Send(buffer.data(), buffer.size()) >= 0;
	}
}

//! @file TCPEnemy.cpp
//! @note 

#include "TCPEnemy.h"
#include "TAKOEngine/Tool/U8Buffer.h"
#include "GameObject/Props/SpawnerManager.h"
#include "GameObject/Character/Enemy/EnemyManager.h"

namespace Online
{
	/**************************************************************************//**
	 	@brief		エネミー生成受信関数
		@param[in]	size データサイズ
		@return		成功判定
	*//***************************************************************************/
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
					enemy->SetIsMine();
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
	/**************************************************************************//**
		@brief		エネミー生成送信関数
		@param[in]	data データ参照ポインタ
		@return		成功判定
	*//***************************************************************************/
	bool TCPEnemyNew::Send(void* data)
	{
		SEND_DATA* sendData = static_cast<SEND_DATA*>(data);

		std::vector<uint8_t> buffer;
		CreateHeaderBuffer(buffer, m_cmd, sizeof(SEND_DATA));
		U8Buffer::Insert(buffer, sendData->spawnerIndex);
		U8Buffer::Insert(buffer, sendData->enemyType);
		U8Buffer::Insert(buffer, sendData->count);

		return m_pcontroller->GetTcpSocket()->Send(buffer.data(), buffer.size()) >= 0;
	}
	/**************************************************************************//**
	 	@brief		エネミー同期受信関数
		@param[in]	size データサイズ
		@return		成功判定
	*//***************************************************************************/
	bool TCPEnemySync::Receive(size_t size)
	{
		uint64_t enemy_number = 0;
		if (m_pcontroller->GetTcpSocket()->Receive(&enemy_number, sizeof(uint64_t)) > 0)
		{
			for (uint64_t i = 0; i < enemy_number; i++)
			{
				Enemy::SYNC_DATA enemyData;
				if (m_pcontroller->GetTcpSocket()->Receive(&enemyData, sizeof(enemyData)) > 0)
				{
					Enemy* enemy = ENEMIES.GetEnemyById(enemyData.enemy_id);
					if (enemy == nullptr)
					{
						enemy = ENEMIES.Register(Enemy::EnemyFactory(enemyData.type));
					}

					enemy->Sync(enemyData);
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
	/**************************************************************************//**
	 	@brief		エネミー同期送信関数
		@param[in]	data データ参照ポインタ
		@return		成功判定
	*//***************************************************************************/
	bool TCPEnemySync::Send(void* data)
	{
		std::vector<Enemy::SYNC_DATA>* sendData = static_cast<std::vector<Enemy::SYNC_DATA>*>(data);

		std::vector<uint8_t> buffer;

		CreateHeaderBuffer(buffer, m_cmd, sizeof(uint32_t) + sizeof(Enemy::SYNC_DATA) * sendData->size());
		// 数
		U8Buffer::Insert(buffer, static_cast<uint32_t>(sendData->size()));

		for (const Enemy::SYNC_DATA& data : *sendData)
		{
			U8Buffer::Insert(buffer, data.enemy_id);
			U8Buffer::Insert(buffer, data.type);
			U8Buffer::Insert(buffer, data.position[0]);
			U8Buffer::Insert(buffer, data.position[1]);
			U8Buffer::Insert(buffer, data.position[2]);
			U8Buffer::Insert(buffer, data.target_id);
			U8Buffer::Insert(buffer, data.targetPosition[0]);
			U8Buffer::Insert(buffer, data.targetPosition[1]);
			U8Buffer::Insert(buffer, data.targetPosition[2]);
			U8Buffer::Insert(buffer, data.rotate);
			U8Buffer::Insert(buffer, data.state);
		}

		return m_pcontroller->GetTcpSocket()->Send(buffer.data(), buffer.size()) >= 0;
	}

	bool TCPEnemyRemove::Send(void* data)
	{
		std::set<uint32_t>* sendData = static_cast<std::set<uint32_t>*>(data);

		std::vector<uint8_t> buffer;

		CreateHeaderBuffer(buffer, m_cmd, sizeof(uint32_t) + sizeof(uint32_t) * sendData->size());
		// 数
		U8Buffer::Insert(buffer, static_cast<uint32_t>(sendData->size()));
		for (const uint32_t& enemy_id : *sendData)
		{
			U8Buffer::Insert(buffer, enemy_id);
		}
		return m_pcontroller->GetTcpSocket()->Send(buffer.data(), buffer.size()) >= 0;
	}

	/**************************************************************************//**
	 	@brief		エネミーオーナー変換受信関数
		@param[in]	size データサイズ
		@return		成功判定
	*//***************************************************************************/
	bool TCPEnemyOwner::Receive(size_t size)
	{
		uint64_t enemy_number = 0;
		if (m_pcontroller->GetTcpSocket()->Receive(&enemy_number, sizeof(uint64_t)) > 0)
		{
			uint32_t enemy_id = 0;
			for (uint64_t i = 0; i < enemy_number; i++)
			{
				if (m_pcontroller->GetTcpSocket()->Receive(&enemy_id, sizeof(uint32_t)) > 0)
				{
					Enemy* enemy = ENEMIES.GetEnemyById(enemy_id);
					if (enemy != nullptr)
					{
						enemy->SetIsMine(true);
					}
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
}

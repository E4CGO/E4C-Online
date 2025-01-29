//! @file UDPHit.cpp
//! @note 

#include "UDPHit.h"
#include "TAKOEngine/Tool/U8Buffer.h"
#include "GameObject/Character/Enemy/EnemyManager.h"

namespace Online
{
	/**************************************************************************//**
		@brief		UDP攻撃判定コールバック受信
		@param[in]	buffer
		@param[in]	size
		@return
	*//***************************************************************************/
	bool UDPHitSend::Receive(uint8_t* buffer, size_t size)
	{
		uint64_t count = 0;
		std::memcpy(&count, buffer, sizeof(uint64_t));
		buffer += sizeof(uint64_t);

		std::vector<uint32_t> acceptedHit;
		for (uint64_t i = 0; i < count; i++)
		{
			uint32_t hit_id = 0;
			std::memcpy(&hit_id, buffer, sizeof(uint32_t));
			buffer += sizeof(uint32_t);
			acceptedHit.push_back(hit_id);
		}

		m_pcontroller->RemoveHit(acceptedHit);
		return true;
	}
	/**************************************************************************//**
		@brief		UDP攻撃判定送信
		@param[in]	data	ヒット結果
	*//***************************************************************************/
	void UDPHitSend::Send(void* data)
	{
		std::vector<Controller::HIT_DATA>* dataList = static_cast<std::vector<Controller::HIT_DATA>*>(data);

		std::vector<uint8_t> buffer;
		CreateHeaderBuffer(buffer, m_cmd, sizeof(uint64_t) + sizeof(Controller::HIT_DATA) * dataList->size());
		U8Buffer::Insert(buffer, dataList->size());
		for (const Controller::HIT_DATA& data : *dataList)
		{
			U8Buffer::Insert(buffer, data.hit_id);
			U8Buffer::Insert(buffer, data.enemy_id);
			U8Buffer::Insert(buffer, data.damage);
		}

		m_pcontroller->GetUdpSocket()->Send(buffer.data(), buffer.size());
	}


	/**************************************************************************//**
		@brief		UDP攻撃判定受信
		@param[in]	buffer
		@param[in]	size
		@return
	*//***************************************************************************/
	bool UDPHitAccept::Receive(uint8_t* buffer, size_t size)
	{
		// ヒット数
		uint64_t count = 0;
		std::memcpy(&count, buffer, sizeof(uint64_t));
		buffer += sizeof(uint64_t);

		std::vector<uint8_t> callback;
		CreateHeaderBuffer(callback, m_cmd, sizeof(uint32_t) * 2 * count);
		U8Buffer::Insert(callback, count);

		for (uint64_t i = 0; i < count; i++)
		{
			uint32_t hit_id = 0;
			std::memcpy(&hit_id, buffer, sizeof(uint32_t));
			buffer += sizeof(uint32_t);

			uint32_t enemy_id = 0;
			std::memcpy(&enemy_id, buffer, sizeof(uint32_t));
			buffer += sizeof(uint32_t);

			uint16_t damage = 0;
			std::memcpy(&damage, buffer, sizeof(uint32_t));
			buffer += sizeof(uint16_t);

			if (!m_receivedHit.contains(enemy_id))
			{
				m_receivedHit[enemy_id] = 0;
			}

			if (hit_id > m_receivedHit[enemy_id])
			{
				Enemy* enemy = ENEMIES.GetEnemyById(enemy_id);
				if (enemy != nullptr)
				{
					enemy->OnDamage(damage);
				}
				m_receivedHit[enemy_id] = hit_id;
			}
			U8Buffer::Insert(callback, enemy_id);
			U8Buffer::Insert(callback, hit_id);
		}

		return m_pcontroller->GetUdpSocket()->Send(callback.data(), callback.size()) > 0;
	}
}

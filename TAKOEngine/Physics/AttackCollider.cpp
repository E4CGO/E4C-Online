//! @file AttackCollider.cpp
//! @note 

#include "AttackCollider.h"
#include "Source/GameObject/Character/Character.h"
#include "TAKOEngine/Editor/Camera/CameraManager.h"
#include "Source/GameObject/Character/Player/PlayerCharacter.h"
#include "Source/GameObject/Character/Player/PlayerCharacterManager.h"
void AttackSphereCollider::Update()
{
	SphereCollider::Update();

	if (!m_enable)
	{
		if (m_currentRate >= m_hitStartRate)
		{
			m_enable = true;
		}
	}
	if (m_enable)
	{
		if (m_currentRate >= m_hitEndRate)
		{
			m_enable = false;
		}
	}
}

void AttackSphereCollider::OnCollision(Collider* other)
{
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
	m_playerMp = player->GetMp();
	m_playerMaxMp = player->GetMaxMp(); // 最大MPを取得
	if (collisionFanction)
	{
		collisionFanction(this, other);
	}
	else
	{
		for (GameObject* owner : m_hitOthers)
		{
			// 既にヒットした敵には当たらない
			if (owner == other->GetOwner()) return;
		}

		if (m_power > other->GetArmor())
		{
			uint16_t damage = m_power - other->GetArmor();
			Character* chara = static_cast<Character*>(other->GetOwner());
			m_hitOthers.emplace_back(chara);
      
	    // 攻撃がヒットしたらカメラシェイクをリセット
	    CameraManager::Instance().GetCamera()->ResetShakeTimer();
        CameraManager::Instance().GetCamera()->SetShake(true);
		m_playerMp += 5;
		if (m_playerMp > m_playerMaxMp)
		{
			m_playerMp = m_playerMaxMp;
		}
		player->SetCurrentMp(m_playerMp);
		player->SetHitStop(true);


			chara->OnDamage(damage);
		}
	}
}

void AttackCapsuleCollider::Update()
{
	CapsuleCollider::Update();

	if (!m_enable)
	{
		if (m_currentRate >= m_hitStartRate)
		{
			m_enable = true;
		}
	}
	if (m_enable)
	{
		if (m_currentRate >= m_hitEndRate)
		{
			m_enable = false;
		}
	}
}

void AttackCapsuleCollider::OnCollision(Collider* other)
{
	for (GameObject* owner : m_hitOthers)
	{
		// 既にヒットした敵には当たらない
		if (owner == other->GetOwner()) return;
	}

	if (m_power > other->GetArmor())
	{
		uint16_t damage = m_power - other->GetArmor();
		Character* chara = static_cast<Character*>(other->GetOwner());
		m_hitOthers.emplace_back(chara);
		chara->OnDamage(damage);
	}
}
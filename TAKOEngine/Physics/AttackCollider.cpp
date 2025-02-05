//! @file AttackCollider.cpp
//! @note 

#include "AttackCollider.h"
#include "Source/GameObject/Character/Character.h"
#include "TAKOEngine/Editor/Camera/CameraManager.h"
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
			// 攻撃がヒットしたらカメラシェイクをリセット
			CameraManager::Instance().GetCamera()->ResetShakeTimer();
			CameraManager::Instance().GetCamera()->SetShake(true);

			uint16_t damage = m_power - other->GetArmor();
			ModelObject* owner = static_cast<ModelObject*>(other->GetOwner());
			m_hitOthers.emplace_back(owner);
     
			owner->OnDamage(damage);
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

void PlayerNormalAttackSphereCollider::Update()
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

void PlayerNormalAttackSphereCollider::OnCollision(Collider* other)
{
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
	int playerMp = player->GetMp();
	int playerMaxMp = player->GetMaxMp(); // 最大MPを取得
	if (collisionFanction)
	{
		collisionFanction(this, other);
	}
	else
	{
		for (Enemy* owner : m_hitOthers)
		{
			// 既にヒットした敵には当たらない
			if (owner == other->GetOwner()) return;
		}
		player->SetCurrentMp(playerMp);
		player->SetHitStop(true);

		if (m_power > other->GetArmor())
		{
			uint16_t damage = m_power - other->GetArmor();
			Enemy* chara = static_cast<Enemy*>(other->GetOwner());
			m_hitOthers.emplace_back(chara);

			// 攻撃がヒットしたらカメラシェイクをリセット
			CameraManager::Instance().GetCamera()->ShakeStart(0.2f, 0.1f);
			playerMp += 5;
			if (playerMp > playerMaxMp)
			{
				playerMp = playerMaxMp;
			}
			player->SetCurrentMp(playerMp);
			player->SetHitStop(true);
			chara->OnDamage(damage);
		}
	}
}
void PlayerSkill1AttackSphereCollider::Update()
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

void PlayerSkill1AttackSphereCollider::OnCollision(Collider* other)
{
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
	if (collisionFanction)
	{
		collisionFanction(this, other);
	}
	else
	{
		for (Enemy* owner : m_hitOthers)
		{
			// 既にヒットした敵には当たらない
			if (owner == other->GetOwner()) return;
		}

		if (m_power > other->GetArmor())
		{
			uint16_t damage = m_power - other->GetArmor();
			Enemy* chara = static_cast<Enemy*>(other->GetOwner());
			m_hitOthers.emplace_back(chara);

			// 攻撃がヒットしたらカメラシェイクをリセット
			CameraManager::Instance().GetCamera()->ShakeStart(0.5f, 0.1f);
			player->SetHitStop(true);
			chara->OnDamage(damage);
		}
	}
}



void AttackCapsuleCollider::OnCollision(Collider* other)
{
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
			chara->OnDamage(damage);
		}
	}
}
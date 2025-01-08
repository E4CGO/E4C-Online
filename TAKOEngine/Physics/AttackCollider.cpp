//! @file AttackCollider.cpp
//! @note 

#include "AttackCollider.h"
#include "Source/GameObject/Character/Character.h"

void AttackSphereCollider::Update()
{
	Collider::Update();

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
	for (GameObject* owner : m_hitOthers)
	{
		// 既にヒットした敵には当たらない
		if (owner == other->GetOwner()) return;
	}

	Character* chara = static_cast<Character*>(other->GetOwner());
	m_hitOthers.emplace_back(chara);
	chara->OnDamage(m_power);
}

void AttackCapsuleCollider::Update()
{
	Collider::Update();

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

	Character* chara = static_cast<Character*>(other->GetOwner());
	m_hitOthers.emplace_back(chara);
	chara->OnDamage(m_power);
}
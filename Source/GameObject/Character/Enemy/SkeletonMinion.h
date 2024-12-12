#pragma once

#include "Enemy.h"
#include "EnemyState.h"

class SkeletonMinion : public Enemy
{
public:
	SkeletonMinion(float scaling = 1.5f);
	virtual ~SkeletonMinion();
public:
	enum COLLIDER_ID : uint8_t
	{
		COL_BODY,
		COL_HEAD,
		COL_LEFT_HAND,
		COL_RIGHT_HAND,
	};

	enum AttackCollider
	{
		LeftHand,
		RightHand
	};
protected:
	enum HitCollider
	{
		Body,
		Head
	};
	enum State
	{
		Attack = EnemyState::ID::End,
	};

	void UpdateTarget();
};

class SkeletonMinionBoss : public SkeletonMinion
{
public:
	SkeletonMinionBoss();

	void Update(float elaspedTime) override;

	void OnDamage(const ENEMY_COLLISION& hit);
private:
	int armorMaxHp;
	int armorHp;
	float recoverArmorTime = 5.0f;
	float recoverArmorTimer = 0.0f;
};
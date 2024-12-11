#pragma once

#include "Enemy.h"
#include "EnemyState.h"

class SkeletonMinion : public Enemy
{
public:
	SkeletonMinion(float scaling = 1.5f);
	~SkeletonMinion() = default;
public:
	enum COLLIDER : uint8_t
	{
		BODY,
		HEAD,
		LEFT_HAND,
		RIGHT_HAND
	};
protected:
	enum STATE
	{
		ATTACK = enemy::STATE::END,
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
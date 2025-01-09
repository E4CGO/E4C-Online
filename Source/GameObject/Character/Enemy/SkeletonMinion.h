#pragma once

#include "Enemy.h"
#include "EnemyState.h"

class SkeletonMinion : public Enemy
{
public:
	SkeletonMinion(float scaling = 1.5f);
	~SkeletonMinion() = default;
public:
	enum Animation
	{
		Attack_1H,
		Attack_Combo,
		Attack_Spinning,
		Base,
		Block,
		Cheer,
		Climbing,
		Dance,
		Dash_Back,
		Dash_Front,
		Dash_Left,
		Dash_Right,
		Defeat,
		Heavy_Attack,
		Hop,
		Idle,
		Interact,
		Jump,
		Laying_Down_Idle,
		Pick_Up,
		Roll,
		Run,
		Shoot_1H,
		Shoot_2H,
		Shoot_Bow,
		Shooting_1H,
		Shooting_2H,
		Throw,
		Walk,
		Wave
	};

	enum COLLIDER_ID : uint8_t
	{
		COL_BODY,
		COL_HEAD,
		COL_LEFT_HAND,
		COL_RIGHT_HAND,
	};

	enum STATE
	{
		TARGET_FOUND = Enemy::STATE::END,
		ATTACK
	};
protected:
	void UpdateTarget() override;
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
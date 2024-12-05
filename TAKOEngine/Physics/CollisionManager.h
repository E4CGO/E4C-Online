#pragma once

#include "TAKOEngine/Tool/Singleton.h"
#include "TAKOEngine/Physics/Collider.h"
#include "Source/Manager.h"

class CollisionManager : Manager<Collider>, public Singleton<CollisionManager>
{
	friend class Singleton<CollisionManager>;
private:
	//	PLAYER = 1 << 0,
	//	PLAYER_ATTACK = 1 << 1,
	//	PLAYER_BLOCK = 1 << 2,
	//	PLAYER_PROJECTILE = 1 << 3,
	//	ENEMY = 1 << 4,
	//	ENEMY_ATTACK = 1 << 5,
	//	ENEMY_PROJECTILE = 1 << 6,
	//	ITEM = 1 << 7,
	//	OBSTRUCTION = 1 << 8,
	enum CONTACT_TYPE : uint16_t
	{
		PLAYERvsPLAYER = Collider::COLLIDER_OBJ::PLAYER,
		PLAYERvsPLAYER_ATTACK = Collider::COLLIDER_OBJ::PLAYER | Collider::COLLIDER_OBJ::PLAYER_ATTACK,
		PLAYERvsENEMY_ATTACK = Collider::COLLIDER_OBJ::PLAYER | Collider::COLLIDER_OBJ::ENEMY_ATTACK,
		PLAYERvsENEMY_PROJECTILE = Collider::COLLIDER_OBJ::PLAYER | Collider::COLLIDER_OBJ::ENEMY_PROJECTILE,
		ENEMYvsPLAYER_ATTACK = Collider::COLLIDER_OBJ::ENEMY | Collider::COLLIDER_OBJ::PLAYER_ATTACK,
		ENEMYvsPLAYER_PROJECTILE = Collider::COLLIDER_OBJ::ENEMY | Collider::COLLIDER_OBJ::PLAYER_PROJECTILE,

		CONTACT_ERR = Collider::COLLIDER_OBJ::HIT_ERR
	};
private:
	CollisionManager() {};
	~CollisionManager() = default;
public:
	void Clear() override { items.clear(); }
	void Contacts();
	void Update(float elapsedTime);
private:
	void ContactAction(CONTACT_TYPE contactType, Collider* col1, Collider* col2);
	//void EnemiesVsEnemiesCollsiion();
	//void PlayerPositionAdjustment();
	//void ProjectileCollision();
	void PlayerAttackToEnemyCollision(Collider* col1, Collider* col2);
	void EnemyAttackToPlayerCollision(Collider* col1, Collider* col2);
};

#define COLLISION CollisionManager::Instance()
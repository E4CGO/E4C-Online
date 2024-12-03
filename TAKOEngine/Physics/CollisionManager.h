#pragma once

#include "TAKOEngine/Physics/Collider/Collider.h"
#include "Source/Manager.h"
#include "TAKOEngine/Tool/Singleton.h"

class CollisionManager : public Manager<Collider>, public Singleton<CollisionManager>
{
	friend class Singleton<CollisionManager>;
private:
	CollisionManager() {};
	~CollisionManager() = default;
public:
	void Update(float elapsedTime);
	void Clear() override { items.clear(); } 
private:
	void EnemiesVsEnemiesCollsiion();
	void PlayerPositionAdjustment();
	void ProjectileCollision();
	void PlayerAttackToEnemyCollision();
	void EnemyAttackToPlayerCollision();
};

#define COLLISION CollisionManager::Instance()
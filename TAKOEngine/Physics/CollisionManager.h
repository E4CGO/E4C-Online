#pragma once

#include "TAKOEngine/Tool/Singleton.h"

class CollisionManager : public Singleton<CollisionManager>
{
	friend class Singleton<CollisionManager>;
private:
	CollisionManager() {};
	~CollisionManager() = default;
public:
	void Update(float elapsedTime);
private:
	void EnemiesVsEnemiesCollsiion();
	void PlayerPositionAdjustment();
	void ProjectileCollision();
	void PlayerAttackToEnemyCollision();
	void EnemyAttackToPlayerCollision();
};

#define COLLISION CollisionManager::Instance()
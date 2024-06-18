#pragma once

#include "TAKOEngine/Tool/Singleton.h"

class Collision : public Singleton<Collision>
{
	friend class Singleton<Collision>;
private:
	Collision() {};
	~Collision() = default;
public:
	void Update(float elapsedTime);
private:
	void EnemiesVsEnemiesCollsiion();
	void PlayerPositionAdjustment();
	void ProjectileCollision();
	void PlayerAttackToEnemyCollision();
	void EnemyAttackToPlayerCollision();
};

#define COLLISION Collision::Instance()
#pragma once

#include "TAKOEngine\Tool\Singleton.h"

#include "GameObject\ObjectManager.h"
#include "GameObject\Character\Enemy\Enemy.h"

class EnemyManager : public ObjectManager<Enemy>, public Singleton<EnemyManager>
{
	friend class Singleton<EnemyManager>;
protected:
	EnemyManager() = default;
	~EnemyManager() = default;

public:
	Enemy* Register(Enemy* enemy) override;

	Enemy* GetEnemyById(int id);
	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit, bool camera = false);

	Enemy* UpdateOrCreate(ENEMY_DATA data);
private:
	int count = -1;
};

#define ENEMIES EnemyManager::Instance()
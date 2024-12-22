#pragma once

#include <DirectXMath.h>

#include "TAKOEngine/Tool/Singleton.h"

#include "Manager.h"
#include "GameObject/Character/Enemy/Enemy.h"

/*
クライアントの衝突判定ストック
結果をホストに送信する
*/

class CollisionDataManager : public Manager<ENEMY_COLLISION>, public Singleton<CollisionDataManager>
{
	friend class Singleton<CollisionDataManager>;
protected:
	CollisionDataManager() = default;
	~CollisionDataManager() = default;

public:
	ENEMY_COLLISION* Register(ENEMY_COLLISION* item) override;
	ENEMY_COLLISION* Register(Enemy* enemy, int colider_id, int damage, bool power, DirectX::XMFLOAT3 force = {});
	void RemoveById(int id);
private:
	static int CountId;
};

#define ENEMYCOLLISIONS CollisionDataManager::Instance()
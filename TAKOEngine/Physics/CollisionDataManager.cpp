#include "CollisionDataManager.h"

#include "TAKOEngine/Effects/EffectManager.h"

#include "GameData.h"

int CollisionDataManager::CountId = -1;

ENEMY_COLLISION* CollisionDataManager::Register(ENEMY_COLLISION* item)
{
	item->count = ++CountId;
	item->player_id = GAME_DATA.GetClientId();
	return Manager<ENEMY_COLLISION>::Register(item);
}
ENEMY_COLLISION* CollisionDataManager::Register(Enemy* enemy, int colider_id, int damage, bool power, DirectX::XMFLOAT3 force)
{
	ENEMY_COLLISION* data = new ENEMY_COLLISION;
	data->colider_id = colider_id;
	data->damage = damage;
	data->enemy_id = enemy->GetEnemyId();
	data->power = power;
	data->force = force;
	return ENEMYCOLLISIONS.Register(data);
}

void CollisionDataManager::RemoveById(int id)
{
	for (std::vector<ENEMY_COLLISION*>::iterator it = items.begin(); it != items.end(); it++) {
		ENEMY_COLLISION*& collision = *it;
		if (collision->count == id)
		{
			items.erase(it);
			return;
		}
	}
}
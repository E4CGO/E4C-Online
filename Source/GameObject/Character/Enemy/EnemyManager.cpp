#include "EnemyManager.h"

Enemy* EnemyManager::GetEnemyById(int id)
{
	for (Enemy*& enemy : items)
	{
		if (enemy->GetEnemyId() == id) return enemy;
	}

	return nullptr;
}

// レイキャスト
bool EnemyManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit, bool camera)
{
	HitResult temp;
	hit.distance = FLT_MAX;
	for (Enemy*& item : items)
	{
		for (std::pair<int, Collider*>collider : item->GetColliders())
		{
			if (collider.second->RayCast(start, end, temp) && (temp.distance < hit.distance))
			{
				hit = temp; // 最短
			}
		}
	}
	return hit.distance < FLT_MAX;
}
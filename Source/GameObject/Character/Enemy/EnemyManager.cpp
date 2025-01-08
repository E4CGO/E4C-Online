#include "EnemyManager.h"

void EnemyManager::Update(float elapsedTime)
{
	ObjectManager::Update(elapsedTime);

	// エネミー同士の衝突処理
	size_t size = this->items.size();
	HitResult hit;
	for (size_t i = 0; i < size; i++)
	{
		Collider* col1 = items.at(i)->GetMoveCollider();
		if (!col1) continue;

		for (size_t j = i + 1; j < size; j++)
		{
			Collider* col2 = items.at(j)->GetMoveCollider();
			if (!col2) continue;

			if (col1->Collision(col2, {}, hit))
			{
				items.at(i)->AddImpulse({ hit.normal.x * hit.distance, 0, hit.normal.z * hit.distance });
				items.at(j)->AddImpulse({ -hit.normal.x * hit.distance, 0, -hit.normal.z * hit.distance });
			}
		}
	}
}

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
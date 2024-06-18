#include "Arrow.h"

#include "TAKOEngine/Physics/CollisionDataManager.h"
#include "TAKOEngine/Effects/EffectManager.h"

#include "GameObject/Character/Enemy/EnemyManager.h"
#include "GameObject/Character/Player/PlayerManager.h"

#include "Map/MapTileManager.h"

Arrow::Arrow(Character* owner) : Projectile("Data/Model/Object/arrow.gltf", 1.0f, owner)
{
	collisionTarget |= COLLISION_TARGET::ENEMY | COLLISION_TARGET::STAGE;

	atk = 7;
	speed = 40.0f;
	gravity = 0.0f;
	front = { 0.0f, -1.0f, 0.0f };
	pierce = false;

	force = 10.0f;
	coolTime = 0.1f;
}

void Arrow::Update(float elapsedTime)
{
	tempPosition = position; // 1フレーム前の位置更新

	Projectile::Update(elapsedTime); // 位置更新

	timer -= elapsedTime;
	if (timer < 0.0f)
	{
		Destory();
	}
}

void Arrow::Collision()
{
	Collider* col = collider.get();
	for (ModelObject*& map : MAPTILES.GetAll())
	{
		HitResult hit;
		DirectX::XMFLOAT3 direction = {};
		if (map->GetCollider()->RayCast(tempPosition, position, hit))
		{
			OnHitGround(hit);
			break;
		}
	}

	Enemy* hitTarget = nullptr;
	int hitCollider = -1;
	HitResult hit;
	for (Enemy*& enemy : ENEMIES.GetAll())
	{
		for (std::pair<int, Collider*> enemyCollider : enemy->GetColliders())
		{
			HitResult temp;
			if (enemyCollider.second->RayCast(tempPosition, position, temp)) // レイ衝突
			{
				if (temp.distance < hit.distance) // 一番接近計算
				{
					hit = temp;
					hitTarget = enemy;
					hitCollider = enemyCollider.first;
				}
				if (pierce) // 貫通処理：当たった敵全部
				{
					if (owner == PLAYERS.GetPlayerById(GAME_DATA.GetClientId())) // クライアントの攻撃しか処理しない
					{
						OnHitEnemy(hit);
						SendCollision(hitTarget, enemyCollider.first);
					}
				}
				continue; // 次の敵
			}
		}
	}
	if (hitTarget != nullptr && !pierce) // 非貫通処理：衝突の一番近い目標
	{
		if (owner == PLAYERS.GetPlayerById(GAME_DATA.GetClientId())) // クライアントの攻撃しか処理しない
		{
			OnHitEnemy(hit);
			SendCollision(hitTarget, hitCollider);
		}
		Destory();
	}
}

void Arrow::OnHitEnemy(HitResult& hit)
{
	EFFECTS.GetEffect(EffectManager::EFFECT_IDX::HIT_SHOT_EFFECT)->Play(hit.position, 0.5f);
}
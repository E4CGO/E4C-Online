#include "CollisionManager.h"

#include "TAKOEngine/Physics/CollisionDataManager.h"

#include "GameObject/Character/Player/PlayerCharacterManager.h"
#include "GameObject/Character/Enemy/EnemyManager.h"
#include "GameObject/Projectile/ProjectileManager.h"

void CollisionManager::Update(float elapsedTime)
{
	EnemiesVsEnemiesCollsiion();
	PlayerPositionAdjustment();

	ProjectileCollision();
	PlayerAttackToEnemyCollision();
	EnemyAttackToPlayerCollision();
}

/**
* 敵同士衝突判定
**/
void CollisionManager::EnemiesVsEnemiesCollsiion()
{
}

/**
* 自機位置調整
* 敵衝突判定
**/
void CollisionManager::PlayerPositionAdjustment()
{
	// 自機限定
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
	if (!player) return;
	DirectX::XMFLOAT3 position = player->GetPosition();
	Collider* playerCollider = player->GetCollider();

	// 敵衝突
	for (Enemy*& enemy : ENEMIES.GetAll())
	{
		for (const std::pair<int, Collider*>& collider : enemy->GetColliders())
		{
			HitResult hit;
			if (collider.second->Collision(playerCollider, {}, hit))
			{
				DirectX::XMFLOAT3 diff = hit.normal * (-hit.distance);
				//diff *= (collider.second->GetScale().x + playerCollider->GetScale().x - hit.distance);
				diff.y = 0.0f;
				position += diff;
				//playerCollider->SetPosition(playerCollider->GetPosition() + diff);

			}
		}
	}

	// マップ内の調整する￥
	// Y
	if (position.y < -15.0f)
	{
		position.y = 0.2f;
	}
	player->SetPosition(position);
	player->UpdateTransform();
}

void CollisionManager::ProjectileCollision()
{
	for (Projectile*& projectile : PROJECTILES.GetAll())
	{
		projectile->Collision();
	}
}

void CollisionManager::PlayerAttackToEnemyCollision()
{
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
	if (!player) return;	// 自機だけ

	player->AttackCollision();
}

/**
* 敵攻撃
* 自機だけ判断
* 結果はホストに送らない
* HP、ステートは同期
**/
void CollisionManager::EnemyAttackToPlayerCollision()
{
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
	if (!player) return;
	Collider* playerCollider = player->GetCollider();
	if (!playerCollider->IsEnable()) return;

	for (Enemy*& enemy : ENEMIES.GetAll())
	{
		enemy->AttackCollision();
	}
}
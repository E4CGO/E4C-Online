#include "CollisionManager.h"

#include "TAKOEngine/Physics/CollisionDataManager.h"

#include "GameObject/Character/Player/PlayerCharacterManager.h"
#include "GameObject/Character/Enemy/EnemyManager.h"
#include "GameObject/Projectile/ProjectileManager.h"

void CollisionManager::Contacts()
{
	for (int i = 0; i < items.size(); i++)
	{
		Collider* col1 = items[i];
		if (!col1->IsEnable()) continue;
		
		for (int j = i + 1; j < items.size(); j++)
		{
			Collider* col2 = items[j];
			if (!col2->IsEnable()) continue;

			uint16_t contactType = col1->GetOBJType() | col2->GetOBJType();

			if ((col1->GetHittableOBJ() & col2->GetOBJType()) || (col2->GetHittableOBJ() & col1->GetOBJType()))
			{
				HitResult hit;
				if (col1->Collision(col2, {}, hit))
				{
					ContactAction(static_cast<CONTACT_TYPE>(contactType), col1, col2);
				}
			}
		}
	}
}

void CollisionManager::Update(float elapsedTime)
{
	//EnemiesVsEnemiesCollsiion();
	//PlayerPositionAdjustment();

	//ProjectileCollision();
	//PlayerAttackToEnemyCollision();
	//EnemyAttackToPlayerCollision();
}

void CollisionManager::ContactAction(CONTACT_TYPE contactType, Collider* col1, Collider* col2)
{
	switch (contactType)
	{
	case CollisionManager::PLAYERvsPLAYER:
		break;
	case CollisionManager::PLAYERvsPLAYER_ATTACK:
		break;
	case CollisionManager::PLAYERvsENEMY_ATTACK:
		EnemyAttackToPlayerCollision(col1, col2);
		break;
	case CollisionManager::PLAYERvsENEMY_PROJECTILE:
		break;
	case CollisionManager::ENEMYvsPLAYER_ATTACK:
		PlayerAttackToEnemyCollision(col1, col2);
		break;
	case CollisionManager::ENEMYvsPLAYER_PROJECTILE:
		break;
	case CollisionManager::CONTACT_ERR:
		break;
	default:
		break;
	}
}

/**
* 敵同士衝突判定
**/
//void CollisionManager::EnemiesVsEnemiesCollsiion()
//{
//}

/**
* 自機位置調整
* 敵衝突判定
**/
//void CollisionManager::PlayerPositionAdjustment()
//{
//	// 自機限定
//	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
//	if (!player) return;
//	DirectX::XMFLOAT3 position = player->GetPosition();
//	Collider* playerCollider = player->GetCollider();
//
//	// 敵衝突
//	for (Enemy*& enemy : ENEMIES.GetAll())
//	{
//		for (const std::pair<int, Collider*>& collider : enemy->GetColliders())
//		{
//			HitResult hit;
//			if (collider.second->Collision(playerCollider, {}, hit))
//			{
//				DirectX::XMFLOAT3 diff = hit.normal * -1.0f;
//				//diff *= (collider.second->GetScale().x + playerCollider->GetScale().x - hit.distance);
//				diff.y = 0.0f;
//				position += diff;
//				//playerCollider->SetPosition(playerCollider->GetPosition() + diff);
//			}
//		}
//	}
//
//	// マップ内の調整する￥
//	// Y
//	if (position.y < -15.0f)
//	{
//		position.y = 0.2f;
//	}
//	player->SetPosition(position);
//	player->UpdateTransform();
//}

//void CollisionManager::ProjectileCollision()
//{
//	for (Projectile*& projectile : PROJECTILES.GetAll())
//	{
//		projectile->Collision();
//	}
//}

void CollisionManager::PlayerAttackToEnemyCollision(Collider* col1, Collider* col2)
{
	if (col1->GetOBJType() & Collider::COLLIDER_OBJ::PLAYER_ATTACK)
	{
		PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById(col1->GetID());
		Enemy* enemy = EnemyManager::Instance().GetEnemyById(col2->GetID());

		//enemy->OnDamage(player->GetAttackData());
		//player->PlayEffect();
	}
	else
	{
		PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById(col2->GetID());
		Enemy* enemy = EnemyManager::Instance().GetEnemyById(col1->GetID());

		//enemy->OnDamage(player->GetAttackData());
		//player->PlayEffect();
	}
}

/**
* 敵攻撃
* 自機だけ判断
* 結果はホストに送らない
* HP、ステートは同期
**/
void CollisionManager::EnemyAttackToPlayerCollision(Collider* col1, Collider* col2)
{
	if (col1->GetOBJType() & Collider::COLLIDER_OBJ::PLAYER)
	{
		PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById(col1->GetID());
		Enemy* enemy = EnemyManager::Instance().GetEnemyById(col2->GetID());

		//player->OnDamage(enemy->GetAttackData());
	}
	else
	{
		PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById(col2->GetID());
		Enemy* enemy = EnemyManager::Instance().GetEnemyById(col1->GetID());

		//player->OnDamage(enemy->GetAttackData());
	}
}
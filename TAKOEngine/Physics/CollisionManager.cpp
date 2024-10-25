#include "CollisionManager.h"

#include "TAKOEngine/Physics/CollisionDataManager.h"

#include "GameObject/Character/Player/PlayerManager.h"
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
* �G���m�Փ˔���
**/
void CollisionManager::EnemiesVsEnemiesCollsiion()
{
}

/**
* ���@�ʒu����
* �G�Փ˔���
**/
void CollisionManager::PlayerPositionAdjustment()
{
	// ���@����
	Player* player = PLAYERS.GetPlayerById(GAME_DATA.GetClientId());
	if (!player) return;
	DirectX::XMFLOAT3 position = player->GetPosition();
	Collider* playerCollider = player->GetCollider();

	// �G�Փ�
	for (Enemy*& enemy : ENEMIES.GetAll())
	{
		for (const std::pair<int, Collider*>& collider : enemy->GetColliders())
		{
			HitResult hit;
			if (collider.second->Collision(playerCollider, {}, hit))
			{
				DirectX::XMFLOAT3 diff = hit.normal * -1.0f;
				diff *= (collider.second->GetScale().x + playerCollider->GetScale().x - hit.distance);
				diff.y = 0.0f;
				position += diff;
				playerCollider->SetPosition(playerCollider->GetPosition() + diff);
			}
		}
	}

	// �}�b�v���̒������遏
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
	Player* player = PLAYERS.GetPlayerById(GAME_DATA.GetClientId());
	if (!player) return;	// ���@����

	player->AttackCollision();
}

/**
* �G�U��
* ���@�������f
* ���ʂ̓z�X�g�ɑ���Ȃ�
* HP�A�X�e�[�g�͓���
**/
void CollisionManager::EnemyAttackToPlayerCollision()
{
	Player* player = PLAYERS.GetPlayerById(GAME_DATA.GetClientId());
	if (!player) return;
	Collider* playerCollider = player->GetCollider();
	if (!playerCollider->IsEnable()) return;

	for (Enemy*& enemy : ENEMIES.GetAll())
	{
		enemy->AttackCollision();
	}
}
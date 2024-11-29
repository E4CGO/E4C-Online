#include "TAKOEngine/Tool/Mathf.h"

#include "GameObject/Character/Player/PlayerManager.h"
#include "GameObject/Character/Player/PlayerCharacterManager.h"
#include "GameObject/Character/Enemy/EnemyManager.h"
#include "GameObject/Character/Enemy/Enemy.h"
#include "GameObject/Character/Enemy/Chest.h"
#include "GameObject/Character/Enemy/SkeletonMinion.h"


Enemy::Enemy(const char* filename, float scaling) : Character(filename, scaling)
{
	//SetCollider(Collider::COLLIDER_TYPE::MODEL);

	stateMachine = new StateMachine<Enemy>;
	stateMachine->RegisterState(EnemyState::ID::Idle, new EnemyState::IdleState(this, 2.0f));
	stateMachine->RegisterState(EnemyState::ID::Hurt, new EnemyState::HurtState(this));
	stateMachine->RegisterState(EnemyState::ID::Death, new EnemyState::DeathState(this));
}

Enemy::~Enemy()
{
	delete stateMachine;
	for (const std::pair<int, Collider*>& collider : colliders)
	{
		delete collider.second;
	}
	for (const std::pair<int, Collider*>& collider : attackColliders)
	{
		delete collider.second;
	}
	colliders.clear();
	attackColliders.clear();
}

bool Enemy::MoveTo(float elapsedTime, const DirectX::XMFLOAT3& target)
{
	return Character::MoveTo(elapsedTime, target, moveSpeed, turnSpeed);
}

bool Enemy::IsAlive()
{
	if (hp >= 1)
	{
		return true;
	}
	return false;
}


void Enemy::TurnTo(float elapsedTime, const DirectX::XMFLOAT3& target)
{
	DirectX::XMFLOAT3 d = target - position;
	Turn(elapsedTime, d.x, d.z, turnSpeed);
}

PlayerCharacter* Enemy::GetClosestPlayer(float limit)
{
	PlayerCharacter* result = nullptr;
	limit *= limit;

	for (auto& player : PlayerCharacterManager::Instance().GetAll())
	{
		float d = XMFLOAT3LengthSq(player->GetPosition() - position);
		if (d < limit)
		{
			result = player;
			limit = d;
		}
	}

	return result;
}

void Enemy::Update(float elapsedTime)
{
	stateMachine->Update(elapsedTime);
	Character::Update(elapsedTime);
}

void Enemy::Render(const RenderContext& rc)
{
	Character::Render(rc);

#ifdef _DEBUG
	for (const std::pair<int, Collider*>& collider : colliders)
	{
		collider.second->DrawDebugPrimitive({ 1, 1, 1, 1 });
	}

	/*Collider* playerCollider = PLAYERS.GetPlayerById(GAME_DATA.GetClientId())->GetCollider();
	for (const std::pair<int, Collider*>& collider : attackColliders)
	{
		DirectX::XMFLOAT4 color = { 1, 0, 0, 1 };
		HitResult hit;
		if (collider.second->Collision(playerCollider, {}, hit)) color = { 0, 0, 1, 1 };

		collider.second->DrawDebugPrimitive(color);
	}*/
#endif // DEBUG
}
void Enemy::AttackCollision()
{
	PlayerCharacterManager& pMnager = PlayerCharacterManager::Instance();
	PlayerCharacter* player = pMnager.GetPlayerCharacterById(GAME_DATA.GetClientId());
	if (!player) return;
	Collider* playerCollider = pMnager.GetPlayerCharacterById(GAME_DATA.GetClientId())->GetCollider();
	if (!playerCollider->IsEnable()) return;

	for (const std::pair<int, Collider*>& collider : attackColliders)
	{
		HitResult hit;
		if (collider.second->Collision(playerCollider, {}, hit))
		{
			player->OnDamage(hit, atk);

		}
	}
}

void Enemy::OnDamage(const ENEMY_COLLISION& hit)
{
	hp -= hit.damage;
	if (hp > 0)
	{
		if (hit.power) stateMachine->ChangeState(EnemyState::ID::Hurt);
		velocity += hit.force;
	}
	else
	{
		stateMachine->ChangeState(EnemyState::ID::Death);
	}
}
void Enemy::OnDeath() { ENEMIES.Remove(this); }

void Enemy::ImportData(ENEMY_DATA data)
{
	PlayerCharacterManager& pMnager = PlayerCharacterManager::Instance();

	position = data.position;
	velocity = data.velocity;
	target = pMnager.GetPlayerCharacterById(data.target);
	angle = data.angle;
	if (stateMachine->GetStateIndex() != data.state)
	{
		stateMachine->ChangeState(data.state);
	}
	subState = data.subState;
	hp = data.hp;
	maxHp = data.maxHp;
}
void Enemy::ExportData(ENEMY_DATA& data)
{
 	data.enemy_id = enemy_id;
	data.enemyType = enemyType;
	data.position = position;
	data.velocity = velocity;
	data.target = (target) ? target->GetClientId() : -1;
	data.angle = angle;
	data.state = stateMachine->GetStateIndex();
	data.subState = subState;
	data.hp = hp;
	data.maxHp = maxHp;
}

Enemy* Enemy::EnemyFactory(int enemyType)
{
	switch (enemyType)
	{
	case ENEMY_TYPE::CHEST_COIN: return new VictoryChest; break;
	case ENEMY_TYPE::CHEST_EMPTY: return new EmptyChest; break;
	case ENEMY_TYPE::CHEST_EMPTY_SKELETON_MINION:
	{
		EmptyChest* enemy = new EmptyChest;
		enemy->SetOnOpen([chest = enemy](Enemy*) mutable {
			Enemy* sketelon = ENEMIES.Register(new SkeletonMinion);
			sketelon->SetPosition(chest->GetPosition());
			sketelon->SetAngle(chest->GetAngle());
			});
		return enemy;
		break;
	}
	case ENEMY_TYPE::SKELETON_MINION: return new SkeletonMinion; break;
	case ENEMY_TYPE::SKELETON_MINION_BOSS: return new SkeletonMinionBoss; break;
	}
	return nullptr;
}
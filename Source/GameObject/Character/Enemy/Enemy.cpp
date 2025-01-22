#include "TAKOEngine/Tool/Mathf.h"
#include "TAKOEngine/Physics/CollisionManager.h"
#include "GameObject/Character/Enemy/EnemyManager.h"
#include "GameObject/Character/Enemy/Enemy.h"
#include "GameObject/Character/Enemy/MouseMob.h"
#include "GameObject/Character/Enemy/BearBoss.h"
#include "GameObject/Props/Spawner.h"
#include "Network/OnlineController.h"

Enemy::Enemy(const char* filename, float scaling, ModelObject::RENDER_MODE renderMode) : Character(filename, scaling, renderMode)
{
	stateMachine = new StateMachine<Enemy>;
	stateMachine->RegisterState(Enemy::STATE::HURT, new EnemyState::HurtState(this));
	stateMachine->RegisterState(Enemy::STATE::DEATH, new EnemyState::DeathState(this));
}

Enemy::~Enemy()
{
	delete stateMachine;

	if (m_pSpawner != nullptr)
	{
		m_pSpawner->EnemyDestoryCallBack(this);
	}
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

	for (PlayerCharacter*& player : PlayerCharacterManager::Instance().GetAll())
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
	for (const std::pair<uint8_t, Collider*>& collider : m_pColliders)
	{
		collider.second->DrawDebugPrimitive({ 1, 1, 1, 1 });
	}
#endif // DEBUG
}
void Enemy::OnDamage(const uint16_t& damage)
{
	if (IsMine() || ONLINE_CONTROLLER->GetState() != Online::State::SYNC)
	{
		hp -= damage;
		if (hp > 0)
		{
			if (!superArmor)
			EnemyState::StateTransition(this, STATE::HURT);
		}
		else
		{
			EnemyState::StateTransition(this, STATE::DEATH);
		}
	}
	else
	{
		// 同期
		ONLINE_CONTROLLER->RegisterHit(enemy_id, damage);
	}
}

void Enemy::OnDeath()
{
	ENEMIES.Remove(this);
}

Enemy* Enemy::EnemyFactory(uint8_t enemyType)
{
	switch (enemyType)
	{
	case ENEMY_TYPE::MOUSE: return T_GRAPHICS.isDX11Active ? new MouseMob(0.5f, ModelObject::DX11) : new MouseMob(0.5f, ModelObject::DX12); break;
	case ENEMY_TYPE::BEAR_BOSS: return T_GRAPHICS.isDX11Active ? new BearBoss(1.0f, ModelObject::DX11) : new BearBoss(1.0f, ModelObject::DX12); break;
	}
	return nullptr;
}

bool Enemy::SearchPlayer()
{
	PlayerCharacter* target = PlayerCharacterManager::Instance().GetPlayerCharacterById(m_target);
	if (target == nullptr) return false;

	const DirectX::XMFLOAT3& closestPlayerPosition = target->GetPosition();

	float vx = closestPlayerPosition.x - position.x;
	float vy = closestPlayerPosition.y - position.y;
	float vz = closestPlayerPosition.z - position.z;
	float closestDist = sqrtf(vx * vx + vy * vy + vz * vz);

	if (closestDist < m_SearchRange)
	{
		float distXZ = sqrtf(vx * vx + vz * vz);
		// 単位ベクトル化
		vx /= distXZ;
		vz /= distXZ;

		// 方向ベクトル化
		float frontX = sinf(angle.y);
		float frontZ = cosf(angle.y);
		// 2つのベクトルの内積値で前後判定
		float dot = (frontX * vx) + (frontZ * vz);
		if (dot > 0.0f)
		{
			return true;
		}
	}
	return false;
}

/**************************************************************************//**
	@brief	同期用データ生成
	@return 同期用データ
*//***************************************************************************/
const Enemy::SYNC_DATA Enemy::SyncData()
{
	return SYNC_DATA{
	   enemy_id,
	   enemyType,
	   { position.x, position.y, position.z },
	   m_target,
	   { m_TargetPosition.x, m_TargetPosition.y, m_TargetPosition.z },
	   angle.y,
	   static_cast<uint8_t>(stateMachine->GetStateIndex())
	};
}
/**************************************************************************//**
	@brief		同期を入力
	@param[in]	data	同期データ
*//***************************************************************************/
void Enemy::Sync(const Enemy::SYNC_DATA& data)
{
	enemy_id = data.enemy_id;
	enemyType = data.type;
	position = { data.position[0],data.position[1], data.position[2] };
	m_target = data.target_id;
	m_TargetPosition = { data.targetPosition[0], data.targetPosition[1], data.targetPosition[2] };
	angle.y = data.rotate;
	stateMachine->ChangeState(data.state);
}

void Enemy::SetRandomMoveTargetPosition()
{
	float theta = Mathf::RandomRange(-DirectX::XM_PI, DirectX::XM_PI);
	float range = Mathf::RandomRange(0.0f, m_SearchRange);
	m_TargetPosition.x = this->m_SpawnPosition.x + sinf(theta) * range;
	m_TargetPosition.z = this->m_SpawnPosition.z + cosf(theta) * range;
}
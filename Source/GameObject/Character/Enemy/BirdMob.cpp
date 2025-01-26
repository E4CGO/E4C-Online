//! @file BirdMob.cpp
//! @note 

#include "BirdMob.h"
#include "BirdMobState.h"

BirdMob::BirdMob(float scaling, ModelObject::RENDER_MODE renderMode) : Enemy("Data/Model/Enemy/MDLANM_ENMbird_0120.glb", scaling, renderMode)
{
	enemyType = ENEMY_TYPE::BIRD;
	radius = 1.0f;
	maxHp = hp = 100;
	atk = 10;
	moveSpeed = 1.0f;
	turnSpeed = DirectX::XMConvertToRadians(90);
	gravity = 0.0f;
	isGround = true;

	SetSearchRange(10.0f);

	// 衝突判定

	// ステート
	{
		using namespace EnemyState::BirdMob;
		stateMachine->RegisterState(Enemy::STATE::IDLE, new IdleState(this));
		stateMachine->SetState(Enemy::STATE::IDLE);

		stateMachine->RegisterState(::Enemy::STATE::HURT, new HurtState(this));
		stateMachine->RegisterState(::Enemy::STATE::DEATH, new DeathState(this));

		stateMachine->RegisterState(STATE::WANDER, new MoveState(this));
		stateMachine->RegisterState(STATE::FOLLOW, new EnemyState::FollowState(this, 3.0f, STATE::ATTACK));

		stateMachine->RegisterState(STATE::ATTACK, new AttackState(this));
		stateMachine->RegisterSubState(STATE::ATTACK, ATTACK_STATE::ATTACK_MOVE, new AttackMoveState(this));
		stateMachine->RegisterSubState(STATE::ATTACK, ATTACK_STATE::ATTACK_DIVE, new AttackDiveState(this));
		stateMachine->RegisterSubState(STATE::ATTACK, ATTACK_STATE::ATTACK_LAND, new AttackLandingState(this));
		stateMachine->RegisterState(STATE::TAKE_OFF, new TakeOffState(this));
		stateMachine->RegisterState(STATE::ENCOUNTER, new EncounterState(this));
	}
}

void BirdMob::UpdateTarget()
{
	PlayerCharacter* player = GetHighestHateClient();
	if (player != nullptr)
	{
		m_target = player->GetClientId();
	}
	else
	{
		for (PlayerCharacter* player : PlayerCharacterManager::Instance().GetAll())
		{
			if (XMFLOAT3LengthSq(player->GetPosition() - position) <= m_SearchRangeSq)
			{
				if (InSight(player->GetPosition(), 60.0f))
				{
					AddHate(player->GetClientId(), 1);
					m_target = player->GetClientId();
				}
			}
		}
	}
}

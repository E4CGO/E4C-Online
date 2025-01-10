//! @file BearBoss.cpp
//! @note 

#include "BearBoss.h"
#include "BearBossState.h"


BearBoss::BearBoss(float scaling, ModelObject::RENDER_MODE renderMode) : Enemy("Data/Model/Enemy/MDLANM_ENMboss_1205.glb", scaling, renderMode)
{
	enemyType = ENEMY_TYPE::BEAR_BOSS;
	radius = 2.0f;
	maxHp = hp = 500;
	atk = 10;
	moveSpeed = 1.0f;
	turnSpeed = DirectX::XMConvertToRadians(90);

	{
		using namespace EnemyState::BearBoss;
		stateMachine->RegisterState(Enemy::STATE::IDLE, new EnemyState::BearBoss::IdleState(this));
		stateMachine->SetState(Enemy::STATE::IDLE);

		stateMachine->RegisterState(STATE::WANDER, new EnemyState::BearBoss::MoveState(this));
		
		stateMachine->RegisterState(STATE::ATTACK, new AttackState(this));
		stateMachine->RegisterSubState(STATE::ATTACK, ATTACK_STATE::FOLLOW, new AttackFollowState(this));
		stateMachine->RegisterSubState(STATE::ATTACK, ATTACK_STATE::PUNCH, new AttackPunchState(this));
	
	}
}


void BearBoss::UpdateTarget()
{

}

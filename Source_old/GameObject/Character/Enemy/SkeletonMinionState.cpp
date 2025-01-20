//! @file SkeletonMinionState.cpp
//! @note 

#include "SkeletonMinionState.h"

namespace SkeletonMinionState
{
	// 待機ステート
	void IdleState::Enter()
	{
		EnemyState::IdleState::Enter();
		owner->GetModel()->PlayAnimation(SkeletonMinion::Animation::Idle, false);
	}

	void IdleState::Execute(float elapsedTime)
	{
		owner->UpdateTarget();
		EnemyState::IdleState::Execute(elapsedTime);
		PlayerCharacter* target = PlayerCharacterManager::Instance().GetPlayerCharacterById(owner->GetTarget());
		if (target != nullptr)
		{
			owner->TurnTo(elapsedTime, target->GetPosition());
			if (!IsWaiting())
			{
				owner->GetStateMachine()->ChangeState(SkeletonMinion::STATE::TARGET_FOUND);
			}
		}
	}

	// 攻撃ステート
	void AttackState::Enter()
	{
		owner->GetModel()->PlayAnimation(SkeletonMinion::Animation::Attack_Combo, false);
	}
	void AttackState::Execute(float elapsedTime)
	{
		if (owner->GetModel()->GetAnimationRate() <= 0.5f)
		{
			owner->GetCollider(SkeletonMinion::COLLIDER_ID::COL_RIGHT_HAND)->SetEnable(true);
			owner->GetCollider(SkeletonMinion::COLLIDER_ID::COL_LEFT_HAND)->SetEnable(false);
		}
		else
		{
			owner->GetCollider(SkeletonMinion::COLLIDER_ID::COL_RIGHT_HAND)->SetEnable(false);
			owner->GetCollider(SkeletonMinion::COLLIDER_ID::COL_LEFT_HAND)->SetEnable(true);
		}

		if (!owner->IsPlayAnimation())
		{
			owner->GetStateMachine()->ChangeState(Enemy::STATE::IDLE);
		}
	}
	void AttackState::Exit()
	{
		owner->GetCollider(SkeletonMinion::COLLIDER_ID::COL_RIGHT_HAND)->SetEnable(false);
		owner->GetCollider(SkeletonMinion::COLLIDER_ID::COL_LEFT_HAND)->SetEnable(false);
	}
}
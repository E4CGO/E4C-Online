﻿//! @file SkeletonMinionState.cpp
//! @note 

#include "SkeletonMinionState.h"

// 攻撃ステート
void SkeletonMinionState::AttackState::Enter()
{
	owner->GetModel()->PlayAnimation(Enemy::Animation::Attack_Combo, false);
}
void SkeletonMinionState::AttackState::Execute(float elapsedTime)
{
	if (owner->GetModel()->GetAnimationRate() <= 0.5f)
	{
		owner->GetCollider(SkeletonMinion::COLLIDER::RIGHT_HAND)->SetEnable(true);
		owner->GetCollider(SkeletonMinion::COLLIDER::LEFT_HAND)->SetEnable(false);
	}
	else
	{
		owner->GetCollider(SkeletonMinion::COLLIDER::RIGHT_HAND)->SetEnable(false);
		owner->GetCollider(SkeletonMinion::COLLIDER::LEFT_HAND)->SetEnable(true);
	}

	if (!owner->IsPlayAnimation())
	{
		owner->GetStateMachine()->ChangeState(enemy::STATE::DEATH);
	}
}
void SkeletonMinionState::AttackState::Exit()
{
	owner->GetCollider(SkeletonMinion::COLLIDER::RIGHT_HAND)->SetEnable(false);
	owner->GetCollider(SkeletonMinion::COLLIDER::LEFT_HAND)->SetEnable(false);
}
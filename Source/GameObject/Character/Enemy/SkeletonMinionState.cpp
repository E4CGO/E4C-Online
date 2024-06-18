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
		owner->GetAttackColliders()[SkeletonMinion::AttackCollider::RightHand]->SetEnable(true);
		owner->GetAttackColliders()[SkeletonMinion::AttackCollider::LeftHand]->SetEnable(false);
	}
	else
	{
		owner->GetAttackColliders()[SkeletonMinion::AttackCollider::RightHand]->SetEnable(false);
		owner->GetAttackColliders()[SkeletonMinion::AttackCollider::LeftHand]->SetEnable(true);
	}

	if (!owner->GetModel()->IsPlayAnimation())
	{
		owner->GetStateMachine()->ChangeState(EnemyState::ID::Idle);
	}
}
void SkeletonMinionState::AttackState::Exit()
{
	owner->EnableAttackColliders(false);
}
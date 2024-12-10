//! @file EnemyState.cpp
//! @note 

#include "EnemyState.h"
#include "GameObject/Character/Player/PlayerCharacterManager.h"

namespace enemy
{
	// 待機ステート
	void IdleState::Enter()
	{
		if (owner->GetModel()->GetCurrentAnimationIndex() >= 0)
			owner->GetModel()->PlayAnimation(Enemy::Animation::Idle, true);
		waitTimer = waitTime;
	}
	void IdleState::Execute(float elapsedTime)
	{
		owner->UpdateTarget();
		waitTimer -= elapsedTime;

		if (owner->GetTarget() != nullptr)
		{
			owner->TurnTo(elapsedTime, owner->GetTarget()->GetPosition());
			if (waitTimer <= 0.0f)
			{
				owner->GetStateMachine()->ChangeState(STATE::TARGET_FOUND);
			}
		}
	}
	void IdleState::Exit()
	{
	}

	// 移動ステート
	void MoveState::Enter()
	{
		if (owner->GetModel()->GetCurrentAnimationIndex() >= 0)
			owner->GetModel()->PlayAnimation(Enemy::Animation::Walk, true);
	}
	void MoveState::Execute(float elapsedTime)
	{
		if (owner->MoveTo(elapsedTime, position))
		{
			owner->GetStateMachine()->ChangeState(nextState);
		}
	}
	void MoveState::Exit()
	{
	}

	// 追跡ステート
	void FollowState::Enter()
	{
		if (owner->GetModel()->GetCurrentAnimationIndex() >= 0)
			owner->GetModel()->PlayAnimation(Enemy::Animation::Walk, true);
	}
	void FollowState::Execute(float elapsedTime)
	{



		PlayerCharacter* target = owner->GetTarget();
		if (!target)
		{
			owner->GetStateMachine()->ChangeState(STATE::IDLE);
			return;
		}

		owner->MoveTo(elapsedTime, owner->GetTarget()->GetPosition());

		DirectX::XMFLOAT3 diff = owner->GetTarget()->GetPosition() - owner->GetPosition();
		diff.y = 0; // Y軸無視
		if (XMFLOAT3LengthSq(diff) < distance * distance)
		{
			owner->GetStateMachine()->ChangeState(nextState);
		}
	}
	void FollowState::Exit()
	{
	}

	// 怪我ステート
	void HurtState::Enter()
	{
		owner->SetAnimationSpeed(3.0f);
		if (owner->GetModel()->GetCurrentAnimationIndex() >= 0)
			owner->GetModel()->PlayAnimation(Enemy::Animation::Dash_Back, false);
	}
	void HurtState::Execute(float elapsedTime)
	{
		if (!owner->GetModel()->IsPlayAnimation())
		{
			owner->GetStateMachine()->ChangeState(STATE::IDLE);
		}
	}
	void HurtState::Exit()
	{
		owner->SetAnimationSpeed(1.0f);
	}

	// 死亡ステート
	void DeathState::Enter()
	{
		if (owner->GetModel()->GetCurrentAnimationIndex() >= 0)
			owner->GetModel()->PlayAnimation(Enemy::Animation::Defeat, false);
		for (std::pair<int, Collider*> collider : owner->GetColliders()) collider.second->SetEnable(false);
	}
	void DeathState::Execute(float elapsedTime)
	{
		if (!owner->GetModel()->IsPlayAnimation())
		{
			owner->OnDeath();
		}
	}
	void DeathState::Exit()
	{
	}
}
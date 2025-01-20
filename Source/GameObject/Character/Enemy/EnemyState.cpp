//! @file EnemyState.cpp
//! @note 

#include "EnemyState.h"
#include "GameObject/Character/Player/PlayerCharacterManager.h"

#include "Network/OnlineController.h"
#include "GameObject/Character/Enemy/EnemyManager.h"

namespace EnemyState
{
	/**************************************************************************//**
	 	@brief		エネミーステート変更関数・自動同期
		@param[in]	enemy	エネミー参照ポインタ
		@param[in]	state	ステート
	*//***************************************************************************/
	void StateTransition(Enemy* enemy, uint8_t state)
	{
		if (enemy->GetStateMachine()->GetState(state) == nullptr) return;

		if (!enemy->IsMine())
		{
			enemy->GetStateMachine()->ChangeState(Enemy::STATE::IDLE); // デフォルトステート
			return;
		}
		enemy->GetStateMachine()->ChangeState(state);

		if (ONLINE_CONTROLLER->GetState() != Online::Controller::STATE::OFFLINE)
		{
			// TODO
			ENEMIES.RegisterSync(enemy->GetEnemyId());
		}
	}

	// 待機ステート
	void IdleState::Enter()
	{
		m_waitTimer = m_waitTime;
	}
	void IdleState::Execute(float elapsedTime)
	{
		if (m_waitTimer == 0.0f) return;

		m_waitTimer -= elapsedTime;
		if (m_waitTimer <= 0.0f)
		{
			m_waitTimer = 0.0f;
		}
	}
	void IdleState::Exit()
	{
	}

	// 移動ステート
	void MoveState::Enter()
	{
	}
	void MoveState::Execute(float elapsedTime)
	{
		if (owner->MoveTo(elapsedTime, owner->GetTargetPosition()))
		{
			StateTransition(owner, static_cast<uint8_t>(m_nextState));
		}
	}
	void MoveState::Exit()
	{
	}

	// 追跡ステート
	void FollowState::Enter()
	{
	}
	void FollowState::Execute(float elapsedTime)
	{
		PlayerCharacter* target = PlayerCharacterManager::Instance().GetPlayerCharacterById(owner->GetTarget());
		if (target == nullptr)
		{
			StateTransition(owner, Enemy::STATE::IDLE);
			return;
		}

		owner->MoveTo(elapsedTime, target->GetPosition());

		if (XMFLOAT3HorizontalLength(target->GetPosition() - owner->GetPosition()) < m_distance * m_distance)
		{
			StateTransition(owner, static_cast<uint8_t>(m_nextState));
		}
	}
	void FollowState::Exit()
	{
	}

	// 怪我ステート
	void HurtState::Enter()
	{
	}
	void HurtState::Execute(float elapsedTime)
	{
		if (!owner->GetModel()->IsPlayAnimation())
		{
			StateTransition(owner, Enemy::STATE::IDLE);
		}
	}
	void HurtState::Exit()
	{
	}

	// 死亡ステート
	void DeathState::Enter()
	{
		owner->SetAnimation(0, false);
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
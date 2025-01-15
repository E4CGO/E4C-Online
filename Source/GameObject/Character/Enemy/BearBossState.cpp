//! @file BearBossState.cpp
//! @note 

#include "BearBossState.h"
#include "GameObject/Character/Player/PlayerCharacterManager.h"

namespace EnemyState
{
	namespace BearBoss
	{
		void IdleState::Enter()
		{
			owner->SetAnimation(::BearBoss::ANIMATION::ANIM_IDLE, true);
		}
		// ステートで実行するメソッド
		void IdleState::Execute(float elapsedTime)
		{
			if (!owner->IsMine()) return;
			EnemyState::IdleState::Execute(elapsedTime);

			if (!IsWaiting()) // 待機時間終わり
			{
				for (PlayerCharacter* player : PlayerCharacterManager::Instance().GetAll())
				{
					if (XMFLOAT3HorizontalLengthSq(player->GetPosition() - owner->GetPosition()) < 10.0f * 10.0f)
					{
						owner->AddHate(player->GetClientId(), 1);
					}
				}


				PlayerCharacter* player = owner->GetHighestHateClient();
				if (player != nullptr)
				{
					owner->SetTarget(player);
					EnemyState::StateTransition(owner, ::BearBoss::STATE::ATTACK);
				}
				else
				{

					owner->SetTarget(nullptr);
					owner->SetTargetPosition(GetRandomPointInCircleArea(owner->GetPosition(), 10.0f));
					EnemyState::StateTransition(owner, ::BearBoss::STATE::WANDER);
				}
			}
		}

		void MoveState::Enter()
		{
			::EnemyState::MoveState::Enter();
			owner->SetAnimation(::BearBoss::ANIMATION::ANIM_WALK_LOOP, true);
		}

		// 攻撃ステート
		void AttackState::Enter()
		{
			this->SetSubState(::BearBoss::ATTACK_STATE::FOLLOW);
		}
		void AttackState::Execute(float elapsedTime)
		{
			subState->Execute(elapsedTime);

			if (!owner->IsPlayAnimation())
			{
				EnemyState::StateTransition(owner, Enemy::STATE::IDLE);
			}
		}
		void AttackState::Exit() {}

		// 攻撃追跡ステート
		void AttackFollowState::Enter()
		{
			owner->SetAnimation(::BearBoss::ANIMATION::ANIM_WALK_LOOP, true);
		}
		void AttackFollowState::Execute(float elapsedTime)
		{
			PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById(owner->GetTarget());
			if (player != nullptr)
			{
				float distanceSq = XMFLOAT3LengthSq(owner->GetPosition() - player->GetPosition());

				if (distanceSq < owner->GetRadius() * owner->GetRadius() + 1.0f)
				{
					owner->GetStateMachine()->ChangeSubState(::BearBoss::ATTACK_STATE::PUNCH);
				}
				else
				{
					owner->MoveTo(elapsedTime, player->GetPosition());
				}
			}
			else
			{
				::EnemyState::StateTransition(owner, Enemy::STATE::IDLE);
			}

		}
		void AttackFollowState::Exit() {}

		// 攻撃パンチステート
		void AttackPunchState::Enter()
		{
			owner->SetAnimation(::BearBoss::ANIMATION::ANIM_PUNCH, false);
		}
		void AttackPunchState::Execute(float elapsedTime)
		{
			PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById(owner->GetTarget());
			if (player == nullptr)
			{
				::EnemyState::StateTransition(owner, Enemy::STATE::IDLE);
			}
		}
		void AttackPunchState::Exit() {}

	}
}

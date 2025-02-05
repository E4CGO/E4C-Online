//! @file BirdMobState.cpp
//! @note

#include "BirdMobState.h"
#include "TAKOEngine/Tool/Mathf.h"

namespace EnemyState
{
	namespace BirdMob
	{
		void IdleState::Enter()
		{
			owner->SetAnimation(::BirdMob::ANIM_IDLE, true);
			::EnemyState::IdleState::Enter();
		}
		void IdleState::Execute(float elapsedTime)
		{
			if (owner->IsGround())
			{
				if (!owner->IsPlayAnimation());
				EnemyState::StateTransition(owner, ::BirdMob::STATE::TAKE_OFF);
				return;
			}

			::EnemyState::IdleState::Execute(elapsedTime);
			uint32_t targetId = owner->GetTarget();
			owner->UpdateTarget();
			if (owner->GetTarget() < UINT32_MAX)
			{
				if (targetId == UINT32_MAX)
				{
					// 初索敵
					EnemyState::StateTransition(owner, ::BirdMob::STATE::ENCOUNTER);
				}
				else if (m_waitTimer == 0.0f)
				{
					// ターゲットに接近
					EnemyState::StateTransition(owner, ::BirdMob::STATE::FOLLOW);
				}
			}
			else if (m_waitTimer == 0.0f)
			{
				// 徘徊
				owner->SetTarget(nullptr);
				owner->SetTargetPosition(GetRandomPointInCircleArea(owner->GetPosition(), 5.0f));
				EnemyState::StateTransition(owner, ::BirdMob::STATE::WANDER);
			}
		}

		// エンカウンターステート
		void EncounterState::Enter()
		{
			owner->SetAnimation(::BirdMob::ANIM_ENCOUNTER, false);
		}
		void EncounterState::Execute(float elapsedTime)
		{
			if (!owner->IsPlayAnimation())
			{
				EnemyState::StateTransition(owner, ::BirdMob::STATE::FOLLOW);
				owner->SetAnimation(::BirdMob::ANIM_IDLE, true);
			}
		}

		// 徘徊ステート
		void MoveState::Enter()
		{
			owner->SetAnimation(::BirdMob::ANIM_IDLE, true);
		}
		void MoveState::Execute(float elapsedTime)
		{
			owner->UpdateTarget();
			if (owner->GetTarget() < UINT32_MAX)
			{
				// ターゲット発見
				EnemyState::StateTransition(owner, ::BirdMob::STATE::ENCOUNTER);
				return;
			}

			::EnemyState::MoveState::Execute(elapsedTime);
		}

		
		// 攻撃ステート
		void AttackState::Enter()
		{
			this->SetSubState(::BirdMob::ATTACK_MOVE);
			owner->OnSuperArmor();
		}
		void AttackState::Execute(float elapsedTime)
		{
			subState->Execute(elapsedTime);

			if (this->GetSubStateIndex() == ::BirdMob::ATTACK_LAND && !owner->IsPlayAnimation())
			{
				EnemyState::StateTransition(owner, Enemy::STATE::IDLE);
			}
		}
		void AttackState::Exit()
		{
			subState->Exit();
			owner->OffSuperArmor();
		}

		// 攻撃移動ステート
		void AttackMoveState::Enter()
		{
			m_startPos = owner->GetPosition();
			m_targetPos = PlayerCharacterManager::Instance().GetPlayerCharacterById(owner->GetTarget())->GetPosition();
			m_targetPos.y += 3.0f;
			owner->FaceTo(m_targetPos);
			owner->SetAnimation(::BirdMob::ANIM_ATTACK_1, false);
		}
		void AttackMoveState::Execute(float elapsedTime)
		{
			owner->Stop();
			float rate = owner->GetModel()->GetAnimationRate();

			owner->SetAnimationSpeed(1.0f + 2.0f * (1.0f - rate));
			//owner->SetPosition(Mathf::Lerp(m_startPos, m_targetPos, rate));
			owner->AddImpulse(m_targetPos - m_startPos);
			if (rate >= 1.0f)
			{
				owner->GetStateMachine()->ChangeSubState(::BirdMob::ATTACK_DIVE);
			}
		}
		void AttackMoveState::Exit()
		{
			owner->SetAnimationSpeed(1.0f);
		}

		// 攻撃ダイブステート
		void AttackDiveState::Enter()
		{
			owner->Jump(0.0f);
			owner->Stop();
			owner->SetGravity(-1.5f);
			owner->SetAnimation(::BirdMob::ANIM_ATTACK_2, true);

			ModelObject::ATTACK_COLLIDER_DATA attackData;
			attackData.power = birdAttack.power;
			attackData.idx = birdAttack.idx;
			attackData.objType = birdAttack.objType;
			attackData.hittableOBJ = birdAttack.hittableOBJ;
			attackData.hitStartRate = birdAttack.hitStartRate;
			attackData.hitEndRate = birdAttack.hitEndRate;
			owner->MakeAttackCollider(attackData, birdAttack.sphere, &owner->GetModel(0)->FindNode("JOT_C_Body")->worldTransform);
		}
		void AttackDiveState::Execute(float elapsedTime)
		{
			Collider* collider = owner->GetCollider(birdAttack.idx);
			if (collider != nullptr)
			{
				collider->SetCurrentRate(owner->GetModel()->GetAnimationRate());
			}

			if (owner->IsGround())
			{
				owner->GetStateMachine()->ChangeSubState(::BirdMob::ATTACK_LAND);
			}
		}
		void AttackDiveState::Exit()
		{
			owner->DeleteAttackCollider(birdAttack.idx);
			owner->SetGravity(0.0f);
		}

		// 攻撃着地ステート
		void AttackLandingState::Enter()
		{
			owner->SetAnimation(::BirdMob::ANIM_ATTACK_3, false);
		}
		void AttackLandingState::Exit()
		{
		}

		// 飛び上げステート
		void TakeOffState::Enter()
		{
			m_startPos = owner->GetPosition();
			owner->Jump(2.0f);
		}
		void TakeOffState::Execute(float elapsedTime)
		{
			if (owner->GetPosition().y - m_startPos.y >= 1.0f)
			{
				EnemyState::StateTransition(owner, ::Enemy::STATE::IDLE);
			}
		}
		void TakeOffState::Exit()
		{
			owner->Stop();
		}
	}
}
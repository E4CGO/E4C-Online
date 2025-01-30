//! @file MouseMobState.cpp
//! @note 

#include "MouseMobState.h"

#include "TAKOEngine/Tool/Mathf.h"

namespace EnemyState
{
	namespace MouseMob
	{
		/**************************************************************************//**
			@brief	待機ステートに入れる
		*//***************************************************************************/
		void IdleState::Enter()
		{
			owner->SetAnimation(::MouseMob::ANIM_IDLE, true, 0.1f);
			::EnemyState::IdleState::Enter();
		}

		/**************************************************************************//**
			@brief	待機ステートで実行する
			@param[in]    elapsedTime	経過時間
		*//***************************************************************************/
		void IdleState::Execute(float elapsedTime)
		{
			if (!owner->IsMine()) return;
			EnemyState::IdleState::Execute(elapsedTime);

			uint32_t targetId = owner->GetTarget();
			owner->UpdateTarget();
			if (owner->GetTarget() < UINT32_MAX)
			{
				if (targetId == UINT32_MAX)
				{
					// 
					EnemyState::StateTransition(owner, ::MouseMob::STATE::ENCOUNTER);
				}
				else if (!IsWaiting())
				{
					// ターゲットに接近
					EnemyState::StateTransition(owner, ::MouseMob::STATE::FOLLOW);
				}
			}
			else if (!IsWaiting())
			{
				// 徘徊
				owner->SetTarget(nullptr);
				owner->SetTargetPosition(GetRandomPointInCircleArea(owner->GetPosition(), 5.0f));
				EnemyState::StateTransition(owner, ::MouseMob::STATE::WANDER);
			}
		}

		/**************************************************************************//**
			@brief	エンカウントステートに入れる
		*//***************************************************************************/
		void EncounterState::Enter()
		{
			owner->SetAnimation(::MouseMob::ANIM_ENCOUNTER, false, 0.1f);
		}

		/**************************************************************************//**
			@brief	エンカウントステートで実行する
			@param[in]    elapsedTime	経過時間
		*//***************************************************************************/
		void EncounterState::Execute(float elapsedTime)
		{
			if (!owner->IsPlayAnimation())
			{
				EnemyState::StateTransition(owner, ::MouseMob::STATE::FOLLOW);
			}
		}

		// 徘徊ステート
		void MoveState::Enter()
		{
			owner->SetAnimation(::MouseMob::ANIM_MOVE, true);
		}
		void MoveState::Execute(float elapsedTime)
		{
			owner->UpdateTarget();
			if (owner->GetTarget() < UINT32_MAX)
			{
				// ターゲット発見
				EnemyState::StateTransition(owner, ::MouseMob::STATE::ENCOUNTER);
				return;
			}

			::EnemyState::MoveState::Execute(elapsedTime);
		}


		/**************************************************************************//**
			@brief	攻撃ステートに入れる
		*//***************************************************************************/
		void AttackState::Enter()
		{
			owner->OnSuperArmor();
			owner->SetAnimation(::MouseMob::ANIMATION::ANIM_ATTACK, false, 0.1f);

			ModelObject::ATTACK_COLLIDER_DATA attackData;
			attackData.power = mouseAttack.power;
			attackData.idx = mouseAttack.idx;
			attackData.objType = mouseAttack.objType;
			attackData.hittableOBJ = mouseAttack.hittableOBJ;
			attackData.hitStartRate = mouseAttack.hitStartRate;
			attackData.hitEndRate = mouseAttack.hitEndRate;
			owner->MakeAttackCollider(attackData, mouseAttack.capsule, &owner->GetModel(0)->FindNode("JOT_C_Body")->worldTransform);
		}

		/**************************************************************************//**
			@brief	攻撃ステートで実行する
			@param[in]    elapsedTime	経過時間
		*//***************************************************************************/
		void AttackState::Execute(float elapsedTime)
		{
			Collider* collider = owner->GetCollider(mouseAttack.idx);
			if (collider != nullptr)
			{
				collider->SetCurrentRate(owner->GetModel()->GetAnimationRate());
			}

			if (!owner->IsPlayAnimation())
			{
				EnemyState::StateTransition(owner, ::Enemy::STATE::IDLE);
			}
		}
		/**************************************************************************//**
		 	@brief	攻撃ステートから出る
		*//***************************************************************************/
		void AttackState::Exit()
		{
			owner->DeleteAttackCollider(mouseAttack.idx);
			owner->OffSuperArmor();
		}

		/**************************************************************************//**
			@brief	ダメージステート
		*//***************************************************************************/
		void HurtState::Enter()
		{
			owner->SetAnimation(::MouseMob::ANIMATION::ANIM_DAMAGE, false, 0.1f);
		}

		/**************************************************************************//**
			@brief	死亡ステート
		*//***************************************************************************/
		void DeathState::Enter()
		{
			EnemyState::DeathState::Enter();
			owner->SetAnimation(::MouseMob::ANIMATION::ANIM_DIE, false, 0.1f);
		}
	}
}
//! @file CrocodileMobState.cpp
//! @note 

#include "CrocodileMobState.h"
#include "TAKOEngine/Tool/Mathf.h"

namespace EnemyState
{
	namespace CrocodileMob
	{
		/**************************************************************************//**
			@brief	待機ステートに入れる
		*//***************************************************************************/
		void IdleState::Enter()
		{
			owner->SetAnimation(::CrocodileMob::ANIM_IDLE, true, 0.1f);
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

			if (!IsWaiting())
			{
				owner->UpdateTarget();
				if (owner->GetTarget() < UINT32_MAX)
				{

					// ターゲットに接近
					EnemyState::StateTransition(owner, ::CrocodileMob::STATE::FOLLOW);

				}
				else
				{
					// 徘徊
					owner->SetTarget(nullptr);
					owner->SetTargetPosition(GetRandomPointInCircleArea(owner->GetPosition(), 5.0f));
					EnemyState::StateTransition(owner, ::CrocodileMob::STATE::WANDER);
				}
			}
		}

		// 徘徊ステート
		void MoveState::Enter()
		{
			owner->SetAnimation(::CrocodileMob::ANIM_MOVE, true);
		}
		void MoveState::Execute(float elapsedTime)
		{
			owner->UpdateTarget();
			if (owner->GetTarget() < UINT32_MAX)
			{
				// ターゲット発見
				EnemyState::StateTransition(owner, ::CrocodileMob::STATE::FOLLOW);
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
			owner->SetAnimation(::CrocodileMob::ANIMATION::ANIM_ATTACK, false, 0.1f);

			ModelObject::ATTACK_COLLIDER_DATA attackData;
			attackData.power = crocodileAttack.power;
			attackData.idx = crocodileAttack.idx;
			attackData.objType = crocodileAttack.objType;
			attackData.hittableOBJ = crocodileAttack.hittableOBJ;
			attackData.hitStartRate = crocodileAttack.hitStartRate;
			attackData.hitEndRate = crocodileAttack.hitEndRate;
			owner->MakeAttackCollider(attackData, crocodileAttack.capsule, &owner->GetModel(0)->FindNode("Croc_root")->worldTransform);
		}

		/**************************************************************************//**
			@brief	攻撃ステートで実行する
			@param[in]    elapsedTime	経過時間
		*//***************************************************************************/
		void AttackState::Execute(float elapsedTime)
		{
			owner->GetCollider(crocodileAttack.idx)->SetCurrentRate(owner->GetModel()->GetAnimationRate());

			if (!owner->IsPlayAnimation())
			{
				EnemyState::StateTransition(owner, ::Enemy::STATE::IDLE);
			}
		}
		void AttackState::Exit()
		{
			owner->DeleteAttackCollider(crocodileAttack.idx);
			owner->OffSuperArmor();
		}

		/**************************************************************************//**
			@brief	ダメージステート
		*//***************************************************************************/
		void HurtState::Enter()
		{
			owner->SetAnimation(::CrocodileMob::ANIMATION::ANIM_DAMAGE, false, 0.1f);
		}

		/**************************************************************************//**
			@brief	死亡ステート
		*//***************************************************************************/
		void DeathState::Enter()
		{
			EnemyState::DeathState::Enter();
			owner->SetAnimation(::CrocodileMob::ANIMATION::ANIM_DIE, false, 0.1f);
		}
	}
}
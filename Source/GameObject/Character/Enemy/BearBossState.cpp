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
			EnemyState::IdleState::Enter();
			owner->SetAnimation(::BearBoss::ANIMATION::ANIM_IDLE, true);
		}
		void AngryIdleState::Enter()
		{
			IdleState::Enter();
			owner->SetAnimation(::BearBoss::ANIMATION::ANIM_IDLE2, true);
		}
		// ステートで実行するメソッド
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
					EnemyState::StateTransition(owner, ::BearBoss::STATE::ROAR);
				}
				else if (!IsWaiting())
				{
					// ターゲットに接近
					EnemyState::StateTransition(owner, ::BearBoss::STATE::FOLLOW);
				}
			}
			else if (!IsWaiting())
			{
				// 徘徊
				owner->SetTarget(nullptr);
				owner->SetTargetPosition(GetRandomPointInCircleArea(owner->GetPosition(), 5.0f));
				EnemyState::StateTransition(owner, ::BearBoss::STATE::WANDER);
			}
		}

		// 移動ステート
		void MoveState::Enter()
		{
			::EnemyState::MoveState::Enter();
			owner->SetAnimation(::BearBoss::ANIMATION::ANIM_WALK_LOOP, true);
		}
		void AngryMoveState::Enter()
		{
			MoveState::Enter();
			owner->SetAnimation(::BearBoss::ANIMATION::ANIM_WALK_LOOP2, true);
		}

		// 追跡ステート
		void FollowState::Enter()
		{
			PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById(owner->GetTarget());
			if (player != nullptr)
			{
				m_distance = XMFLOAT3HorizontalLength(player->GetPosition() - owner->GetPosition());
			}
			owner->SetAnimation(::BearBoss::ANIM_WALK_LOOP, true);
		}

		// 攻撃ステート
		void AttackState::Enter()
		{
			owner->SetAnimation(::BearBoss::ANIMATION::ANIM_PUNCH, false);

			attackArms[0].power = 50;
			attackArms[0].idx = ::BearBoss::COLLIDER_ID::COL_ATTACK_LEFT_HAND;
			attackArms[0].objType = Collider::COLLIDER_OBJ::ENEMY_ATTACK;
			attackArms[0].hittableOBJ = Collider::COLLIDER_OBJ::PLAYER;
			attackArms[0].capsule = { {0, 0, 0}, {1, 0, 0}, 2.0f, 1.4f };
			attackArms[0].hitStartRate = 159.0f / maxFlame;
			attackArms[0].hitEndRate = 168.0f / maxFlame;
			attackArms[1].power = 50;
			attackArms[1].idx = ::BearBoss::COLLIDER_ID::COL_ATTACK_RIGHT_HAND;
			attackArms[1].objType = Collider::COLLIDER_OBJ::ENEMY_ATTACK;
			attackArms[1].hittableOBJ = Collider::COLLIDER_OBJ::PLAYER;
			attackArms[1].capsule = { {0, 0, 0}, {-1, 0, 0}, 2.0f, 1.4f };
			attackArms[1].hitStartRate = 264.0f / maxFlame;
			attackArms[1].hitEndRate = 273.0f / maxFlame;

			DirectX::XMFLOAT4X4* leftArmMatrix = &owner->GetModel(0)->FindNode("JOT_L_Elbow")->worldTransform;
			DirectX::XMFLOAT4X4* rightArmMatrix = &owner->GetModel(0)->FindNode("JOT_R_Elbow")->worldTransform;

			ModelObject::ATTACK_COLLIDER_DATA attackData;
			attackData.power = attackArms[0].power;
			attackData.idx = attackArms[0].idx;
			attackData.objType = attackArms[0].objType;
			attackData.hittableOBJ = attackArms[0].hittableOBJ;
			attackData.hitStartRate = attackArms[0].hitStartRate;
			attackData.hitEndRate = attackArms[0].hitEndRate;
			owner->MakeAttackCollider(attackData, attackArms[0].capsule, leftArmMatrix);

			attackData.power = attackArms[1].power;
			attackData.idx = attackArms[1].idx;
			attackData.objType = attackArms[1].objType;
			attackData.hittableOBJ = attackArms[1].hittableOBJ;
			attackData.hitStartRate = attackArms[1].hitStartRate;
			attackData.hitEndRate = attackArms[1].hitEndRate;
			owner->MakeAttackCollider(attackData, attackArms[1].capsule, rightArmMatrix);
		}
		void AngryAttackState::Enter()
		{
			AttackState::Enter();
			owner->SetAnimation(::BearBoss::ANIMATION::ANIM_PUNCH2, false);
		}

		void AttackState::Execute(float elapsedTime)
		{
			owner->GetCollider(attackArms[0].idx)->SetCurrentRate(owner->GetModel()->GetAnimationRate());
			owner->GetCollider(attackArms[1].idx)->SetCurrentRate(owner->GetModel()->GetAnimationRate());

			UpdatePunchImpact(elapsedTime);

			PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById(owner->GetTarget());
			if (player == nullptr || !owner->IsPlayAnimation())
			{
				::EnemyState::StateTransition(owner, Enemy::STATE::IDLE);
			}
		}
		void AttackState::Exit()
		{
			owner->DeleteAttackCollider(attackArms[0].idx);
			owner->DeleteAttackCollider(attackArms[1].idx);

			impacts[0] = false;
			impacts[1] = false;
		}
		void AttackState::UpdatePunchImpact(float elapsedTime)
		{
			if (!impacts[0])
			{
				if (owner->GetModel()->GetAnimationRate() > impact1StartRate)
				{
					DirectX::XMFLOAT4X4* matrix = &owner->GetModel(0)->FindNode("JOT_L_Hand")->worldTransform;

					DirectX::XMFLOAT3 pos = { matrix->_41, matrix->_42 , matrix->_43 };

					PunchImpact* impact = new PunchImpact(pos, owner);
					PROJECTILES.Register(impact);
					impacts[0] = true;
				}
			}
			if (!impacts[1])
			{
				if (owner->GetModel()->GetAnimationRate() > impact2StartRate)
				{
					DirectX::XMFLOAT4X4* matrix = &owner->GetModel(0)->FindNode("JOT_R_Hand")->worldTransform;

					DirectX::XMFLOAT3 pos = { matrix->_41, matrix->_42 , matrix->_43 };

					PunchImpact* impact = new PunchImpact(pos, owner);
					PROJECTILES.Register(impact);
					impacts[1] = true;
				}
			}
		}

		// スタンステート
		void StunState::Enter()
		{
			this->SetSubState(::BearBoss::STUN_START);
		}
		void StunState::Execute(float elapsedTime)
		{
			this->subState->Execute(elapsedTime);
			if (!owner->IsPlayAnimation())
			{
				EnemyState::StateTransition(owner, ::Enemy::STATE::IDLE);
			}
		}
		void StunState::Exit()
		{
			this->subState->Exit();
		}

		void StunStartState::Enter()
		{
			owner->SetAnimation(::BearBoss::ANIM_STUN_START, false);
			// 弱点当たり判定消す
		}
		void AngryStunStartState::Enter()
		{
			StunStartState::Enter();
			owner->SetAnimation(::BearBoss::ANIM_STUN_START2, false);
		}
		void StunStartState::Execute(float elapsedTime)
		{
			if (!owner->IsPlayAnimation())
			{
				owner->GetStateMachine()->ChangeSubState(::BearBoss::STUN_STATE::STUN_LOOP);
			}
		}
		void StunStartState::Exit()
		{
			// 弱点当たり判定復活
		}
		void StunLoopState::Enter()
		{
			m_timer = 0.0f;
			owner->SetAnimation(::BearBoss::ANIM_STUN_LOOP, true);
		}
		void AngryStunLoopState::Enter()
		{
			StunLoopState::Enter();
			owner->SetAnimation(::BearBoss::ANIM_STUN_LOOP2, true);
		}
		void StunLoopState::Execute(float elapsedTime)
		{
			m_timer += elapsedTime;
			if (m_timer > m_time) {
				owner->GetStateMachine()->ChangeSubState(::BearBoss::STUN_STATE::STUN_END);
			}
		}
		void StunEndState::Enter()
		{
			owner->SetAnimation(::BearBoss::ANIM_STUN_END, false);
			// 弱点当たり判定消す
		}
		void AngryStunEndState::Enter()
		{
			StunEndState::Enter();
			owner->SetAnimation(::BearBoss::ANIM_STUN_END2, false);
		}
		void StunEndState::Exit()
		{
			// 弱点当たり判定復活
		}

		// 咆哮ステート
		void RoarState::Enter()
		{
			owner->SetAnimation(::BearBoss::ANIM_ROAR, false);
		}
		void AngryRoarState::Enter()
		{
			owner->SetAnimation(::BearBoss::ANIM_ROAR2, false);
		}
		void RoarState::Execute(float elapsedTime)
		{
			if (!owner->IsPlayAnimation())
			{
				EnemyState::StateTransition(owner, ::BearBoss::STATE::FOLLOW);
			}
		}

		// 形態変更
		void FormChangeState::Enter()
		{
			owner->SetAnimation(::BearBoss::ANIM_CHANGE, false);
			owner->SetAnimationSpeed(owner->GetAnimationSpeed() * 1.5f);
			owner->SetMaxMoveSpeed(owner->GetMaxMoveSpeed() * 1.5f);
			// 怒りのステート登録し直し
			StateMachine<Enemy>* stateMachine = owner->GetStateMachine();
			stateMachine->RegisterState(::Enemy::STATE::IDLE, new AngryIdleState(owner));
			stateMachine->RegisterState(::BearBoss::STATE::WANDER, new AngryMoveState(owner));
			stateMachine->RegisterState(::BearBoss::STATE::FOLLOW, new AngryFollowState(owner));
			stateMachine->RegisterState(::BearBoss::STATE::ATTACK, new AngryAttackState(owner));

			stateMachine->RegisterState(::BearBoss::STATE::ROAR, new AngryRoarState(owner));

			stateMachine->RegisterSubState(::BearBoss::STATE::STUN, ::BearBoss::STUN_STATE::STUN_START, new AngryStunStartState(owner));
			stateMachine->RegisterSubState(::BearBoss::STATE::STUN, ::BearBoss::STUN_STATE::STUN_LOOP, new AngryStunLoopState(owner));
			stateMachine->RegisterSubState(::BearBoss::STATE::STUN, ::BearBoss::STUN_STATE::STUN_END, new AngryStunEndState(owner));
			// 当たり判定消し
		}
		void FormChangeState::Execute(float elapsedTime)
		{
			if (!owner->IsPlayAnimation())
			{
				EnemyState::StateTransition(owner, ::Enemy::STATE::IDLE);
			}
		}
		void FormChangeState::Exit()
		{
			// 当たり判定消復活
		}
	}
}

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
		void AttackState::Exit()
		{
			subState->Exit();
		}

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

			attackArms[0].power = 50;
			attackArms[0].idx = ::BearBoss::COLLIDER_ID::COL_ATTACK_LEFT_HAND;
			attackArms[0].objType = Collider::COLLIDER_OBJ::ENEMY_ATTACK;
			attackArms[0].hittableOBJ = Collider::COLLIDER_OBJ::PLAYER;
			attackArms[0].capsule = { {0, 0, 0}, {1, 0, 0}, 2.0f, 1.4f };
			attackArms[1].power = 50;
			attackArms[1].idx = ::BearBoss::COLLIDER_ID::COL_ATTACK_RIGHT_HAND;
			attackArms[1].objType = Collider::COLLIDER_OBJ::ENEMY_ATTACK;
			attackArms[1].hittableOBJ = Collider::COLLIDER_OBJ::PLAYER;
			attackArms[1].capsule = { {0, 0, 0}, {-1, 0, 0}, 2.0f, 1.4f };

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
		void AttackPunchState::Execute(float elapsedTime)
		{
			owner->GetCollider(attackArms[0].idx)->SetCurrentRate(owner->GetModel()->GetAnimationRate());
			owner->GetCollider(attackArms[1].idx)->SetCurrentRate(owner->GetModel()->GetAnimationRate());

			PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById(owner->GetTarget());
			if (player == nullptr)
			{
				::EnemyState::StateTransition(owner, Enemy::STATE::IDLE);
			}
		}
		void AttackPunchState::Exit() 
		{
			owner->DeleteAttackCollider(attackArms[0].idx);
			owner->DeleteAttackCollider(attackArms[1].idx);
		}
	}
}

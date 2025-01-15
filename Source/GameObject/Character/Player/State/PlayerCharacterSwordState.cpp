//! @file PlayerCharacterSwordState.cpp
//! @note

#include "PlayerCharacterSwordState.h"
#include "PlayerCharacterState.h"

namespace PlayerCharacterState
{
	namespace Sword
	{
		ATTACK_SPHERE sphereAttacks[3] = {
		{10, PlayerCharacter::COLLIDER_ID::COL_ATTACK_1, Collider::COLLIDER_OBJ::PLAYER_ATTACK, Collider::COLLIDER_OBJ::ENEMY, 0.15f, 0.55f, {{0, 0.6f/0.005f, 0}, 0.5f}},
		{20, PlayerCharacter::COLLIDER_ID::COL_ATTACK_2, Collider::COLLIDER_OBJ::PLAYER_ATTACK, Collider::COLLIDER_OBJ::ENEMY, 0.15f, 0.55f, {{0, 0.6f/0.005f, 0}, 0.5f}},
		{30, PlayerCharacter::COLLIDER_ID::COL_ATTACK_3, Collider::COLLIDER_OBJ::PLAYER_ATTACK, Collider::COLLIDER_OBJ::ENEMY, 0.08f, 0.45f, {{0, 0.6f, 0}, 0.5f}}
		};

		// 待機用ステート
		void WaitState::Enter()
		{
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_SWORD_IDLE, true, 0.1f);
		}
		void WaitState::Execute(float elapsedTime)
		{
		}
		void WaitState::Exit()
		{
		}

		// 待機ステート
		void IdleState::Enter()
		{
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_SWORD_IDLE, true, 0.1f);
		}

		void IdleState::Execute(float elapsedTime)
		{
			owner->RecoverMp(elapsedTime);

			owner->InputMove(elapsedTime);
			owner->Jump();

			PlayerTransition(
				owner,
				flag_Dodge | flag_Jump | flag_Move | flag_Fall | flag_AttackN | flag_AttackS | flag_Skill_1 | flag_Skill_2
			);
		}

		void IdleState::Exit()
		{
		}

		// 移動ステート
		void MoveState::Enter()
		{
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_SWORD_MOVE_CONTINUE, true, 0.2f);
		}

		void MoveState::Execute(float elapsedTime)
		{
			owner->RecoverMp(elapsedTime);

			owner->InputMove(elapsedTime);
			owner->Jump();

			PlayerTransition(
				owner,
				flag_Dodge | flag_Jump | flag_Stop | flag_Fall | flag_AttackN | flag_AttackS | flag_Skill_1 | flag_Skill_2
			);
		}

		void MoveState::Exit()
		{
		}

		// 一般攻撃ステート
		void AttackNormalState::Enter()
		{
			owner->SetAnimationSpeed(1.f);

			SetSubState(NORMAL_ATTACK_STATE::ATTACK_1);
		}
		void AttackNormalState::Execute(float elapsedTime)
		{
			subState->Execute(elapsedTime);

			// 反重力
			owner->StopFall();
			owner->StopMove();

			if (!owner->IsPlayAnimation()) // 攻撃モーション終わり
			{

				owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::IDLE));
			}
		}
		void AttackNormalState::Exit()
		{
			owner->SetAnimationSpeed(1.0f);
			
			subState->Exit();
		}
		//  一般攻撃1
		void AttackNormalState_1::Enter()
		{
			owner->SetAnimationSpeed(1.0f);
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_SWORD_ATTACK_COMBO_FIRST, false, 0.1f);
			
			if (owner->IsPlayer())
			{
				DirectX::XMFLOAT4X4* matrix = &owner->GetModel(0)->FindNode("JOT_C_Sword")->worldTransform;
				Sphere attack1{ { 0, 0.6f / XMFLOAT3Length({matrix->_21, matrix->_22, matrix->_23}), 0} , 0.5f };

				ModelObject::ATTACK_COLLIDER_DATA attackData;
				attackData.power = sphereAttacks[0].power;
				attackData.idx = sphereAttacks[0].idx;
				attackData.objType = sphereAttacks[0].objType;
				attackData.hittableOBJ = sphereAttacks[0].hittableOBJ;
				attackData.hitStartRate = sphereAttacks[0].hitStartRate;
				attackData.hitEndRate = sphereAttacks[0].hitEndRate;
				owner->MakeAttackCollider(attackData, attack1, matrix);
			}
		}
		void AttackNormalState_1::Execute(float elapsedTime)
		{
			if (owner->IsPlayer())
			{
				owner->GetCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_1)->SetCurrentRate(owner->GetModel()->GetAnimationRate());

				float time = owner->GetModel()->GetCurrentAnimationSeconds();
				if (0.184f <= time && time <= 0.368f)
				{
					if (owner->InputAttackNormal())
					{
						owner->GetStateMachine()->ChangeSubState(NORMAL_ATTACK_STATE::ATTACK_2);
					}
					
				}
				else if (0.435f <= time)
				{
					if (owner->InputMove(elapsedTime))
					{
						owner->GetStateMachine()->ChangeState(PlayerCharacter::STATE::MOVE);
					}
					else if (owner->InputDodge())
					{
						owner->GetStateMachine()->ChangeState(PlayerCharacter::STATE::DODGE);
					}
					else if (owner->InputSpecial())
					{
						owner->GetStateMachine()->ChangeState(PlayerCharacter::STATE::ATTACK_SPECIAL);
					}
					else if (owner->InputSkill1())
					{
						owner->GetStateMachine()->ChangeState(SKILL_1_STATE::ATTACK_START);
					}
					else if (owner->InputSkill2())
					{
						owner->GetStateMachine()->ChangeState(PlayerCharacter::STATE::SKILL_2);
					}
				}
			}
			else
			{
				if (!owner->IsPlayAnimation())
					owner->GetStateMachine()->ChangeSubState(NORMAL_ATTACK_STATE::ATTACK_2);
			}
		}
		void AttackNormalState_1::Exit()
		{
			if (owner->IsPlayer())
			{
				owner->DeleteAttackCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_1);
			}
		}
		//  一般攻撃2
		void AttackNormalState_2::Enter()
		{
			owner->SetAnimationSpeed(1.0f);
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_SWORD_ATTACK_COMBO_SECOND, false, 0.2f);

			if (owner->IsPlayer())
			{
				DirectX::XMFLOAT4X4* matrix = &owner->GetModel(0)->FindNode("JOT_C_Sword")->worldTransform;
				Sphere attack2{ { 0, 0.6f / XMFLOAT3Length({matrix->_21, matrix->_22, matrix->_23}), 0} , 0.5f };

				ModelObject::ATTACK_COLLIDER_DATA attackData;
				attackData.power = sphereAttacks[1].power;
				attackData.idx = sphereAttacks[1].idx;
				attackData.objType = sphereAttacks[1].objType;
				attackData.hittableOBJ = sphereAttacks[1].hittableOBJ;
				attackData.hitStartRate = sphereAttacks[1].hitStartRate;
				attackData.hitEndRate = sphereAttacks[1].hitEndRate;
				owner->MakeAttackCollider(attackData, attack2, matrix);
			}
		}
		void AttackNormalState_2::Execute(float elapsedTime)
		{
			float time = owner->GetModel()->GetCurrentAnimationSeconds();
			if (owner->IsPlayer())
			{
				owner->GetCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_2)->SetCurrentRate(owner->GetModel()->GetAnimationRate());

				if (0.185f <= time && time <= 0.418f)
				{
					if (owner->InputAttackNormal())
					{
						owner->GetStateMachine()->ChangeSubState(NORMAL_ATTACK_STATE::ATTACK_3);
					}

				}
				else if (0.835f <= time)
				{
					if (owner->InputMove(elapsedTime))
					{
						owner->GetStateMachine()->ChangeState(PlayerCharacter::STATE::MOVE);
					}
					else if (owner->InputDodge())
					{
						owner->GetStateMachine()->ChangeState(PlayerCharacter::STATE::DODGE);
					}
					else if (owner->InputSpecial())
					{
						owner->GetStateMachine()->ChangeState(PlayerCharacter::STATE::ATTACK_SPECIAL);
					}
					else if (owner->InputSkill1())
					{
						owner->GetStateMachine()->ChangeState(SKILL_1_STATE::ATTACK_START);
					}
					else if (owner->InputSkill2())
					{
						owner->GetStateMachine()->ChangeState(PlayerCharacter::STATE::SKILL_2);
					}
				}
			}
			else
			{
				if (!owner->IsPlayAnimation())
					owner->GetStateMachine()->ChangeSubState(NORMAL_ATTACK_STATE::ATTACK_3);
			}
		}
		void AttackNormalState_2::Exit()
		{
			if (owner->IsPlayer())
			{
				owner->DeleteAttackCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_2);
			}
		}
		//  一般攻撃3
		void AttackNormalState_3::Enter()
		{
			owner->SetAnimationSpeed(0.7f);
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_SWORD_ATTACK_COMBO_THIRD, false, 0.2f);

			if (owner->IsPlayer())
			{
				DirectX::XMFLOAT4X4* matrix = &owner->GetModel(0)->FindNode("JOT_C_Hip")->worldTransform;
				Sphere attack3{ {0, 0, 0} , 1.5f };
				//DirectX::XMFLOAT4X4* matrix = &owner->GetModel(0)->FindNode("JOT_C_Sword")->worldTransform;
				//Sphere attack3{ { 0, 0.6f / XMFLOAT3Length({matrix->_21, matrix->_22, matrix->_23}), 0} , 0.5f };


				ModelObject::ATTACK_COLLIDER_DATA attackData;
				attackData.power = sphereAttacks[2].power;
				attackData.idx = sphereAttacks[2].idx;
				attackData.objType = sphereAttacks[2].objType;
				attackData.hittableOBJ = sphereAttacks[2].hittableOBJ;
				attackData.hitStartRate = sphereAttacks[2].hitStartRate;
				attackData.hitEndRate = sphereAttacks[2].hitEndRate;
				owner->MakeAttackCollider(attackData, attack3, matrix);
			}
		}
		void AttackNormalState_3::Execute(float elapsedTime)
		{
			float time = owner->GetModel()->GetCurrentAnimationSeconds();
			if (owner->IsPlayer())
			{
				owner->GetCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_3)->SetCurrentRate(owner->GetModel()->GetAnimationRate());

				if (0.5f <= time && time <= 0.753f)
				{
					if (owner->InputAttackNormal())
					{
						owner->GetStateMachine()->ChangeSubState(NORMAL_ATTACK_STATE::ATTACK_1);
					}
				}
				else if (0.95f <= time)
				{
					if (owner->InputMove(elapsedTime))
					{
						owner->GetStateMachine()->ChangeState(PlayerCharacter::STATE::MOVE);
					}
					else if (owner->InputDodge())
					{
						owner->GetStateMachine()->ChangeState(PlayerCharacter::STATE::DODGE);
					}
					else if (owner->InputSpecial())
					{
						owner->GetStateMachine()->ChangeState(PlayerCharacter::STATE::ATTACK_SPECIAL);
					}
					else if (owner->InputSkill1())
					{
						owner->GetStateMachine()->ChangeState(SKILL_1_STATE::ATTACK_START);
					}
					else if (owner->InputSkill2())
					{
						owner->GetStateMachine()->ChangeState(PlayerCharacter::STATE::SKILL_2);
					}
				}
			}
			else
			{
				if (!owner->IsPlayAnimation())
				{
					owner->GetStateMachine()->ChangeSubState(NORMAL_ATTACK_STATE::ATTACK_1);
				}
			}
			if (!owner->IsPlayer()) return;
		}
		void AttackNormalState_3::Exit()
		{
			if (owner->IsPlayer())
			{
				owner->DeleteAttackCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_3);
			}
		}

		// スキル_1ステート
		void Skill1State::Enter()
		{
			SetSubState(SKILL_1_STATE::ATTACK_START);
		}
		void Skill1State::Execute(float elapsedTime)
		{
			subState->Execute(elapsedTime);
		}
		void Skill1State::Exit()
		{
			owner->SetAnimationSpeed(1.0f);
		}

		void Skill1StateStart::Enter()
		{
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_SWORD_ATTACK_SPECIAL_FIRST, false, 0.1f);
		}

		void Skill1StateStart::Execute(float elapsedTime)
		{
			if (owner->IsPlayer())
			{
				if (owner->GetModel()->GetAnimationRate() > 0.25f)
				{
					owner->GetStateMachine()->ChangeSubState(SKILL_1_STATE::ATTACK_CONTINUE);
				}
			}
		}

		void Skill1ContinueStart::Enter()
		{
			DirectX::XMFLOAT3 front = owner->GetFront();
			DirectX::XMFLOAT3 impulse;
			DirectX::XMStoreFloat3(&impulse, DirectX::XMVectorScale(DirectX::XMLoadFloat3(&front), impulseSpeed));
			owner->AddImpulse(impulse);
		}
		void Skill1ContinueStart::Execute(float elapsedTime)
		{
			float time = owner->GetModel()->GetCurrentAnimationSeconds();
			if (2.6f <= time)
			{
				if (owner->InputMove(elapsedTime))
				{
					owner->GetStateMachine()->ChangeState(PlayerCharacter::STATE::MOVE);
				}
				else if (owner->InputDodge())
				{
					owner->GetStateMachine()->ChangeState(PlayerCharacter::STATE::DODGE);
				}
				else if (owner->InputSpecial())
				{
					owner->GetStateMachine()->ChangeState(PlayerCharacter::STATE::ATTACK_SPECIAL);
				}
			}

			if (!owner->IsPlayAnimation())
				owner->GetStateMachine()->ChangeState(PlayerCharacter::STATE::IDLE);
		}

		void Skill2State::Enter()
		{
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_SWORD_ATTACK_SPECIAL_SECOND, false, 0.1f);
		}
		void Skill2State::Execute(float elapsedTime)
		{
			float time = owner->GetModel()->GetCurrentAnimationSeconds();
			// 反重力
			owner->StopFall();
			owner->StopMove();

			if (1.215f <= time)
			{
				if (owner->InputMove(elapsedTime))
				{
					owner->GetStateMachine()->ChangeState(PlayerCharacter::STATE::MOVE);
				}
				else if (owner->InputDodge())
				{
					owner->GetStateMachine()->ChangeState(PlayerCharacter::STATE::DODGE);
				}
				else if (owner->InputSpecial())
				{
					owner->GetStateMachine()->ChangeState(PlayerCharacter::STATE::ATTACK_SPECIAL);
				}
			}

			if (!owner->IsPlayAnimation()) // 攻撃モーション終わり
			{
				owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::IDLE));
			}
		}
	}
}
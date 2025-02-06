//! @file PlayerCharacterSwordState.cpp
//! @note

#include "PlayerCharacterSwordState.h"
#include "PlayerCharacterState.h"
#include "GameObject/Props/Zone/DefenceBuffZone.h"
#include "GameObject/Props/Zone/ZoneManager.h"

namespace PlayerCharacterState
{
	namespace Sword
	{
		ATTACK_SPHERE sphereAttacks[4] = {
		{10, PlayerCharacter::COLLIDER_ID::COL_ATTACK_1, Collider::COLLIDER_OBJ::PLAYER_ATTACK, Collider::COLLIDER_OBJ::ENEMY, 0.15f, 0.55f, {{  0, 1.5f, 2.0f} , 0.5f}},
		{20, PlayerCharacter::COLLIDER_ID::COL_ATTACK_2, Collider::COLLIDER_OBJ::PLAYER_ATTACK, Collider::COLLIDER_OBJ::ENEMY, 0.05f, 0.9f, {{  0, 1.5f, 2.0f} , 0.5f}},
		{30, PlayerCharacter::COLLIDER_ID::COL_ATTACK_3, Collider::COLLIDER_OBJ::PLAYER_ATTACK, Collider::COLLIDER_OBJ::ENEMY, 0.08f, 0.45f, {{0, 0, 0} , 1.5f}},
		{30, PlayerCharacter::COLLIDER_ID::COL_SKILL_1, Collider::COLLIDER_OBJ::PLAYER_ATTACK, Collider::COLLIDER_OBJ::ENEMY, 0.08f, 0.45f, {{0, 0, 0} , 1.5f}}
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
			owner->SetAnimationSpeed(1.0f);

			SetSubState(NORMAL_ATTACK_STATE::ATTACK_1);
		}
		void AttackNormalState::Execute(float elapsedTime)
		{
			subState->Execute(elapsedTime);

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

			DirectX::XMFLOAT3 front = owner->GetFront();
			DirectX::XMFLOAT3 impulse;
			DirectX::XMStoreFloat3(&impulse, DirectX::XMVectorScale(DirectX::XMLoadFloat3(&front), impulseSpeed));
			owner->AddImpulse(impulse);

			if (owner->IsPlayer())
			{
				XMFLOAT4X4* matrix = owner->GetTransformAdress();

				ModelObject::ATTACK_COLLIDER_DATA attackData;
				attackData.power = sphereAttacks[0].power;
				attackData.idx = sphereAttacks[0].idx;
				attackData.objType = sphereAttacks[0].objType;
				attackData.hittableOBJ = sphereAttacks[0].hittableOBJ;
				attackData.hitStartRate = sphereAttacks[0].hitStartRate;
				attackData.hitEndRate = sphereAttacks[0].hitEndRate;

				owner->MakePlayerNormalAttackCollider(attackData, sphereAttacks[0].sphere, matrix);
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
					if (owner->InputAttackNormal())
					{
						owner->GetStateMachine()->ChangeSubState(NORMAL_ATTACK_STATE::ATTACK_2);
						return;
					}
					owner->InputMove(elapsedTime);
					PlayerTransition(
						owner,
						flag_Dodge | flag_Move | flag_AttackS | flag_Skill_1 | flag_Skill_2
					);
				}
				if (0.05f <= time && time <= 0.115f)
				{
					owner->SetTrail(true);
				}
				else
				{
					owner->SetTrail(false);
				}
			}
			else
			{
				// 別プレイヤー自動攻撃（予測）
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
			owner->SetTrail(false);
		}
		//  一般攻撃2
		void AttackNormalState_2::Enter()
		{
			owner->SetAnimationSpeed(1.0f);
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_SWORD_ATTACK_COMBO_SECOND, false, 0.2f);

			DirectX::XMFLOAT3 front = owner->GetFront();
			DirectX::XMFLOAT3 impulse;
			DirectX::XMStoreFloat3(&impulse, DirectX::XMVectorScale(DirectX::XMLoadFloat3(&front), impulseSpeed));
			owner->AddImpulse(impulse);

			if (owner->IsPlayer())
			{
				XMFLOAT4X4* matrix = owner->GetTransformAdress();

				ModelObject::ATTACK_COLLIDER_DATA attackData;
				attackData.power = sphereAttacks[1].power;
				attackData.idx = sphereAttacks[1].idx;
				attackData.objType = sphereAttacks[1].objType;
				attackData.hittableOBJ = sphereAttacks[1].hittableOBJ;
				attackData.hitStartRate = sphereAttacks[1].hitStartRate;
				attackData.hitEndRate = sphereAttacks[1].hitEndRate;

				owner->MakePlayerNormalAttackCollider(attackData, sphereAttacks[1].sphere, matrix);
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
					if (owner->InputAttackNormal())
					{
						owner->GetStateMachine()->ChangeSubState(NORMAL_ATTACK_STATE::ATTACK_3);
						return;
					}
					owner->InputMove(elapsedTime);
					PlayerTransition(
						owner,
						flag_Dodge | flag_Move | flag_AttackS | flag_Skill_1 | flag_Skill_2
					);

				}
				if (0.05f <= time && time <= 0.4f)
				{
					owner->SetTrail(true);
				}
				else
				{
					owner->SetTrail(false);
				}
			}
			else
			{
				// 別プレイヤー自動攻撃（予測）
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
			owner->SetTrail(false);
		}
		//  一般攻撃3
		void AttackNormalState_3::Enter()
		{
			owner->SetAnimationSpeed(0.7f);
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_SWORD_ATTACK_COMBO_THIRD, false, 0.2f);

			DirectX::XMFLOAT3 front = owner->GetFront();
			DirectX::XMFLOAT3 impulse;
			DirectX::XMStoreFloat3(&impulse, DirectX::XMVectorScale(DirectX::XMLoadFloat3(&front), impulseSpeed));
			owner->AddImpulse(impulse);

			if (owner->IsPlayer())
			{
				XMFLOAT4X4* matrix = &owner->GetModel(0)->FindNode("JOT_C_Hip")->worldTransform;

				ModelObject::ATTACK_COLLIDER_DATA attackData;
				attackData.power = sphereAttacks[2].power;
				attackData.idx = sphereAttacks[2].idx;
				attackData.objType = sphereAttacks[2].objType;
				attackData.hittableOBJ = sphereAttacks[2].hittableOBJ;
				attackData.hitStartRate = sphereAttacks[2].hitStartRate;
				attackData.hitEndRate = sphereAttacks[2].hitEndRate;

				owner->MakePlayerNormalAttackCollider(attackData, sphereAttacks[2].sphere, matrix);
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
					if (owner->InputAttackNormal())
					{
						owner->GetStateMachine()->ChangeSubState(NORMAL_ATTACK_STATE::ATTACK_1);
						return;
					}
					owner->InputMove(elapsedTime);
					PlayerTransition(
						owner,
						flag_Dodge | flag_Move | flag_AttackS | flag_Skill_1 | flag_Skill_2
					);
				}
				if (0.05f <= time && time <= 0.7f)
				{
					owner->SetTrail(true);
				}
				else
				{
					owner->SetTrail(false);
				}
			}
			else
			{
				// 別プレイヤー自動攻撃（予測）
				if (!owner->IsPlayAnimation())
				{
					owner->GetStateMachine()->ChangeSubState(NORMAL_ATTACK_STATE::ATTACK_1);
				}
			}
		}
		void AttackNormalState_3::Exit()
		{
			if (owner->IsPlayer())
			{
				owner->DeleteAttackCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_3);
			}
			owner->SetTrail(false);
		}

		// スキル_1ステート
		void Skill1State::Enter()
		{
			if (owner->IsPlayer())
			{
				owner->ModifyMp(-owner->GetMpCost(PlayerCharacter::STATE::SKILL_1));
			}
			SetSubState(SKILL_1_STATE::ATTACK_START);
		}
		void Skill1State::Execute(float elapsedTime)
		{
			subState->Execute(elapsedTime);
		}
		void Skill1State::Exit()
		{
			owner->SetAnimationSpeed(1.0f);
			subState->Exit();
		}

		void Skill1StateStart::Enter()
		{
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_SWORD_ATTACK_SPECIAL_FIRST, false, 0.1f);
		}

		void Skill1StateStart::Execute(float elapsedTime)
		{
			if (owner->GetModel()->GetAnimationRate() > 0.25f)
			{
				owner->GetStateMachine()->ChangeSubState(SKILL_1_STATE::ATTACK_CONTINUE);
			}
		}

		void Skill1ContinueStart::Enter()
		{
			DirectX::XMFLOAT3 front = owner->GetFront();
			DirectX::XMFLOAT3 impulse;
			DirectX::XMStoreFloat3(&impulse, DirectX::XMVectorScale(DirectX::XMLoadFloat3(&front), impulseSpeed));
			owner->AddImpulse(impulse);

			if (owner->IsPlayer())
			{
				XMFLOAT4X4* matrix = &owner->GetModel(0)->FindNode("JOT_C_Hip")->worldTransform;

				ModelObject::ATTACK_COLLIDER_DATA attackData;
				attackData.power = sphereAttacks[3].power;
				attackData.idx = sphereAttacks[3].idx;
				attackData.objType = sphereAttacks[3].objType;
				attackData.hittableOBJ = sphereAttacks[3].hittableOBJ;
				attackData.hitStartRate = sphereAttacks[3].hitStartRate;
				attackData.hitEndRate = sphereAttacks[3].hitEndRate;
				owner->MakePlayerSkill1AttackCollider(attackData, sphereAttacks[3].sphere, matrix);
			}
		}
		void Skill1ContinueStart::Execute(float elapsedTime)
		{
			if (owner->IsPlayer())
			{
				owner->GetCollider(PlayerCharacter::COLLIDER_ID::COL_SKILL_1)->SetCurrentRate(owner->GetModel()->GetAnimationRate());
			}

			float time = owner->GetModel()->GetCurrentAnimationSeconds();
			if (2.6f <= time)
			{
				owner->InputMove(elapsedTime);
				PlayerTransition(
					owner,
					flag_Dodge | flag_Move | flag_AttackS
				);
			}
			if (0.65f <= time && time <= 1.5f)
			{
				owner->SetTrail(true);
			}
			else
			{
				owner->SetTrail(false);
			}
			if (!owner->IsPlayAnimation())
				owner->GetStateMachine()->ChangeState(PlayerCharacter::STATE::IDLE);
		}
		void Skill1ContinueStart::Exit()
		{
			if (owner->IsPlayer())
			{
				owner->DeleteAttackCollider(PlayerCharacter::COLLIDER_ID::COL_SKILL_1);
			}
		}

		void Skill2State::Enter()
		{
			if (owner->IsPlayer())
			{
				owner->ModifyMp(-owner->GetMpCost(PlayerCharacter::STATE::SKILL_2));
			}
			DefenceBuffZone* zone = new DefenceBuffZone(owner);
			ZoneManager::Instance().Register(zone);
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_SWORD_ATTACK_SPECIAL_SECOND, false, 0.1f);
		}
		void Skill2State::Execute(float elapsedTime)
		{
			float time = owner->GetModel()->GetCurrentAnimationSeconds();

			if (1.215f <= time)
			{
				owner->InputMove(elapsedTime);
				PlayerTransition(
					owner,
					flag_Dodge | flag_Move | flag_AttackS
				);
			}

			if (!owner->IsPlayAnimation()) // 攻撃モーション終わり
			{
				owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::IDLE));
			}
		}
	}
}
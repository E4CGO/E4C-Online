//! @file PlayerCharacterSwordState.cpp
//! @note

#include "PlayerCharacterSwordState.h"
#include "PlayerCharacterState.h"

namespace PlayerCharacterState
{
	namespace Sword
	{
		ATTACK_SPHERE sphereAttacks[3] = {
		{10, PlayerCharacter::COLLIDER_ID::COL_ATTACK_1, Collider::COLLIDER_OBJ::PLAYER_ATTACK, Collider::COLLIDER_OBJ::ENEMY, 0.15f, 0.55f, {{0, 0.6f, 0}, 0.5f}},
		{20, PlayerCharacter::COLLIDER_ID::COL_ATTACK_2, Collider::COLLIDER_OBJ::PLAYER_ATTACK, Collider::COLLIDER_OBJ::ENEMY, 0.15f, 0.55f, {{0, 0.6f, 0}, 0.5f}},
		{30, PlayerCharacter::COLLIDER_ID::COL_ATTACK_3, Collider::COLLIDER_OBJ::PLAYER_ATTACK, Collider::COLLIDER_OBJ::ENEMY, 0.15f, 0.55f, {{0, 0.6f, 0}, 0.5f}}
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

//#ifdef _DEBUG
//			// 攻撃パラメータ設定用ImGUI描画
//			if (owner->IsPlayer())
//			{
//				DirectX::XMFLOAT4X4* matrix = &owner->GetModel(0)->FindNode("JOT_C_Sword")->worldTransform;
//				XMFLOAT3 scale = { 0, XMFLOAT3Length({ matrix->_21, matrix->_22, matrix->_23 }), 0 };
//
//				ImVec2 pos = ImGui::GetMainViewport()->Pos;
//				ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_FirstUseEver);
//				ImGui::SetNextWindowSize(ImVec2(300, 450), ImGuiCond_FirstUseEver);
//
//				if (ImGui::Begin("AttackCollider", nullptr, ImGuiWindowFlags_None))
//				{
//					for (auto& attack : sphereAttacks)
//					{
//						std::string name = "Attack" + std::to_string(attack.idx);
//						if (ImGui::TreeNode(name.c_str()))
//						{
//							float radius = attack.sphere.radius;
//							XMFLOAT3 offset = attack.sphere.position * scale;
//							float hitStartRate = attack.hitStartRate * 100.0f;
//							float hitEndRate = attack.hitEndRate * 100.0f;
//
//							ImGui::InputFloat("radius", &radius);
//							ImGui::InputFloat3("offset", &offset.x);
//							ImGui::InputFloat("hitStartRate(%)", &hitStartRate);
//							if (hitStartRate < 0.0f)	hitStartRate = 0.0f;
//							if (hitStartRate > hitEndRate)	hitStartRate = hitEndRate;
//							ImGui::InputFloat("hitEndRate(%)", &hitEndRate);
//							if (hitEndRate < hitStartRate)	hitEndRate = hitStartRate;
//							if (hitEndRate > 100.0f)	hitEndRate = 100.0f;
//
//							Sphere sphere(offset / scale, radius);
//							attack.sphere = sphere;
//							attack.hitStartRate = hitStartRate;
//							attack.hitEndRate = hitEndRate;
//
//							ImGui::TreePop();
//						}
//					}
//				}
//				ImGui::End();
//			}
//#endif // _DEBUG

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

				//owner->MakeAttackCollider(10, PlayerCharacter::COLLIDER_ID::COL_ATTACK_1, attack1, Collider::COLLIDER_OBJ::PLAYER_ATTACK, (Collider::COLLIDER_OBJ::ENEMY), matrix);
				ModelObject::ATTACK_COLLIDER_DATA attack1Data;
				attack1Data.power = sphereAttacks[0].power;
				attack1Data.idx = sphereAttacks[0].idx;
				attack1Data.objType = sphereAttacks[0].objType;
				attack1Data.hittableOBJ = sphereAttacks[0].hittableOBJ;
				attack1Data.hitStartRate = sphereAttacks[0].hitStartRate;
				attack1Data.hitEndRate = sphereAttacks[0].hitEndRate;
				owner->MakeAttackCollider(attack1Data, attack1, matrix);
			}
		}
		void AttackNormalState_1::Execute(float elapsedTime)
		{
			if (owner->IsPlayer())
			{
				owner->GetCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_1)->SetCurrentRate(owner->GetModel()->GetAnimationRate());
				//if (!owner->GetCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_1)->IsEnable())
				//{
				//	if (owner->GetModel()->GetAnimationRate() > owner->GetCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_1)->GetHitStartRate())
				//	{
				//		owner->GetCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_1)->SetEnable(true);
				//	}
				//}
				//if (owner->GetModel()->GetAnimationRate() > owner->GetCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_1)->GetHitEndRate())
				//{
				//	owner->GetCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_1)->SetEnable(false);
				//	owner->GetCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_1)->ClearHitOthers();
				//}

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
			owner->DeleteAttackCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_1);
		}
		//  一般攻撃2
		void AttackNormalState_2::Enter()
		{
			owner->SetAnimationSpeed(1.0f);
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_SWORD_ATTACK_COMBO_SECOND, false, 0.2f);
		}
		void AttackNormalState_2::Execute(float elapsedTime)
		{
			float time = owner->GetModel()->GetCurrentAnimationSeconds();
			if (owner->IsPlayer())
			{
				//if (!owner->GetCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_2)->IsEnable())
				//{
				//	if (owner->GetModel()->GetAnimationRate() > owner->GetCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_2)->GetHitStartRate())
				//	{
				//		owner->GetCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_2)->SetEnable(true);
				//	}
				//}
				//if (owner->GetModel()->GetAnimationRate() > owner->GetCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_2)->GetHitEndRate())
				//{
				//	owner->GetCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_2)->SetEnable(false);
				//}

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
		//  一般攻撃3
		void AttackNormalState_3::Enter()
		{
			owner->SetAnimationSpeed(0.7f);
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_SWORD_ATTACK_COMBO_THIRD, false, 0.2f);
		}
		void AttackNormalState_3::Execute(float elapsedTime)
		{
			//if (!owner->GetCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_3)->IsEnable())
			//{
			//	if (owner->GetModel()->GetAnimationRate() > owner->GetCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_3)->GetHitStartRate())
			//	{
			//		owner->GetCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_3)->SetEnable(true);
			//	}
			//}
			//if (owner->GetModel()->GetAnimationRate() > owner->GetCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_3)->GetHitEndRate())
			//{
			//	owner->GetCollider(PlayerCharacter::COLLIDER_ID::COL_ATTACK_3)->SetEnable(false);
			//}

			float time = owner->GetModel()->GetCurrentAnimationSeconds();
			if (owner->IsPlayer())
			{
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
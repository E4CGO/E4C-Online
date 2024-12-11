//! @file PlayerCharacterSwordState.cpp
//! @note

#include "PlayerCharacterRodState.h"
#include "PlayerCharacterState.h"

namespace PlayerCharacterState
{
	namespace Rod
	{
		// 待機用ステート
		void WaitState::Enter()
		{
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_ROD_IDLE, true, 0.1f);
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
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_ROD_IDLE, true, 0.1f);
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
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_ROD_MOVE_CONTINUE, true, 0.2f);
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
		}
		//  一般攻撃1
		void AttackNormalState_1::Enter()
		{
			owner->SetAnimationSpeed(1.0f);
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_ROD_ATTACK_COMBO_FIRST, false, 0.1f);
		}
		void AttackNormalState_1::Execute(float elapsedTime)
		{
			if (owner->IsPlayer())
			{
				float time = owner->GetModel()->GetCurrentAnimationSeconds();
				if (0.95f <= time)
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
				}
			}
			else
			{
				if (!owner->IsPlayAnimation())
					owner->GetStateMachine()->ChangeSubState(NORMAL_ATTACK_STATE::ATTACK_2);
			}
		}
		//  一般攻撃2
		void AttackNormalState_2::Enter()
		{
			owner->SetAnimationSpeed(1.0f);
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_ROD_ATTACK_COMBO_SECOND, false, 0.2f);
		}
		void AttackNormalState_2::Execute(float elapsedTime)
		{
			float time = owner->GetModel()->GetCurrentAnimationSeconds();
			if (owner->IsPlayer())
			{
				if (0.95f <= time)
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
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_ROD_ATTACK_COMBO_THIRD, false, 0.2f);
		}
		void AttackNormalState_3::Execute(float elapsedTime)
		{
			float time = owner->GetModel()->GetCurrentAnimationSeconds();
			if (owner->IsPlayer())
			{
				if (0.95f <= time)
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
				}
			}
			else
			{
				if (!owner->IsPlayAnimation())
					owner->GetStateMachine()->ChangeSubState(NORMAL_ATTACK_STATE::ATTACK_1);
			}
			if (!owner->IsPlayer()) return;
		}

		// 特殊攻撃ステート
		void AttackSpecialState::Enter()
		{
			owner->SetAnimationSpeed(1.0f);
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_ROD_CHARGE_START, false, 0.05f);
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_ROD_CHARGE_CONTINUE, true);
		}
		void AttackSpecialState::Execute(float elapsedTime)
		{
			owner->StopMove();

			if (!owner->InputSpecial())
			{
				owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::IDLE));
			}
		}
		void AttackSpecialState::Exit()
		{
			owner->SetAnimationSpeed(1.0f);
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_ROD_IDLE, false, 0.05f);
		}
	}
}
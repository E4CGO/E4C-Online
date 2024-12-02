//! @file PlayerCharacterSwordState.cpp
//! @note

#include "PlayerCharacterSwordState.h"

namespace PlayerCharacterState
{
	namespace Sword
	{
		// 一般攻撃ステート
		void AttackNormalState::Enter()
		{
			owner->SetAnimationSpeed(1.5f);

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
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_ATTACK_SWORD_COMBO_FIRST, false, 0.05f);
		}
		void AttackNormalState_1::Execute(float elapsedTime)
		{
			if (owner->IsPlayer())
			{
				float time = owner->GetModel()->GetAnimationRate();
				if (owner->GetModel()->GetAnimationRate() > 0.75f)
				{
					if (owner->InputAttackNormal()) // アニメーション75%完成
					{
						owner->GetStateMachine()->ChangeSubState(NORMAL_ATTACK_STATE::ATTACK_2);
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
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_ATTACK_SWORD_COMBO_SECOND, false, 0.05f);
		}
		void AttackNormalState_2::Execute(float elapsedTime)
		{
			if (owner->IsPlayer())
			{
				if (owner->GetModel()->GetAnimationRate() > 0.75f)
				{
					if (owner->InputAttackNormal()) // アニメーション75%完成
					{
						owner->GetStateMachine()->ChangeSubState(NORMAL_ATTACK_STATE::ATTACK_3);
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
			owner->SetAnimationSpeed(1.2f);
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_ATTACK_SWORD_COMBO_THIRD, false, 0.05f);
		}
		void AttackNormalState_3::Execute(float elapsedTime)
		{
			if (!owner->IsPlayer()) return;
		}

		// 特殊攻撃ステート
		void AttackSpecialState::Enter()
		{
			owner->SetAnimationSpeed(1.2f);
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_GUARD_SHIELD_START, false, 0.05f);
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_GUARD_SHIELD_CONTINUE, true);
		}
		void AttackSpecialState::Execute(float elapsedTime)
		{
			// 反重力
			owner->StopFall();
			owner->StopMove();

			if (!owner->InputAttackSpecial())
			{
				owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::IDLE));
			}
		}
		void AttackSpecialState::Exit()
		{
			owner->SetAnimationSpeed(1.0f);
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_GUARD_SHIELD_FINISH, false, 0.05f);
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
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_ATTACK_SWORD_SPECIAL_FIRST, false, 0.05f);
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
			if (!owner->IsPlayAnimation())
				owner->GetStateMachine()->ChangeState(PlayerCharacter::STATE::IDLE);
		}

		void Skill2State::Enter()
		{
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_ATTACK_SWORD_SPECIAL_SECOND, false, 0.05f);
		}
		void Skill2State::Execute(float elapsedTime)
		{
			// 反重力
			owner->StopFall();
			owner->StopMove();

			if (!owner->IsPlayAnimation()) // 攻撃モーション終わり
			{
				owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::IDLE));
			}
		}
	}
}
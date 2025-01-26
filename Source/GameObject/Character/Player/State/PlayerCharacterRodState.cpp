//! @file PlayerCharacterSwordState.cpp
//! @note

#include "PlayerCharacterRodState.h"
#include "PlayerCharacterState.h"

namespace PlayerCharacterState
{
	namespace Rod
	{
		bool m_isShot = false;
		Projectile* m_pfireball = nullptr;

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
			m_isShot = false;
			owner->SetAnimationSpeed(1.f);

			SetSubState(NORMAL_ATTACK_STATE::ATTACK_1);
		}
		void AttackNormalState::Execute(float elapsedTime)
		{
			subState->Execute(elapsedTime);

			// 反重力
			owner->StopMove();

			if (!owner->IsPlayAnimation()) // 攻撃モーション終わり
			{
				owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::IDLE));
				if (m_isShot) PROJECTILES.Remove(m_pfireball);
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

				if (time <= 0.3f)
				{
					if (!m_isShot)
					{
						m_pfireball = PROJECTILES.Register(new FireballObject(owner));
						m_pfireball->SetPosition({ owner->GetShotPosition().x, owner->GetShotPosition().y, owner->GetShotPosition().z });
						m_pfireball->PointTo(owner->GetShotPosition() + owner->GetFront());
						m_pfireball->SetDirection(owner->GetFront());
						m_pfireball->SetOwner(owner);
						m_pfireball->SetMove(false);
						m_pfireball->SetScale({ 0.25f, 0.25f, 0.25f });

						m_isShot = true;
					}
				}
				else if (0.45f <= time)
				{
					if (owner->InputAttackNormal())
					{
						owner->GetStateMachine()->ChangeSubState(NORMAL_ATTACK_STATE::ATTACK_2);
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

					PROJECTILES.Remove(m_pfireball);
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
				if (time <= 0.3f)
				{
					m_pfireball->SetScale({ 0.5f, 0.5f, 0.5f });
				}
				else if (0.45f <= time)
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

					PROJECTILES.Remove(m_pfireball);
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
			owner->SetAnimationSpeed(1.0f);
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_ROD_ATTACK_COMBO_THIRD, false, 0.2f);
		}
		void AttackNormalState_3::Execute(float elapsedTime)
		{
			float time = owner->GetModel()->GetCurrentAnimationSeconds();
			if (owner->IsPlayer())
			{
				if (time <= 0.3f)
				{
					m_pfireball->SetScale({ 1.0f, 1.0f, 1.0f });
					m_pfireball->SetMove(true);

					m_isShot = false;

					owner->ModifyMp(-20.0f);

					owner->GetStateMachine()->ChangeState(PlayerCharacter::STATE::IDLE);
				}
				else if (0.45f <= time)
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

			m_pparticle = PROJECTILES.Register(new ParticleObject(owner));
			m_pparticle->SetOwner(owner);
		}
		void AttackSpecialState::Execute(float elapsedTime)
		{
			owner->StopMove();

			if (!owner->InputSpecial())
			{
				owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::IDLE));
			}

			owner->ModifyMp(-10.0f * elapsedTime);
		}
		void AttackSpecialState::Exit()
		{
			owner->SetAnimationSpeed(1.0f);
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_ROD_IDLE, false, 0.05f);
			PROJECTILES.Remove(m_pparticle);
		}

		// スキル_1ステート
		void Skill1State::Enter()
		{
			owner->ModifyMp(-50.0f);

			owner->SetAnimation(PlayerCharacter::Animation::ANIM_ROD_ATTACK_SPECIAL_FIRST, false, 0.1f);

			m_pbeam = PROJECTILES.Register(new BeamObject(owner));
			m_pbeam->SetPosition({ owner->GetShotPosition().x, owner->GetShotPosition().y * 2, owner->GetShotPosition().z });
			m_pbeam->PointTo(owner->GetShotPosition() + owner->GetFront());
			m_pbeam->SetDirection(owner->GetFront());
			m_pbeam->SetOwner(owner);
		}
		void Skill1State::Execute(float elapsedTime)
		{
			float time = owner->GetModel()->GetCurrentAnimationSeconds();
			// 反重力
			owner->StopMove();

			if (!owner->IsPlayAnimation()) // 攻撃モーション終わり
			{
				owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::IDLE));
			}
		}
		void Skill1State::Exit()
		{
		}

		void Skill2State::Enter()
		{
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_ROD_ATTACK_SPECIAL_SECOND, false, 0.1f);
		}
		void Skill2State::Execute(float elapsedTime)
		{
			float time = owner->GetModel()->GetCurrentAnimationSeconds();
			// 反重力
			owner->StopMove();

			if (!owner->IsPlayAnimation()) // 攻撃モーション終わり
			{
				owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::IDLE));
			}
		}
	}
}
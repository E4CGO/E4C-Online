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

			SetSubState(FIREBALL_STATE::CHARGE_READY);
		}
		void AttackNormalState::Execute(float elapsedTime)
		{
			subState->Execute(elapsedTime);

			// 反重力
			owner->StopMove();

			if (this->GetSubStateIndex() == FIREBALL_STATE::ATTACK_END && !owner->IsPlayAnimation())
			{
				owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::IDLE));
			}
		}
		void AttackNormalState::Exit()
		{
			owner->SetAnimationSpeed(1.0f);
		}
		// Fireball攻撃準備
		void AttackNormalState_Ready::Enter()
		{
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_ROD_CHARGE_START, false, 0.1f);
		}
		void AttackNormalState_Ready::Execute(float elapsedTime)
		{
			if (!owner->IsPlayAnimation())
				owner->GetStateMachine()->ChangeSubState(FIREBALL_STATE::CHARGE);
			
			if (owner->IsPlayer())
			{
				DirectX::XMFLOAT3 target = owner->GetTarget();
				owner->FaceToCamera();

				if(owner->ReleaseAttackNormal())
					owner->GetStateMachine()->ChangeSubState(FIREBALL_STATE::ATTACK_END);
			}
		}
		// Fireball攻撃チャージ
		void AttackNormalState_Charge::Enter()
		{
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_ROD_CHARGE_CONTINUE, true);
			Projectile* arrow = PROJECTILES.Register(new FireballObject(owner));
			m_chargeTme = 0.0f;
		}
		void AttackNormalState_Charge::Execute(float elapsedTime)
		{
			if (owner->IsPlayer())
			{
				DirectX::XMFLOAT3 target = owner->GetTarget();
				owner->FaceToCamera();

				if (owner->ReleaseAttackNormal())
				{
					if (m_chargeTme > MAX_ChargeTime)
					{
						owner->GetStateMachine()->ChangeSubState(FIREBALL_STATE::ATTACK_3);
					}
					else if (m_chargeTme < MAX_ChargeTime * 0.5f)
					{
						owner->GetStateMachine()->ChangeSubState(FIREBALL_STATE::ATTACK_1);
					}
					else
					{
						owner->GetStateMachine()->ChangeSubState(FIREBALL_STATE::ATTACK_2);
					}
				}
			}
			m_chargeTme += elapsedTime;
		}
		// Fireball小攻撃
		void AttackNormalState_1::Enter()
		{
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_ROD_ATTACK_COMBO_FIRST, false, 0.1f);
		}
		void AttackNormalState_1::Execute(float elapsedTime)
		{
			if (!owner->IsPlayAnimation())
				owner->GetStateMachine()->ChangeSubState(FIREBALL_STATE::ATTACK_END);
		}
		// Fireball中攻撃
		void AttackNormalState_2::Enter()
		{
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_ROD_ATTACK_COMBO_SECOND, false, 0.1f);
		}
		void AttackNormalState_2::Execute(float elapsedTime)
		{
			if (!owner->IsPlayAnimation())
				owner->GetStateMachine()->ChangeSubState(FIREBALL_STATE::ATTACK_END);
		}
		// Fireball大攻撃
		void AttackNormalState_3::Enter()
		{
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_ROD_ATTACK_COMBO_THIRD, false, 0.1f);
		}
		void AttackNormalState_3::Execute(float elapsedTime)
		{
			if (!owner->IsPlayAnimation())
				owner->GetStateMachine()->ChangeSubState(FIREBALL_STATE::ATTACK_END);
		}
		// Fireball攻撃終了
		void AttackNormalState_End::Enter()
		{
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_ROD_CHARGE_END, false, 0.1f);
			owner->ModifyMp(-20.0f);
		}
		void AttackNormalState_End::Execute(float elapsedTime)
		{
			if (owner->IsPlayer())
			{
				if (owner->InputAttackNormal())
					owner->GetStateMachine()->ChangeSubState(FIREBALL_STATE::CHARGE);

				owner->InputMove(elapsedTime);
				owner->Jump();
			}

			PlayerTransition(
				owner,
				flag_Jump | flag_Move );
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
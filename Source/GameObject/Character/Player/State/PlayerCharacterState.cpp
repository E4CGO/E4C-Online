#include "PlayerCharacterState.h"

#include "TAKOEngine/Effects/EffectManager.h"

#include "GameObject/Character/Character.h"
#include "GameObject/Character/Player/Barbarian.h"
#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"

namespace PlayerCharacterState
{
	void PlayerTransition(PlayerCharacter* owner, uint32_t flags)
	{
		if (!owner->IsPlayer()) return;
		if (flags & flag_Dodge && owner->InputDodge())
		{
			owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::DODGE));
			return;
		}
		if (flags & flag_Jump && owner->InputJump())
		{
			owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::JUMP));
			return;
		}
		if (flags & flag_Move && owner->IsMove())
		{
			owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::MOVE));
			return;
		}
		if (flags & flag_Stop && !owner->IsMove())
		{
			owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::IDLE));
			return;
		}
		if (flags & flag_Fall && owner->IsFall())
		{
			owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::FALL));
			return;
		}
		if (flags & flag_Ground && owner->IsGround())
		{
			owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::LAND));
			return;
		}
		if (flags & flag_AttackN && owner->InputAttackNormal())
		{
			owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::ATTACK_NORMAL));
			return;
		}
		if (flags & flag_AttackS && owner->InputAttackSpecial())
		{
			owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::ATTACK_SPECIAL));
			return;
		}
		if (flags & flag_Skill_1 && owner->InputSkill1())
		{
			owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::SKILL_1));
			return;
		}
	}
	// 待機ステート
	void IdleState::Enter()
	{
		owner->SetAnimation(PlayerCharacter::Animation::Idle, true, 0.1f);
	}
	void IdleState::Execute(float elapsedTime)
	{
		owner->RecoverMp(elapsedTime);

		owner->InputMove(elapsedTime);
		owner->Jump();

		PlayerTransition(
			owner,
			flag_Dodge | flag_Jump | flag_Move | flag_Fall | flag_AttackN | flag_AttackS | flag_Skill_1
		);
	}
	void IdleState::Exit()
	{
	}

	// 移動ステート
	void MoveState::Enter()
	{
		owner->SetAnimation(PlayerCharacter::Animation::Running_A, true, 0.1f);
	}
	void MoveState::Execute(float elapsedTime)
	{
		owner->RecoverMp(elapsedTime);

		owner->InputMove(elapsedTime);
		owner->Jump();

		PlayerTransition(
			owner,
			flag_Dodge | flag_Jump | flag_Stop | flag_Fall | flag_AttackN | flag_AttackS | flag_Skill_1
		);
	}
	void MoveState::Exit()
	{
	}

	// ジャンプステート
	void JumpState::Enter()
	{
		owner->SetAnimation(PlayerCharacter::Animation::Jump_Start, false, 0.1f);
	}
	void JumpState::Execute(float elapsedTime)
	{
		owner->InputMove(elapsedTime);

		PlayerTransition(
			owner,
			flag_Fall | flag_Dodge | flag_AttackN | flag_AttackS | flag_Skill_1
		);
	}
	void JumpState::Exit()
	{
	}

	// 落下ステート
	void FallState::Enter()
	{
		owner->SetAnimation(PlayerCharacter::Animation::Jump_Idle, true, 0.1f);
	}
	void FallState::Execute(float elapsedTime)
	{
		owner->InputMove(elapsedTime);

		PlayerTransition(
			owner,
			flag_Fall | flag_Dodge | flag_Ground | flag_AttackN | flag_AttackS | flag_Skill_1
		);
	}
	void FallState::Exit()
	{
	}

	// 着地ステート
	void LandState::Enter()
	{
		owner->SetAnimation(PlayerCharacter::Animation::Jump_Land, false, 0.1f);
	}
	void LandState::Execute(float elapsedTime)
	{
		owner->InputMove(elapsedTime);
		owner->Jump();

		PlayerTransition(
			owner,
			flag_Dodge | flag_Jump | flag_Move | flag_Fall | flag_AttackN | flag_AttackS | flag_Skill_1
		);

		if (!owner->IsPlayAnimation()) owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::IDLE)); return;
	}
	void LandState::Exit()
	{
	}

	// 回避ステート
	void DodgeState::Enter()
	{
		owner->SetAnimation(PlayerCharacter::Animation::Dodge_Forward, false, 0.0f);
		owner->SetHurtCoolTime(0.2f);

		// MP消費
		owner->ModifyMp(-owner->GetMpCost(static_cast<int>(PlayerCharacter::STATE::DODGE)));
	}
	void DodgeState::Execute(float elapsedTime)
	{
		if (!owner->IsPlayAnimation())
		{
			owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::IDLE));
			owner->StopMove();
		}
	}
	void DodgeState::Exit()
	{
		owner->GetCollider()->SetEnable(true);
	}

	// 怪我
	void HurtState::Enter()
	{
		owner->SetAnimation(PlayerCharacter::Animation::Hit_A, false, 0.1f);
	}
	void HurtState::Execute(float elapsedTime)
	{
		if (!owner->IsPlayAnimation())
		{
			owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::IDLE));
		}
	}
	void HurtState::Exit()
	{
	}
	// 死亡
	void DeathState::Enter()
	{
		owner->SetAnimation(PlayerCharacter::Animation::Death_A, false, 0.1f);
		owner->GetCollider()->SetEnable(false);
	}
	void DeathState::Execute(float elapsedTime)
	{
	}
	void DeathState::Exit()
	{
	}
	// 待機用ステート
	void WaitState::Enter()
	{
		owner->SetAnimation(PlayerCharacter::Animation::Idle, true, 0.1f);
	}
	void WaitState::Execute(float elapsedTime)
	{
	}
	void WaitState::Exit()
	{
	}
	// 待機用 (準備完了)ステート
	void ReadyState::Enter()
	{
		owner->SetAnimation(PlayerCharacter::Animation::Cheer, true, 0.1f);
	}
	void ReadyState::Execute(float elapsedTime)
	{
	}
	void ReadyState::Exit()
	{
	}



	//斧
	namespace Axe
	{
		// 待機ステート
		void IdleState::Enter()
		{
			owner->SetAnimation(Player::Animation::TwoHanded_Melee_Idle, true, 0.1f);
		}
		void WaitState::Enter()
		{
			owner->SetAnimation(Player::Animation::TwoHanded_Melee_Idle, true, 0.1f);
		}

		// 一般攻撃ステート
		void AttackNormalState::Enter()
		{
			owner->SetAnimationSpeed(2.0f);
			SetSubState(Barbarian::ATTACK_STATE::ATTACK_1);
		}
		void AttackNormalState::Execute(float elapsedTime)
		{
			subState->Execute(elapsedTime);

			// 反重力
			owner->StopFall();
			owner->StopMove();

			if (!owner->IsPlayAnimation()) // 攻撃モーション終わり
			{
				owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));
			}
		}
		void AttackNormalState::Exit()
		{
			owner->SetAnimationSpeed(1.0f);
		}
		//  一般攻撃1
		void AttackNormalState_1::Enter()
		{
			owner->SetAnimation(Player::Animation::TwoHanded_Melee_Attack_Slice, false, 0.05f);
		}
		void AttackNormalState_1::Execute(float elapsedTime)
		{
			if (!owner->IsPlayer()) return;

			if (owner->GetModel()->GetAnimationRate() > 0.2f)
			{
				//owner->GetAttackCollider(Barbarian::AttackCollider::Axe)->SetEnable(true);
			}

			if (owner->GetModel()->GetAnimationRate() > 0.75f) // アニメーション75%完成
			{
				//owner->GetAttackCollider(Barbarian::AttackCollider::Axe)->SetEnable(false);
				if (owner->InputAttackNormal()) {
					owner->GetStateMachine()->ChangeSubState(Barbarian::ATTACK_STATE::ATTACK_2);
				}
			}
		}
		//  一般攻撃2
		void AttackNormalState_2::Enter()
		{
			owner->SetAnimation(Player::Animation::TwoHanded_Melee_Attack_Chop, false, 0.05f);
		}
		void AttackNormalState_2::Execute(float elapsedTime)
		{
			if (!owner->IsPlayer()) return;
			if (owner->GetModel()->GetAnimationRate() > 0.2f)
			{
				//owner->GetAttackCollider(Barbarian::AttackCollider::Axe)->SetEnable(true);
			}

			if (owner->GetModel()->GetAnimationRate() > 0.75f) // アニメーション75%完成
			{
				//owner->GetAttackCollider(Barbarian::AttackCollider::Axe)->SetEnable(false);
				if (owner->InputAttackNormal()) {
					owner->GetStateMachine()->ChangeSubState(Barbarian::ATTACK_STATE::ATTACK_3);
				}
			}
		}
		//  一般攻撃3
		void AttackNormalState_3::Enter()
		{
			owner->SetAnimation(Player::Animation::TwoHanded_Melee_Attack_Spin, false, 0.05f);
		}
		void AttackNormalState_3::Execute(float elapsedTime)
		{
			if (!owner->IsPlayer()) return;
			if (owner->GetModel()->GetAnimationRate() > 0.2f)
			{
				//owner->GetAttackCollider(Barbarian::AttackCollider::Axe)->SetEnable(true);
			}

			if (owner->GetModel()->GetAnimationRate() > 0.75f) // アニメーション75%完成
			{
				//owner->GetAttackCollider(Barbarian::AttackCollider::Axe)->SetEnable(false);
			}
		}

		// 特殊攻撃ステート
		void AttackSpecialState::Enter()
		{
			owner->SetAnimationSpeed(1.2f);
			SetSubState(Barbarian::THROW_STATE::THROW_AIM);
		}
		void AttackSpecialState::Execute(float elapsedTime)
		{
			subState->Execute(elapsedTime);

			if (!owner->IsPlayAnimation()) // アニメーション終了
			{
				owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));
			}
		}
		void AttackSpecialState::Exit()
		{
			owner->SetAnimationSpeed(1.0f);
			//owner->GetModel()->FindNode("2H_Axe")->visible = true;
			if (owner->IsPlayer()) TPSCamera.AimMode(false);	// AIM
		}
		void AttackSpecialStateAim::Enter()
		{
			owner->SetAnimation(Player::Animation::Throw, false, 0.05f);
			if (owner->IsPlayer()) TPSCamera.AimMode(true);	// AIM
		}
		void AttackSpecialStateAim::Execute(float elapsedTime)
		{
			if (owner->GetModel()->GetAnimationRate() > 0.375f)
			{
				owner->GetModel()->SetAnimationRate(0.375f);
			}

			// 自機オンリー
			if (!owner->IsPlayer()) return;

			owner->FaceToCamera();

			if (owner->ReleaseAttackSpecial()) // 特殊攻撃ボタンリリース
			{
				owner->GetStateMachine()->ChangeSubState(Barbarian::THROW_ATTACK);
			}
		}
		void AttackSpecialStateAim::Exit()
		{
		}

		void AttackSpecialStateAttack::Enter()
		{
			isShot = false;
		}
		void AttackSpecialStateAttack::Execute(float elapsedTime)
		{
			if (owner->GetModel()->GetAnimationRate() > 0.4f && !isShot)
			{
				// 発射
				//Projectile* axe = PROJECTILES.Register(new TwoHandedAxe(owner));
				//axe->SetPosition(owner->GetShotPosition());
				//axe->PointTo(owner->GetTarget());

				//DirectX::XMFLOAT3 direction = XMFLOAT3Normalize(owner->GetTarget() - owner->GetShotPosition());
				//DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);
				//Direction = DirectX::XMVector3Rotate(Direction, DirectX::XMQuaternionRotationAxis(DirectX::XMVector3Cross(Direction, DirectX::XMVectorSet(0, 1, 0, 0)), DirectX::XMConvertToRadians(15.0f)));
				//DirectX::XMStoreFloat3(&direction, Direction);
				//axe->SetDirection(direction);
				//axe->SetOwner(owner);

				isShot = true;
				//owner->GetModel()->FindNode("2H_Axe")->visible = false;
			}
		}

		// スキル_1ステート
		void Skill1State::Enter()
		{
			owner->SetAnimationSpeed(1.5f);
			SetSubState(Barbarian::SKILL_1_STATE::SKILL_1_START);
			cacheTurnSpeed = owner->GetTurnSpeed();
			owner->SetTurnSpeed(0);

			owner->SkillCost(static_cast<int>(Player::State::Skill_1));
		}
		void Skill1State::Execute(float elapsedTime)
		{
			subState->Execute(elapsedTime);

			if (owner->GetMp() <= 0.0f || owner->ReleaseSkill1())
			{
				owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));
			}
		}
		void Skill1State::Exit()
		{
			owner->SetAnimationSpeed(1.0f);
			owner->SetTurnSpeed(cacheTurnSpeed);
		}
		void Skill1StateStart::Enter()
		{
			owner->SetAnimation(Player::Animation::TwoHanded_Melee_Attack_Spin, false);
		}
		void Skill1StateStart::Execute(float elapsedTime)
		{
			if (owner->GetModel()->GetAnimationRate() > 0.15f)
			{
				owner->GetStateMachine()->ChangeSubState(Barbarian::SKILL_1_STATE::SKILL_1_LOOP);
			}
		}
		void Skill1StateLoop::Enter()
		{
			//owner->GetAttackCollider(Barbarian::AttackCollider::Axe)->SetEnable(true);
			owner->SetAnimation(Player::Animation::TwoHanded_Melee_Attack_Spinning, true);
		}
		void Skill1StateLoop::Execute(float elapsedTime)
		{
			owner->InputMove(elapsedTime);
			owner->ModifyMp(-elapsedTime * 15.0f);
		}
		void Skill1StateLoop::Exit()
		{
			//owner->GetAttackCollider(Barbarian::AttackCollider::Axe)->SetEnable(false);
		}
	}
}


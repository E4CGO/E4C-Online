#include "PlayerCharacterState.h"

#include "TAKOEngine/Effects/EffectManager.h"

#include "GameObject/Character/Character.h"
#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"

const uint32_t flag_Dodge = 1 << 0;
const uint32_t flag_Jump = 1 << 1;
const uint32_t flag_Move = 1 << 2;
const uint32_t flag_Stop = 1 << 3;
const uint32_t flag_Fall = 1 << 4;
const uint32_t flag_Ground = 1 << 5;
const uint32_t flag_AttackN = 1 << 6;
const uint32_t flag_AttackS = 1 << 7;
const uint32_t flag_Skill_1 = 1 << 8;
const uint32_t flag_Skill_2 = 1 << 9;
const uint32_t flag_Skill_3 = 1 << 10;
const uint32_t flag_Skill_4 = 1 << 11;

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
		owner->SetAnimation(PlayerCharacter::Animation::ANIM_IDLE, true, 0.1f);
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
		owner->SetAnimation(PlayerCharacter::Animation::ANIM_MOVE_CONTINUE, true, 0.1f);
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
		owner->SetAnimation(PlayerCharacter::Animation::ANIM_MOVE_CONTINUE, false, 0.1f);
	}
	void JumpState::Execute(float elapsedTime)
	{
		owner->InputMove(elapsedTime);

		PlayerTransition(
			owner,
			flag_Fall | flag_Dodge | flag_Ground | flag_AttackN | flag_AttackS | flag_Skill_1
		);
	}
	void JumpState::Exit()
	{
	}

	// 落下ステート
	void FallState::Enter()
	{
		owner->SetAnimation(PlayerCharacter::Animation::ANIM_MOVE_CONTINUE, true, 0.1f);
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
		owner->SetAnimation(PlayerCharacter::Animation::ANIM_MOVE_START, false, 0.1f);
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
		owner->SetAnimation(PlayerCharacter::Animation::ANIM_MOVE_CONTINUE, false, 0.0f);
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
		owner->SetAnimation(PlayerCharacter::Animation::ANIM_HURT, false, 0.1f);
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
		owner->SetAnimation(PlayerCharacter::Animation::ANIM_DEATH, false, 0.1f);
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
		owner->SetAnimation(PlayerCharacter::Animation::ANIM_IDLE, true, 0.1f);
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
		owner->SetAnimation(PlayerCharacter::Animation::ANIM_IDLE, true, 0.1f);
	}
	void ReadyState::Execute(float elapsedTime)
	{
	}
	void ReadyState::Exit()
	{
	}
}
#include "PlayerCharacterState.h"

#include "TAKOEngine/Effects/EffectManager.h"

#include "GameObject/Character/Character.h"
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

		if (flags & flag_AttackS && owner->InputSpecial())
		{
			owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::ATTACK_SPECIAL));
			return;
		}

		if (flags & flag_Skill_1 && owner->InputSkill1())
		{
			owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::SKILL_1));
			return;
		}

		if (flags & flag_Skill_2 && owner->InputSkill2())
		{
			owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::SKILL_2));
			return;
		}
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

	// ジャンプステート
	void JumpState::Enter()
	{
		owner->SetAnimation(PlayerCharacter::Animation::ANIM_SWORD_MOVE_CONTINUE, false, 0.1f);
	}
	void JumpState::Execute(float elapsedTime)
	{
		owner->InputMove(elapsedTime);

		PlayerTransition(
			owner,
			flag_Fall | flag_Dodge | flag_Ground | flag_AttackN | flag_AttackS | flag_Skill_1 | flag_Skill_2
		);
	}
	void JumpState::Exit()
	{
	}

	// 落下ステート
	void FallState::Enter()
	{
		owner->SetAnimation(PlayerCharacter::Animation::ANIM_SWORD_MOVE_CONTINUE, true, 0.1f);
	}
	void FallState::Execute(float elapsedTime)
	{
		owner->InputMove(elapsedTime);

		PlayerTransition(
			owner,
			flag_Fall | flag_Dodge | flag_Ground | flag_AttackN | flag_AttackS | flag_Skill_1 | flag_Skill_2
		);
	}
	void FallState::Exit()
	{
	}

	// 着地ステート
	void LandState::Enter()
	{
		owner->SetAnimation(PlayerCharacter::Animation::ANIM_SWORD_MOVE_START, false, 0.1f);
	}
	void LandState::Execute(float elapsedTime)
	{
		owner->InputMove(elapsedTime);
		owner->Jump();

		PlayerTransition(
			owner,
			flag_Dodge | flag_Jump | flag_Move | flag_Fall | flag_AttackN | flag_AttackS | flag_Skill_1 | flag_Skill_2
		);

		if (!owner->IsPlayAnimation()) owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::IDLE)); return;
	}
	void LandState::Exit()
	{
	}

	// 回避ステート
	void DodgeState::Enter()
	{
		owner->SetAnimation(PlayerCharacter::Animation::ANIM_SWORD_MOVE_CONTINUE, false, 0.0f);
		owner->SetHurtCoolTime(0.2f);

		// 判定消去
		if (owner->IsPlayer())
		{
			owner->GetColliders()[PlayerCharacter::COLLIDER_ID::COL_BODY]->SetEnable(false);
		}

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
		if (owner->IsPlayer())
		{
			owner->GetColliders()[PlayerCharacter::COLLIDER_ID::COL_BODY]->SetEnable(true);
		}
	}

	// 怪我
	void HurtState::Enter()
	{
		owner->SetAnimation(PlayerCharacter::Animation::ANIM_SWORD_HURT, false, 0.1f);
		owner->SetTrail(false);
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
		owner->GetMoveCollider()->SetEnable(false);
		for (std::pair<int, Collider*> collider : owner->GetColliders()) collider.second->SetEnable(false);
		owner->SetAnimation(PlayerCharacter::Animation::ANIM_SWORD_DEATH, false, 0.1f);
	}
	void DeathState::Execute(float elapsedTime)
	{
		if (!owner->IsPlayAnimation())
		{
			// Respawn
			owner->SetCurrentHp(100.0f);
			owner->SetPosition({ 0.0f, 5.0f, 0.0f });
			owner->GetMoveCollider()->SetEnable(true);
			for (std::pair<int, Collider*> collider : owner->GetColliders()) collider.second->SetEnable(true);
			owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::IDLE));
		}
	}
	void DeathState::Exit()
	{
	}
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
	// 待機用 (準備完了)ステート
	void ReadyState::Enter()
	{
		owner->SetAnimation(PlayerCharacter::Animation::ANIM_SWORD_IDLE, true, 0.1f);
	}
	void ReadyState::Execute(float elapsedTime)
	{
	}
	void ReadyState::Exit()
	{
	}
}
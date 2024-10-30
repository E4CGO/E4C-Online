#include "NonPlayerCharacterState.h"

#include "TAKOEngine/Effects/EffectManager.h"

#include "GameObject/Character/Character.h"

void NonPlayerCharacterState::PlayerTransition(NonPlayerCharacter* owner, uint32_t flags)
{
	if (flags & flag_Dodge && owner->InputDodge())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(NonPlayerCharacter::State::Dodge));
		return;
	}
	if (flags & flag_Jump && owner->InputJump())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(NonPlayerCharacter::State::Jump));
		return;
	}
	if (flags & flag_Move && owner->IsMove())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(NonPlayerCharacter::State::Move));
		return;
	}
	if (flags & flag_Stop && !owner->IsMove())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(NonPlayerCharacter::State::Idle));
		return;
	}
	if (flags & flag_Fall && owner->IsFall())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(NonPlayerCharacter::State::Fall));
		return;
	}
	if (flags & flag_Ground && owner->IsGround())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(NonPlayerCharacter::State::Land));
		return;
	}
	if (flags & flag_AttackN && owner->InputAttackNormal())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(NonPlayerCharacter::State::AttackNormal));
		return;
	}
	if (flags & flag_AttackS && owner->InputAttackSpecial())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(NonPlayerCharacter::State::AttackSpecial));
		return;
	}
	if (flags & flag_Skill_1 && owner->InputSkill1())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(NonPlayerCharacter::State::Skill_1));
		return;
	}
}
// 待機ステート
void NonPlayerCharacterState::IdleState::Enter()
{
	owner->SetAnimation(NonPlayerCharacter::Animation::Idle, true, 0.1f);
}
void NonPlayerCharacterState::IdleState::Execute(float elapsedTime)
{
	owner->RecoverMp(elapsedTime);

	owner->InputMove(elapsedTime);
	owner->Jump();

	PlayerTransition(
		owner,
		flag_Dodge | flag_Jump | flag_Move | flag_Fall | flag_AttackN | flag_AttackS | flag_Skill_1
	);
}
void NonPlayerCharacterState::IdleState::Exit()
{
}

// 移動ステート
void NonPlayerCharacterState::MoveState::Enter()
{
	owner->SetAnimation(NonPlayerCharacter::Animation::Running_A, true, 0.1f);
}
void NonPlayerCharacterState::MoveState::Execute(float elapsedTime)
{
	owner->RecoverMp(elapsedTime);

	owner->InputMove(elapsedTime);
	owner->Jump();

	PlayerTransition(
		owner,
		flag_Dodge | flag_Jump | flag_Stop | flag_Fall | flag_AttackN | flag_AttackS | flag_Skill_1
	);
}
void NonPlayerCharacterState::MoveState::Exit()
{
}

// ジャンプステート
void NonPlayerCharacterState::JumpState::Enter()
{
	owner->SetAnimation(NonPlayerCharacter::Animation::Jump_Start, false, 0.1f);
}
void NonPlayerCharacterState::JumpState::Execute(float elapsedTime)
{
	owner->InputMove(elapsedTime);

	PlayerTransition(
		owner,
		flag_Fall | flag_Dodge | flag_AttackN | flag_AttackS | flag_Skill_1
	);
}
void NonPlayerCharacterState::JumpState::Exit()
{
}

// 落下ステート
void NonPlayerCharacterState::FallState::Enter()
{
	owner->SetAnimation(NonPlayerCharacter::Animation::Jump_Idle, true, 0.1f);
}
void NonPlayerCharacterState::FallState::Execute(float elapsedTime)
{
	owner->InputMove(elapsedTime);

	PlayerTransition(
		owner,
		flag_Fall | flag_Dodge | flag_Ground | flag_AttackN | flag_AttackS | flag_Skill_1
	);
}
void NonPlayerCharacterState::FallState::Exit()
{
}

// 着地ステート
void NonPlayerCharacterState::LandState::Enter()
{
	owner->SetAnimation(NonPlayerCharacter::Animation::Jump_Land, false, 0.1f);
}
void NonPlayerCharacterState::LandState::Execute(float elapsedTime)
{
	owner->InputMove(elapsedTime);
	owner->Jump();

	PlayerTransition(
		owner,
		flag_Dodge | flag_Jump | flag_Move | flag_Fall | flag_AttackN | flag_AttackS | flag_Skill_1
	);

	if (!owner->IsPlayAnimaition()) owner->GetStateMachine()->ChangeState(static_cast<int>(NonPlayerCharacter::State::Idle)); return;
}
void NonPlayerCharacterState::LandState::Exit()
{
}

// 回避ステート
void NonPlayerCharacterState::DodgeState::Enter()
{
	owner->SetAnimation(NonPlayerCharacter::Animation::Dodge_Forward, false, 0.0f);
	owner->SetHurtCoolTime(0.2f);

	// MP消費
	owner->ModifyMp(-owner->GetMpCost(static_cast<int>(NonPlayerCharacter::State::Dodge)));
}
void NonPlayerCharacterState::DodgeState::Execute(float elapsedTime)
{
	if (!owner->GetModel()->IsPlayAnimation())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(NonPlayerCharacter::State::Idle));
		owner->StopMove();
	}
}
void NonPlayerCharacterState::DodgeState::Exit()
{
	owner->GetCollider()->SetEnable(true);
}

// 怪我
void NonPlayerCharacterState::HurtState::Enter()
{
	owner->SetAnimation(NonPlayerCharacter::Animation::Hit_A, false, 0.1f);
}
void NonPlayerCharacterState::HurtState::Execute(float elapsedTime)
{
	if (!owner->GetModel()->IsPlayAnimation())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(NonPlayerCharacter::State::Idle));
	}
}
void NonPlayerCharacterState::HurtState::Exit()
{
}
// 死亡
void NonPlayerCharacterState::DeathState::Enter()
{
	owner->SetAnimation(NonPlayerCharacter::Animation::Death_A, false, 0.1f);
	owner->GetCollider()->SetEnable(false);
}
void NonPlayerCharacterState::DeathState::Execute(float elapsedTime)
{
}
void NonPlayerCharacterState::DeathState::Exit()
{
}
// 待機用ステート
void NonPlayerCharacterState::WaitState::Enter()
{
	owner->SetAnimation(NonPlayerCharacter::Animation::Idle, true, 0.1f);
}
void NonPlayerCharacterState::WaitState::Execute(float elapsedTime)
{
}
void NonPlayerCharacterState::WaitState::Exit()
{
}
// 待機用 (準備完了)ステート
void NonPlayerCharacterState::ReadyState::Enter()
{
	owner->SetAnimation(NonPlayerCharacter::Animation::Cheer, true, 0.1f);
}
void NonPlayerCharacterState::ReadyState::Execute(float elapsedTime)
{
}
void NonPlayerCharacterState::ReadyState::Exit()
{
}
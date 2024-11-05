#include "PlayerCharacterState.h"

#include "TAKOEngine/Effects/EffectManager.h"

#include "GameObject/Character/Character.h"

void PlayerCharacterState::PlayerTransition(PlayerCharacter* owner, uint32_t flags)
{
	if (flags & flag_Dodge && owner->InputDodge())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::State::Dodge));
		return;
	}
	if (flags & flag_Jump && owner->InputJump())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::State::Jump));
		return;
	}
	if (flags & flag_Move && owner->IsMove())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::State::Move));
		return;
	}
	if (flags & flag_Stop && !owner->IsMove())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::State::Idle));
		return;
	}
	if (flags & flag_Fall && owner->IsFall())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::State::Fall));
		return;
	}
	if (flags & flag_Ground && owner->IsGround())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::State::Land));
		return;
	}
	if (flags & flag_AttackN && owner->InputAttackNormal())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::State::AttackNormal));
		return;
	}
	if (flags & flag_AttackS && owner->InputAttackSpecial())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::State::AttackSpecial));
		return;
	}
	if (flags & flag_Skill_1 && owner->InputSkill1())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::State::Skill_1));
		return;
	}
}
// 待機ステート
void PlayerCharacterState::IdleState::Enter()
{
	owner->SetAnimation(PlayerCharacter::Animation::Idle, true, 0.1f);
}
void PlayerCharacterState::IdleState::Execute(float elapsedTime)
{
	owner->RecoverMp(elapsedTime);

	owner->InputMove(elapsedTime);
	owner->Jump();

	PlayerTransition(
		owner,
		flag_Dodge | flag_Jump | flag_Move | flag_Fall | flag_AttackN | flag_AttackS | flag_Skill_1
	);
}
void PlayerCharacterState::IdleState::Exit()
{
}

// 移動ステート
void PlayerCharacterState::MoveState::Enter()
{
	owner->SetAnimation(PlayerCharacter::Animation::Running_A, true, 0.1f);
}
void PlayerCharacterState::MoveState::Execute(float elapsedTime)
{
	owner->RecoverMp(elapsedTime);

	owner->InputMove(elapsedTime);
	owner->Jump();

	PlayerTransition(
		owner,
		flag_Dodge | flag_Jump | flag_Stop | flag_Fall | flag_AttackN | flag_AttackS | flag_Skill_1
	);
}
void PlayerCharacterState::MoveState::Exit()
{
}

// ジャンプステート
void PlayerCharacterState::JumpState::Enter()
{
	owner->SetAnimation(PlayerCharacter::Animation::Jump_Start, false, 0.1f);
}
void PlayerCharacterState::JumpState::Execute(float elapsedTime)
{
	owner->InputMove(elapsedTime);

	PlayerTransition(
		owner,
		flag_Fall | flag_Dodge | flag_Ground | flag_AttackN | flag_AttackS | flag_Skill_1
	);
}
void PlayerCharacterState::JumpState::Exit()
{
}

// 落下ステート
void PlayerCharacterState::FallState::Enter()
{
	owner->SetAnimation(PlayerCharacter::Animation::Jump_Idle, true, 0.1f);
}
void PlayerCharacterState::FallState::Execute(float elapsedTime)
{
	owner->InputMove(elapsedTime);

	PlayerTransition(
		owner,
		flag_Fall | flag_Dodge | flag_Ground | flag_AttackN | flag_AttackS | flag_Skill_1
	);
}
void PlayerCharacterState::FallState::Exit()
{
}

// 着地ステート
void PlayerCharacterState::LandState::Enter()
{
	owner->SetAnimation(PlayerCharacter::Animation::Jump_Land, false, 0.1f);
}
void PlayerCharacterState::LandState::Execute(float elapsedTime)
{
	owner->InputMove(elapsedTime);
	owner->Jump();

	PlayerTransition(
		owner,
		flag_Dodge | flag_Jump | flag_Move | flag_Fall | flag_AttackN | flag_AttackS | flag_Skill_1
	);

	if (!owner->IsPlayAnimaition()) owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::State::Idle)); return;
}
void PlayerCharacterState::LandState::Exit()
{
}

// 回避ステート
void PlayerCharacterState::DodgeState::Enter()
{
	owner->SetAnimation(PlayerCharacter::Animation::Dodge_Forward, false, 0.0f);
	owner->SetHurtCoolTime(0.2f);

	// MP消費
	owner->ModifyMp(-owner->GetMpCost(static_cast<int>(PlayerCharacter::State::Dodge)));
}
void PlayerCharacterState::DodgeState::Execute(float elapsedTime)
{
	if (!owner->GetModel()->IsPlayAnimation())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::State::Idle));
		owner->StopMove();
	}
}
void PlayerCharacterState::DodgeState::Exit()
{
	owner->GetCollider()->SetEnable(true);
}

// 怪我
void PlayerCharacterState::HurtState::Enter()
{
	owner->SetAnimation(PlayerCharacter::Animation::Hit_A, false, 0.1f);
}
void PlayerCharacterState::HurtState::Execute(float elapsedTime)
{
	if (!owner->GetModel()->IsPlayAnimation())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::State::Idle));
	}
}
void PlayerCharacterState::HurtState::Exit()
{
}
// 死亡
void PlayerCharacterState::DeathState::Enter()
{
	owner->SetAnimation(PlayerCharacter::Animation::Death_A, false, 0.1f);
	owner->GetCollider()->SetEnable(false);
}
void PlayerCharacterState::DeathState::Execute(float elapsedTime)
{
}
void PlayerCharacterState::DeathState::Exit()
{
}
// 待機用ステート
void PlayerCharacterState::WaitState::Enter()
{
	owner->SetAnimation(PlayerCharacter::Animation::Idle, true, 0.1f);
}
void PlayerCharacterState::WaitState::Execute(float elapsedTime)
{
}
void PlayerCharacterState::WaitState::Exit()
{
}
// 待機用 (準備完了)ステート
void PlayerCharacterState::ReadyState::Enter()
{
	owner->SetAnimation(PlayerCharacter::Animation::Cheer, true, 0.1f);
}
void PlayerCharacterState::ReadyState::Execute(float elapsedTime)
{
}
void PlayerCharacterState::ReadyState::Exit()
{
}
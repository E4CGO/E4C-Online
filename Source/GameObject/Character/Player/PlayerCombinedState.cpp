#include "PlayerCombinedState.h"

#include "TAKOEngine/Effects/EffectManager.h"

#include "GameObject/Character/Character.h"
#include "GameObject/Character/Player/CombinedPlayer.h"

void PlayerCombinedState::PlayerTransition(CombinedPlayer* owner, uint32_t flags)
{
	if (flags & flag_Dodge && owner->InputDodge())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(CombinedPlayer::State::Dodge));
		return;
	}
	if (flags & flag_Jump && owner->InputJump())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(CombinedPlayer::State::Jump));
		return;
	}
	if (flags & flag_Move && owner->IsMove())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(CombinedPlayer::State::Move));
		return;
	}
	if (flags & flag_Stop && !owner->IsMove())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(CombinedPlayer::State::Idle));
		return;
	}
	if (flags & flag_Fall && owner->IsFall())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(CombinedPlayer::State::Fall));
		return;
	}
	if (flags & flag_Ground && owner->IsGround())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(CombinedPlayer::State::Land));
		return;
	}
	if (flags & flag_AttackN && owner->InputAttackNormal())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(CombinedPlayer::State::AttackNormal));
		return;
	}
	if (flags & flag_AttackS && owner->InputAttackSpecial())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(CombinedPlayer::State::AttackSpecial));
		return;
	}
	if (flags & flag_Skill_1 && owner->InputSkill1())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(CombinedPlayer::State::Skill_1));
		return;
	}
}
// 待機ステート
void PlayerCombinedState::IdleState::Enter()
{
	owner->PlayAnimations(CombinedPlayer::Animation::Idle, true, 0.1f);
}
void PlayerCombinedState::IdleState::Execute(float elapsedTime)
{
	owner->RecoverMp(elapsedTime);

	owner->InputMove(elapsedTime);
	owner->Jump();

	PlayerTransition(
		owner,
		flag_Dodge | flag_Jump | flag_Move | flag_Fall | flag_AttackN | flag_AttackS | flag_Skill_1
	);
}
void PlayerCombinedState::IdleState::Exit()
{
}

// 移動ステート
void PlayerCombinedState::MoveState::Enter()
{
	owner->PlayAnimations(CombinedPlayer::Animation::Running_A, true, 0.1f);
}
void PlayerCombinedState::MoveState::Execute(float elapsedTime)
{
	owner->RecoverMp(elapsedTime);

	owner->InputMove(elapsedTime);
	owner->Jump();

	PlayerTransition(
		owner,
		flag_Dodge | flag_Jump | flag_Stop | flag_Fall | flag_AttackN | flag_AttackS | flag_Skill_1
	);
}
void PlayerCombinedState::MoveState::Exit()
{
}

// ジャンプステート
void PlayerCombinedState::JumpState::Enter()
{
	owner->PlayAnimations(CombinedPlayer::Animation::Jump_Start, false, 0.1f);
}
void PlayerCombinedState::JumpState::Execute(float elapsedTime)
{
	owner->InputMove(elapsedTime);

	PlayerTransition(
		owner,
		flag_Fall | flag_Dodge | flag_AttackN | flag_AttackS | flag_Skill_1
	);
}
void PlayerCombinedState::JumpState::Exit()
{
}

// 落下ステート
void PlayerCombinedState::FallState::Enter()
{
	owner->PlayAnimations(CombinedPlayer::Animation::Jump_Idle, true, 0.1f);
}
void PlayerCombinedState::FallState::Execute(float elapsedTime)
{
	owner->InputMove(elapsedTime);

	PlayerTransition(
		owner,
		flag_Fall | flag_Dodge | flag_Ground | flag_AttackN | flag_AttackS | flag_Skill_1
	);
}
void PlayerCombinedState::FallState::Exit()
{
}

// 着地ステート
void PlayerCombinedState::LandState::Enter()
{
	owner->PlayAnimations(CombinedPlayer::Animation::Jump_Land, false, 0.1f);
}
void PlayerCombinedState::LandState::Execute(float elapsedTime)
{
	owner->InputMove(elapsedTime);
	owner->Jump();

	PlayerTransition(
		owner,
		flag_Dodge | flag_Jump | flag_Move | flag_Fall | flag_AttackN | flag_AttackS | flag_Skill_1
	);

	if (!owner->IsPlayAnimations()) owner->GetStateMachine()->ChangeState(static_cast<int>(CombinedPlayer::State::Idle)); return;
}
void PlayerCombinedState::LandState::Exit()
{
}

// 回避ステート
void PlayerCombinedState::DodgeState::Enter()
{
	owner->PlayAnimations(CombinedPlayer::Animation::Dodge_Forward, false, 0.0f);
	owner->SetHurtCoolTime(0.2f);

	// MP消費
	owner->ModifyMp(-owner->GetMpCost(static_cast<int>(CombinedPlayer::State::Dodge)));
}
void PlayerCombinedState::DodgeState::Execute(float elapsedTime)
{
	if (!owner->GetModel()->IsPlayAnimation())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(CombinedPlayer::State::Idle));
		owner->StopMove();
	}
}
void PlayerCombinedState::DodgeState::Exit()
{
	owner->GetCollider()->SetEnable(true);
}

// 怪我
void PlayerCombinedState::HurtState::Enter()
{
	owner->PlayAnimations(CombinedPlayer::Animation::Hit_A, false, 0.1f);
}
void PlayerCombinedState::HurtState::Execute(float elapsedTime)
{
	if (!owner->GetModel()->IsPlayAnimation())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(CombinedPlayer::State::Idle));
	}
}
void PlayerCombinedState::HurtState::Exit()
{
}
// 死亡
void PlayerCombinedState::DeathState::Enter()
{
	owner->PlayAnimations(CombinedPlayer::Animation::Death_A, false, 0.1f);
	owner->GetCollider()->SetEnable(false);
}
void PlayerCombinedState::DeathState::Execute(float elapsedTime)
{
}
void PlayerCombinedState::DeathState::Exit()
{
}
// 待機用ステート
void PlayerCombinedState::WaitState::Enter()
{
	owner->PlayAnimations(CombinedPlayer::Animation::Idle, true, 0.1f);
}
void PlayerCombinedState::WaitState::Execute(float elapsedTime)
{
}
void PlayerCombinedState::WaitState::Exit()
{
}
// 待機用 (準備完了)ステート
void PlayerCombinedState::ReadyState::Enter()
{
	owner->PlayAnimations(CombinedPlayer::Animation::Cheer, true, 0.1f);
}
void PlayerCombinedState::ReadyState::Execute(float elapsedTime)
{
}
void PlayerCombinedState::ReadyState::Exit()
{
}
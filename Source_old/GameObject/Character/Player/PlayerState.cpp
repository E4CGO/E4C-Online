#include "PlayerState.h"

#include "TAKOEngine/Effects/EffectManager.h"

#include "GameObject/Character/Character.h"
#include "GameObject/Character/Player/Player.h"

void PlayerState::PlayerTransition(Player* owner, uint32_t flags)
{
	if (flags & flag_Dodge && owner->InputDodge())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Dodge));
		return;
	}
	if (flags & flag_Jump && owner->InputJump())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Jump));
		return;
	}
	if (flags & flag_Move && owner->IsMove())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Move));
		return;
	}
	if (flags & flag_Stop && !owner->IsMove())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));
		return;
	}
	if (flags & flag_Fall && owner->IsFall())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Fall));
		return;
	}
	if (flags & flag_Ground && owner->IsGround())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Land));
		return;
	}
	if (flags & flag_AttackN && owner->InputAttackNormal())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::AttackNormal));
		return;
	}
	if (flags & flag_AttackS && owner->InputAttackSpecial())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::AttackSpecial));
		return;
	}
	if (flags & flag_Skill_1 && owner->InputSkill1())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Skill_1));
		return;
	}
}
// 待機ステート
void PlayerState::IdleState::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::Idle, true, 0.1f);
}
void PlayerState::IdleState::Execute(float elapsedTime)
{
	owner->RecoverMp(elapsedTime);

	owner->InputMove(elapsedTime);
	owner->Jump();

	PlayerTransition(
		owner,
		flag_Dodge | flag_Jump | flag_Move | flag_Fall | flag_AttackN | flag_AttackS | flag_Skill_1
	);
}
void PlayerState::IdleState::Exit()
{
}

// 移動ステート
void PlayerState::MoveState::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::Running_A, true, 0.1f);
}
void PlayerState::MoveState::Execute(float elapsedTime)
{
	owner->RecoverMp(elapsedTime);

	owner->InputMove(elapsedTime);
	owner->Jump();

	PlayerTransition(
		owner,
		flag_Dodge | flag_Jump | flag_Stop | flag_Fall | flag_AttackN | flag_AttackS | flag_Skill_1
	);
}
void PlayerState::MoveState::Exit()
{
}

// ジャンプステート
void PlayerState::JumpState::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::Jump_Start, false, 0.1f);
}
void PlayerState::JumpState::Execute(float elapsedTime)
{
	owner->InputMove(elapsedTime);

	PlayerTransition(
		owner,
		flag_Fall | flag_Dodge | flag_AttackN | flag_AttackS | flag_Skill_1
	);
}
void PlayerState::JumpState::Exit()
{
}

// 落下ステート
void PlayerState::FallState::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::Jump_Idle, true, 0.1f);
}
void PlayerState::FallState::Execute(float elapsedTime)
{
	owner->InputMove(elapsedTime);

	PlayerTransition(
		owner,
		flag_Fall | flag_Dodge | flag_Ground | flag_AttackN | flag_AttackS | flag_Skill_1
	);
}
void PlayerState::FallState::Exit()
{
}

// 着地ステート
void PlayerState::LandState::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::Jump_Land, false, 0.1f);
}
void PlayerState::LandState::Execute(float elapsedTime)
{
	owner->InputMove(elapsedTime);
	owner->Jump();

	PlayerTransition(
		owner,
		flag_Dodge | flag_Jump | flag_Move | flag_Fall | flag_AttackN | flag_AttackS | flag_Skill_1
	);

	if (!owner->GetModel()->IsPlayAnimation()) owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle)); return;
}
void PlayerState::LandState::Exit()
{
}

// 回避ステート
void PlayerState::DodgeState::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::Dodge_Forward, false, 0.0f);
	owner->SetHurtCoolTime(0.2f);

	// MP消費
	owner->ModifyMp(-owner->GetMpCost(static_cast<int>(Player::State::Dodge)));
}
void PlayerState::DodgeState::Execute(float elapsedTime)
{
	if (!owner->GetModel()->IsPlayAnimation())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));
		owner->StopMove();
	}
}
void PlayerState::DodgeState::Exit()
{
	owner->GetCollider()->SetEnable(true);
}

// 怪我
void PlayerState::HurtState::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::Hit_A, false, 0.1f);
}
void PlayerState::HurtState::Execute(float elapsedTime)
{
	if (!owner->GetModel()->IsPlayAnimation())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));
	}
}
void PlayerState::HurtState::Exit()
{
}
// 死亡
void PlayerState::DeathState::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::Death_A, false, 0.1f);
	owner->GetCollider()->SetEnable(false);
}
void PlayerState::DeathState::Execute(float elapsedTime)
{
}
void PlayerState::DeathState::Exit()
{
}
// 待機用ステート
void PlayerState::WaitState::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::Idle, true, 0.1f);
}
void PlayerState::WaitState::Execute(float elapsedTime)
{
}
void PlayerState::WaitState::Exit()
{
}
// 待機用 (準備完了)ステート
void PlayerState::ReadyState::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::Cheer, true, 0.1f);
}
void PlayerState::ReadyState::Execute(float elapsedTime)
{
}
void PlayerState::ReadyState::Exit()
{
}
#include "KnightState.h"
#include "Knight.h"
#include "Player.h"

// 一般攻撃ステート
void KnightState::AttackNormalState::Enter()
{
	owner->SetAnimationSpeed(1.5f);

	SetSubState(Knight::ATTACK_STATE::ATTACK_1);
}
void KnightState::AttackNormalState::Execute(float elapsedTime)
{
	subState->Execute(elapsedTime);

	// 反重力
	owner->StopFall();
	owner->StopMove();

	if (!owner->GetModel()->IsPlayAnimation()) // 攻撃モーション終わり
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));
	}
}
void KnightState::AttackNormalState::Exit()
{
	owner->SetAnimationSpeed(1.0f);
	owner->GetAttackCollider(Knight::AttackCollider::Sword_1)->SetEnable(false);
	owner->GetAttackCollider(Knight::AttackCollider::Sword_2)->SetEnable(false);
	owner->GetAttackCollider(Knight::AttackCollider::Sword_3)->SetEnable(false);
}
//  一般攻撃1
void KnightState::AttackNormalState_1::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::OneHanded_Melee_Attack_Slice_Diagonal, false, 0.05f);
}
void KnightState::AttackNormalState_1::Execute(float elapsedTime)
{
	if (!owner->IsPlayer()) return;

	if (owner->GetModel()->GetAnimationRate() > 0.2f)
	{
		owner->GetAttackCollider(Knight::AttackCollider::Sword_1)->SetEnable(true);
		owner->GetAttackCollider(Knight::AttackCollider::Sword_2)->SetEnable(true);
		owner->GetAttackCollider(Knight::AttackCollider::Sword_3)->SetEnable(true);
	}

	if (owner->GetModel()->GetAnimationRate() > 0.75f)
	{
		owner->GetAttackCollider(Knight::AttackCollider::Sword_1)->SetEnable(false);
		owner->GetAttackCollider(Knight::AttackCollider::Sword_2)->SetEnable(false);
		owner->GetAttackCollider(Knight::AttackCollider::Sword_3)->SetEnable(false);
		if (owner->InputAttackNormal()) // アニメーション75%完成
		{
			owner->GetStateMachine()->ChangeSubState(Knight::ATTACK_STATE::ATTACK_2);
		}
	}
}
//  一般攻撃2
void KnightState::AttackNormalState_2::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::OneHanded_Melee_Attack_Chop, false, 0.05f);
}
void KnightState::AttackNormalState_2::Execute(float elapsedTime)
{
	if (!owner->IsPlayer()) return;
	if (owner->GetModel()->GetAnimationRate() > 0.2f)
	{
		owner->GetAttackCollider(Knight::AttackCollider::Sword_1)->SetEnable(true);
		owner->GetAttackCollider(Knight::AttackCollider::Sword_2)->SetEnable(true);
		owner->GetAttackCollider(Knight::AttackCollider::Sword_3)->SetEnable(true);
	}
	if (owner->GetModel()->GetAnimationRate() > 0.75f)
	{
		owner->GetAttackCollider(Knight::AttackCollider::Sword_1)->SetEnable(false);
		owner->GetAttackCollider(Knight::AttackCollider::Sword_2)->SetEnable(false);
		owner->GetAttackCollider(Knight::AttackCollider::Sword_3)->SetEnable(false);
		if (owner->InputAttackNormal()) // アニメーション75%完成
		{
			owner->GetStateMachine()->ChangeSubState(Knight::ATTACK_STATE::ATTACK_3);
		}
	}
}
//  一般攻撃3
void KnightState::AttackNormalState_3::Enter()
{
	owner->SetAnimationSpeed(1.2f);
	owner->GetModel()->PlayAnimation(Player::Animation::OneHanded_Melee_Attack_Slice_Horizontal, false, 0.05f);
}
void KnightState::AttackNormalState_3::Execute(float elapsedTime)
{
	if (!owner->IsPlayer()) return;
	if (owner->GetModel()->GetAnimationRate() > 0.2f)
	{
		owner->GetAttackCollider(Knight::AttackCollider::Sword_1)->SetEnable(true);
		owner->GetAttackCollider(Knight::AttackCollider::Sword_2)->SetEnable(true);
		owner->GetAttackCollider(Knight::AttackCollider::Sword_3)->SetEnable(true);
	}
	if (owner->GetModel()->GetAnimationRate() > 0.75f)
	{
		owner->GetAttackCollider(Knight::AttackCollider::Sword_1)->SetEnable(false);
		owner->GetAttackCollider(Knight::AttackCollider::Sword_2)->SetEnable(false);
		owner->GetAttackCollider(Knight::AttackCollider::Sword_3)->SetEnable(false);
	}
}
// 特殊攻撃ステート
void KnightState::AttackSpecialState::Enter()
{
	owner->SetAnimationSpeed(2.0f);
	SetSubState(Knight::BLOCK_STATE::BLOCK_START);
}
void KnightState::AttackSpecialState::Execute(float elapsedTime)
{
	subState->Execute(elapsedTime);

	if (owner->ReleaseAttackSpecial()) // リリース
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));	// 待機に戻る
		return;
	}
}
void KnightState::AttackSpecialState::Exit()
{
	owner->SetAnimationSpeed(1.0f);
	owner->GetAttackCollider(Knight::AttackCollider::Shield)->SetEnable(false);
}

void KnightState::AttackSpecialStateStart::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::Block, false, 0.05f);
}
void KnightState::AttackSpecialStateStart::Execute(float elapsedTime)
{
	if (!owner->IsPlayer()) return;
	PlayerState::PlayerTransition(owner, flag_Skill_1);
	if (!owner->GetModel()->IsPlayAnimation())  //アニメーション終了
	{
		owner->GetStateMachine()->ChangeSubState(Knight::BLOCK_STATE::BLOCKING);
	}
}
void KnightState::AttackSpecialStateBlocking::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::Blocking, true, 0.05f);
}
void KnightState::AttackSpecialStateBlocking::Execute(float elapsedTime)
{
	if (!owner->IsPlayer()) return;

	if (owner->InputAttackNormal())
	{
		owner->GetStateMachine()->ChangeSubState(Knight::BLOCK_STATE::BLOCK_ATTACK);
		return;
	}
}
void KnightState::AttackSpecialStateAttack::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::Block_Attack, false);
}
void KnightState::AttackSpecialStateAttack::Execute(float elapsedTime)
{
	if (!owner->IsPlayer()) return;
	if (owner->GetModel()->GetAnimationRate() > 0.2f)
	{
		owner->GetAttackCollider(Knight::AttackCollider::Shield)->SetEnable(true);
	}

	if (!owner->GetModel()->IsPlayAnimation())  //アニメーション終了
	{
		owner->GetStateMachine()->ChangeSubState(Knight::BLOCK_STATE::BLOCKING);
	}
}
void KnightState::AttackSpecialStateAttack::Exit()
{
	owner->GetAttackCollider(Knight::AttackCollider::Shield)->SetEnable(false);
}

void KnightState::AttackSpecialStateHit::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::Block_Hit, false);
}
void KnightState::AttackSpecialStateHit::Execute(float elapsedTime)
{
	if (!owner->IsPlayer()) return;
	if (!owner->GetModel()->IsPlayAnimation())  //アニメーション終了
	{
		owner->GetStateMachine()->ChangeSubState(Knight::BLOCK_STATE::BLOCKING);
	}
}
// スキル_1ステート
void KnightState::Skill1State::Enter()
{
	owner->SetAnimationSpeed(2.0f);
	owner->GetModel()->PlayAnimation(Player::Animation::Blocking, false, 0.05f);

	if (!owner->IsPlayer()) return;
	DirectX::XMFLOAT2 inputDirection = owner->GetInputDirection();
	owner->StopMove();
	DirectX::XMFLOAT3 impulse = { inputDirection.x, 0, inputDirection.y };
	owner->AddImpulse(impulse * 20.0f);

	// 盾衝突判定
	owner->GetAttackCollider(Knight::AttackCollider::Shield)->SetEnable(true);

	owner->SkillCost(static_cast<int>(Player::State::Skill_1));
}
void KnightState::Skill1State::Execute(float elapsedtime)
{
	if (!owner->GetModel()->IsPlayAnimation())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));
		owner->StopMove();
	}
}
void KnightState::Skill1State::Exit()
{
	owner->SetAnimationSpeed(1.0f);
	owner->GetAttackCollider(Knight::AttackCollider::Shield)->SetEnable(false);
}
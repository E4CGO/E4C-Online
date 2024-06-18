#include "BarbarianState.h"

#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"

#include "GameObject/Projectile/ProjectileManager.h"
#include "GameObject/Projectile/TwoHandedAxe.h"
#include "GameObject/Character/Player/Player.h"
#include "GameObject/Character/Player/Barbarian.h"

// 待機ステート
void BarbarianState::IdleState::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::TwoHanded_Melee_Idle, true, 0.1f);
}
void BarbarianState::WaitState::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::TwoHanded_Melee_Idle, true, 0.1f);
}

// 一般攻撃ステート
void BarbarianState::AttackNormalState::Enter()
{
	owner->SetAnimationSpeed(2.0f);
	SetSubState(Barbarian::ATTACK_STATE::ATTACK_1);
}
void BarbarianState::AttackNormalState::Execute(float elapsedTime)
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
void BarbarianState::AttackNormalState::Exit()
{
	owner->SetAnimationSpeed(1.0f);
}
//  一般攻撃1
void BarbarianState::AttackNormalState_1::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::TwoHanded_Melee_Attack_Slice, false, 0.05f);
}
void BarbarianState::AttackNormalState_1::Execute(float elapsedTime)
{
	if (!owner->IsPlayer()) return;

	if (owner->GetModel()->GetAnimationRate() > 0.2f)
	{
		owner->GetAttackCollider(Barbarian::AttackCollider::Axe)->SetEnable(true);
	}

	if (owner->GetModel()->GetAnimationRate() > 0.75f) // アニメーション75%完成
	{
		owner->GetAttackCollider(Barbarian::AttackCollider::Axe)->SetEnable(false);
		if (owner->InputAttackNormal()) {
			owner->GetStateMachine()->ChangeSubState(Barbarian::ATTACK_STATE::ATTACK_2);
		}
	}
}
//  一般攻撃2
void BarbarianState::AttackNormalState_2::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::TwoHanded_Melee_Attack_Chop, false, 0.05f);
}
void BarbarianState::AttackNormalState_2::Execute(float elapsedTime)
{
	if (!owner->IsPlayer()) return;
	if (owner->GetModel()->GetAnimationRate() > 0.2f)
	{
		owner->GetAttackCollider(Barbarian::AttackCollider::Axe)->SetEnable(true);
	}

	if (owner->GetModel()->GetAnimationRate() > 0.75f) // アニメーション75%完成
	{
		owner->GetAttackCollider(Barbarian::AttackCollider::Axe)->SetEnable(false);
		if (owner->InputAttackNormal()) {
			owner->GetStateMachine()->ChangeSubState(Barbarian::ATTACK_STATE::ATTACK_3);
		}
	}
}
//  一般攻撃3
void BarbarianState::AttackNormalState_3::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::TwoHanded_Melee_Attack_Spin, false, 0.05f);
}
void BarbarianState::AttackNormalState_3::Execute(float elapsedTime)
{
	if (!owner->IsPlayer()) return;
	if (owner->GetModel()->GetAnimationRate() > 0.2f)
	{
		owner->GetAttackCollider(Barbarian::AttackCollider::Axe)->SetEnable(true);
	}

	if (owner->GetModel()->GetAnimationRate() > 0.75f) // アニメーション75%完成
	{
		owner->GetAttackCollider(Barbarian::AttackCollider::Axe)->SetEnable(false);
	}
}

// 特殊攻撃ステート
void BarbarianState::AttackSpecialState::Enter()
{
	owner->SetAnimationSpeed(1.2f);
	SetSubState(Barbarian::THROW_STATE::THROW_AIM);
}
void BarbarianState::AttackSpecialState::Execute(float elapsedTime)
{
	subState->Execute(elapsedTime);

	if (!owner->GetModel()->IsPlayAnimation()) // アニメーション終了
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));
	}
}
void BarbarianState::AttackSpecialState::Exit()
{
	owner->SetAnimationSpeed(1.0f);
	owner->GetModel()->FindNode("2H_Axe")->visible = true;
	if (owner->IsPlayer()) TPSCamera.AimMode(false);	// AIM
}
void BarbarianState::AttackSpecialStateAim::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::Throw, false, 0.05f);
	if (owner->IsPlayer()) TPSCamera.AimMode(true);	// AIM
}
void BarbarianState::AttackSpecialStateAim::Execute(float elapsedTime)
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
void BarbarianState::AttackSpecialStateAim::Exit()
{
}

void BarbarianState::AttackSpecialStateAttack::Enter()
{
	isShot = false;
}
void BarbarianState::AttackSpecialStateAttack::Execute(float elapsedTime)
{
	if (owner->GetModel()->GetAnimationRate() > 0.4f && !isShot)
	{
		// 発射
		Projectile* axe = PROJECTILES.Register(new TwoHandedAxe(owner));
		axe->SetPosition(owner->GetShotPosition());
		axe->PointTo(owner->GetTarget());

		DirectX::XMFLOAT3 direction = XMFLOAT3Normalize(owner->GetTarget() - owner->GetShotPosition());
		DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);
		Direction = DirectX::XMVector3Rotate(Direction, DirectX::XMQuaternionRotationAxis(DirectX::XMVector3Cross(Direction, DirectX::XMVectorSet(0, 1, 0, 0)), DirectX::XMConvertToRadians(15.0f)));
		DirectX::XMStoreFloat3(&direction, Direction);
		axe->SetDirection(direction);
		axe->SetOwner(owner);

		isShot = true;
		owner->GetModel()->FindNode("2H_Axe")->visible = false;
	}
}

// スキル_1ステート
void BarbarianState::Skill1State::Enter()
{
	owner->SetAnimationSpeed(1.5f);
	SetSubState(Barbarian::SKILL_1_STATE::SKILL_1_START);
	cacheTurnSpeed = owner->GetTurnSpeed();
	owner->SetTurnSpeed(0);

	owner->SkillCost(static_cast<int>(Player::State::Skill_1));
}
void BarbarianState::Skill1State::Execute(float elapsedTime)
{
	subState->Execute(elapsedTime);

	if (owner->GetMp() <= 0.0f || owner->ReleaseSkill1())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));
	}
}
void BarbarianState::Skill1State::Exit()
{
	owner->SetAnimationSpeed(1.0f);
	owner->SetTurnSpeed(cacheTurnSpeed);
}
void BarbarianState::Skill1StateStart::Enter()
{
	owner->SetAnimation(Player::Animation::TwoHanded_Melee_Attack_Spin, false);
}
void BarbarianState::Skill1StateStart::Execute(float elapsedTime)
{
	if (owner->GetModel()->GetAnimationRate() > 0.15f)
	{
		owner->GetStateMachine()->ChangeSubState(Barbarian::SKILL_1_STATE::SKILL_1_LOOP);
	}
}
void BarbarianState::Skill1StateLoop::Enter()
{
	owner->GetAttackCollider(Barbarian::AttackCollider::Axe)->SetEnable(true);
	owner->SetAnimation(Player::Animation::TwoHanded_Melee_Attack_Spinning, true);
}
void BarbarianState::Skill1StateLoop::Execute(float elapsedTime)
{
	owner->InputMove(elapsedTime);
	owner->ModifyMp(-elapsedTime * 15.0f);
}
void BarbarianState::Skill1StateLoop::Exit()
{
	owner->GetAttackCollider(Barbarian::AttackCollider::Axe)->SetEnable(false);
}
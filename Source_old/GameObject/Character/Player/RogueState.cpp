#include "RogueState.h"

#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"

#include "GameObject/Character/Player/Player.h"
#include "GameObject/Character/Player/Rogue.h"
#include "GameObject/Projectile/ProjectileManager.h"
#include "GameObject/Projectile/Arrow.h"
#include "GameObject/Projectile/Grenade.h"

// 一般攻撃ステート
void RogueState::AttackNormalState::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::TwoHanded_Ranged_Shoot, false, 0.1f);
	isShot = false;

	owner->SetAnimationSpeed(1.2f);
}
void RogueState::AttackNormalState::Execute(float elapsedTime)
{
	if (owner->GetModel()->GetAnimationRate() <= 0.3f)
	{
		if (owner->IsPlayer()) owner->TurnByInput();
	}
	else if (!isShot)
	{
		Projectile* arrow = PROJECTILES.Register(new Arrow(owner));
		arrow->SetPosition(owner->GetShotPosition());
		arrow->PointTo(owner->GetShotPosition() + owner->GetFront());
		arrow->SetDirection(owner->GetFront());
		arrow->SetOwner(owner);

		isShot = true;
	}
	if (!owner->GetModel()->IsPlayAnimation()) owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));

	// 反重力
	owner->StopFall();
	owner->StopMove();
}
void RogueState::AttackNormalState::Exit()
{
	owner->SetAnimationSpeed(1.0f);
}

// 特殊攻撃ステート
void RogueState::AttackSpecialState::Enter()
{
	SetSubState(Rogue::CHARGE_STATE::CHARGE_AIM);
	if (owner->IsPlayer()) TPSCamera.AimMode(true);
	owner->SetAnimationSpeed(1.8f);
}
void RogueState::AttackSpecialState::Execute(float elapsedTime)
{
	subState->Execute(elapsedTime);
	if (owner->IsPlayer())
	{
		owner->FaceToCamera();

		if (owner->ReleaseAttackSpecial()) owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));
	}
}
void RogueState::AttackSpecialState::Exit()
{
	if (owner->IsPlayer()) TPSCamera.AimMode(false);

	owner->SetAnimationSpeed(1.0f);
}

void RogueState::AttackSpecialStateAim::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::TwoHanded_Ranged_Aiming, false, 0.1f);
}

void RogueState::AttackSpecialStateAim::Execute(float elapsedTime)
{
	if (owner->IsPlayer() && owner->InputAttackNormal()) owner->GetStateMachine()->ChangeSubState(Rogue::CHARGE_STATE::CHARGE_SHOOT);
}

void RogueState::AttackSpecialStateShot::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::TwoHanded_Ranged_Shoot, false, 0.1f);
	owner->GetModel()->SetAnimationRate(0.3f);
	isShot = false;
}

void RogueState::AttackSpecialStateShot::Execute(float elapsedTime)
{
	if (!isShot)
	{
		Projectile* arrow = PROJECTILES.Register(new Arrow(owner));
		arrow->SetPosition(owner->GetShotPosition());
		arrow->PointTo(owner->GetTarget());
		arrow->SetDirection(XMFLOAT3Normalize(owner->GetTarget() - owner->GetShotPosition()));
		arrow->SetOwner(owner);
		isShot = true;
	}
	if (!owner->GetModel()->IsPlayAnimation()) owner->GetStateMachine()->ChangeSubState(Rogue::CHARGE_STATE::CHARGE_AIM);
}

// スキル1ステート
void RogueState::Skill1State::Enter()
{
	owner->SetAnimationSpeed(1.2f);
	owner->GetModel()->FindNode("2H_Crossbow")->visible = false;
	SetSubState(Rogue::SKILL_1_STATE::SKILL_1_AIM);

	owner->SkillCost(static_cast<int>(Player::State::Skill_1));
}
void RogueState::Skill1State::Execute(float elapsedTime)
{
	subState->Execute(elapsedTime);

	if (!owner->GetModel()->IsPlayAnimation()) // アニメーション終了
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));
	}
}
void RogueState::Skill1State::Exit()
{
	owner->SetAnimationSpeed(1.0f);
	owner->GetModel()->FindNode("2H_Crossbow")->visible = true;
	if (owner->IsPlayer()) TPSCamera.AimMode(false);	// AIM
}

void RogueState::Skill1StateAim::Enter()
{
	owner->GetModel()->PlayAnimation(Player::Animation::Throw, false, 0.05f);
	if (owner->IsPlayer()) TPSCamera.AimMode(true);	// AIM
}
void RogueState::Skill1StateAim::Execute(float elapsedTime)
{
	if (owner->GetModel()->GetAnimationRate() > 0.375f)
	{
		owner->GetModel()->SetAnimationRate(0.375f);
	}

	// 自機オンリー
	if (!owner->IsPlayer()) return;

	owner->FaceToCamera();

	if (owner->ReleaseSkill1()) // スキルボタンリリース
	{
		owner->GetStateMachine()->ChangeSubState(Rogue::SKILL_1_STATE::SKILL_1_THROW);
	}
}
void RogueState::Skill1StateAim::Exit()
{
}

void RogueState::Skill1StateAttack::Enter()
{
	isShot = false;
}
void RogueState::Skill1StateAttack::Execute(float elapsedTime)
{
	if (owner->GetModel()->GetAnimationRate() > 0.4f && !isShot)
	{
		// 発射
		Projectile* grenade = PROJECTILES.Register(new Grenade(owner));
		grenade->SetPosition(owner->GetShotPosition());
		grenade->PointTo(owner->GetTarget());

		DirectX::XMFLOAT3 direction = XMFLOAT3Normalize(owner->GetTarget() - owner->GetShotPosition());
		DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);
		Direction = DirectX::XMVector3Rotate(Direction, DirectX::XMQuaternionRotationAxis(DirectX::XMVector3Cross(Direction, DirectX::XMVectorSet(0, 1, 0, 0)), DirectX::XMConvertToRadians(10.0f)));
		DirectX::XMStoreFloat3(&direction, Direction);
		grenade->SetDirection(direction);
		grenade->SetOwner(owner);

		isShot = true;
	}
}
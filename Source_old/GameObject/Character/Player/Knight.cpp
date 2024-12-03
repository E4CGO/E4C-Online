#include "Knight.h"

#include "TAKOEngine/Physics/Collider/SphereCollider.h"
#include "TAKOEngine/Effects/EffectManager.h"
#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"

#include "GameObject/Character/Player/KnightState.h"

Knight::Knight() : Player("Data/Model/Character/Knight.glb", 1.0f)
{
	hp = maxHp = 180;
	type = PLAYER_CLASS::KNIGHT;

	m_pmodels[0]->FindNode("1H_Sword_Offhand")->visible = false;
	m_pmodels[0]->FindNode("2H_Sword")->visible = false;
	m_pmodels[0]->FindNode("Badge_Shield")->visible = false;
	m_pmodels[0]->FindNode("Round_Shield")->visible = false;
	m_pmodels[0]->FindNode("Spike_Shield")->visible = false;

	// 攻撃判定
	//attackColliders[AttackCollider::Sword_1] = new SphereCollider(0.3f);
	//attackColliders[AttackCollider::Sword_2] = new SphereCollider(0.3f);
	//attackColliders[AttackCollider::Sword_3] = new SphereCollider(0.3f);
	//attackColliders[AttackCollider::Shield] = new SphereCollider(0.8f);
	EnableAttackColliders(false);

	stateMachine->RegisterState(static_cast<int>(Player::State::AttackNormal), new KnightState::AttackNormalState(this));
	stateMachine->RegisterSubState(static_cast<int>(Player::State::AttackNormal), ATTACK_STATE::ATTACK_1, new KnightState::AttackNormalState_1(this));
	stateMachine->RegisterSubState(static_cast<int>(Player::State::AttackNormal), ATTACK_STATE::ATTACK_2, new KnightState::AttackNormalState_2(this));
	stateMachine->RegisterSubState(static_cast<int>(Player::State::AttackNormal), ATTACK_STATE::ATTACK_3, new KnightState::AttackNormalState_3(this));

	stateMachine->RegisterState(static_cast<int>(Player::State::AttackSpecial), new KnightState::AttackSpecialState(this));
	stateMachine->RegisterSubState(static_cast<int>(Player::State::AttackSpecial), BLOCK_STATE::BLOCK_START, new KnightState::AttackSpecialStateStart(this));
	stateMachine->RegisterSubState(static_cast<int>(Player::State::AttackSpecial), BLOCK_STATE::BLOCKING, new KnightState::AttackSpecialStateBlocking(this));
	stateMachine->RegisterSubState(static_cast<int>(Player::State::AttackSpecial), BLOCK_STATE::BLOCK_ATTACK, new KnightState::AttackSpecialStateAttack(this));
	stateMachine->RegisterSubState(static_cast<int>(Player::State::AttackSpecial), BLOCK_STATE::BLOCK_HIT, new KnightState::AttackSpecialStateHit(this));

	stateMachine->RegisterState(static_cast<int>(Player::State::Skill_1), new KnightState::Skill1State(this));
	mpCost[static_cast<int>(Player::State::Skill_1)] = 20.0f;
	skillTimer[static_cast<int>(Player::State::Skill_1)] = 8.0f;
}

void Knight::AttackCollision()
{
	//Sword
	switch (stateMachine->GetStateIndex())
	{
	case static_cast<int>(State::AttackNormal): // 一般攻撃
		for (int i = AttackCollider::Sword_1; i <= AttackCollider::Sword_3; i++)
		{
			CollisionVsEnemies(attackColliders[i], atk, true, 1.0f, EffectManager::EFFECT_IDX::HIT_SLASH_EFFECT, 0.4f);
		}
		break;
	case static_cast<int>(State::AttackSpecial): // 防御
		if (stateMachine->GetState()->GetSubStateIndex() == BLOCK_STATE::BLOCK_ATTACK) CollisionVsEnemies(attackColliders[AttackCollider::Shield], atk / 5, true, 4.0f);
		break;
	case static_cast<int>(State::Skill_1): // 防御
		CollisionVsEnemies(attackColliders[AttackCollider::Shield], atk, true, 15.0f, EffectManager::EFFECT_IDX::HIT_SHOT_EFFECT, 0.5f);
		break;
	}
}
void Knight::OnDamage(const HitResult& hit, int damage)
{
	if (hurtCoolTime > 0.0f) return;
	hurtCoolTime = 0.5f;
	TPSCamera.Shake(0.2f, 0.5f);

	float vx = hit.position.x - position.x;
	float vz = hit.position.z - position.z;
	Turn(1.0f, vx, vz, DirectX::XMConvertToRadians(360));

	// ブローク
	if (
		stateMachine->GetStateIndex() == static_cast<int>(State::AttackSpecial) &&
		AngleBetweenXMFLOAT3(GetFront(), DirectX::XMFLOAT3{ vx, 0, vz }) <= DirectX::XMConvertToRadians(90)
		)
	{
		stateMachine->ChangeSubState(BLOCK_STATE::BLOCK_HIT);
	}
	else
	{
		hp -= damage;
		if (hp < 0)
		{
			hp = 0;
			stateMachine->ChangeState(static_cast<int>(State::Death));
		}
		else
		{
			stateMachine->ChangeState(static_cast<int>(State::Hurt));
		}
	}
}

void Knight::UpdateColliders()
{
	// 当たり判定
	Player::UpdateColliders();

	// 攻撃判定
	//float Yoffset = 0.2f;
	//attackColliders[AttackCollider::Sword_1]->SetPosition(GetNodePosition("1H_Sword", DirectX::XMFLOAT3{ 0.0f, Yoffset, 0.0f }));
	//Yoffset += attackColliders[AttackCollider::Sword_1]->GetScale().x + attackColliders[AttackCollider::Sword_2]->GetScale().x;
	//attackColliders[AttackCollider::Sword_2]->SetPosition(GetNodePosition("1H_Sword", DirectX::XMFLOAT3{ 0.0f, Yoffset, 0.0f }));
	//Yoffset += attackColliders[AttackCollider::Sword_2]->GetScale().x + attackColliders[AttackCollider::Sword_3]->GetScale().x;
	//attackColliders[AttackCollider::Sword_3]->SetPosition(GetNodePosition("1H_Sword", DirectX::XMFLOAT3{ 0.0f, Yoffset, 0.0f }));

	//attackColliders[AttackCollider::Shield]->SetPosition(GetNodePosition("Rectangle_Shield", DirectX::XMFLOAT3{ 0.0f, 0.0f, -0.1f }));
}

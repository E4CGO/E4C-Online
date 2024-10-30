#include "Barbarian.h"

#include "TAKOEngine/Physics/SphereCollider.h"
#include "TAKOEngine/Effects/EffectManager.h"

#include "GameObject/Character/Player/BarbarianState.h"

Barbarian::Barbarian() : Player("Data/Model/Character/Barbarian.glb", 1.0f)
{
	hp = maxHp = 200;
	type = PLAYER_CLASS::BARBARIAN;

	m_pmodels[0]->FindNode("Barbarian_Round_Shield")->visible = false;
	m_pmodels[0]->FindNode("Mug")->visible = false;
	m_pmodels[0]->FindNode("1H_Axe")->visible = false;
	m_pmodels[0]->FindNode("1H_Axe_Offhand")->visible = false;

	// çUåÇîªíË
	attackColliders[AttackCollider::Axe] = new SphereCollider(0.7f);
	EnableAttackColliders(false);

	stateMachine->RegisterState(static_cast<int>(Player::State::Idle), new BarbarianState::IdleState(this));
	stateMachine->RegisterState(static_cast<int>(Player::State::Waiting), new BarbarianState::WaitState(this));
	stateMachine->SetState(static_cast<int>(State::Waiting));

	// àÍî çUåÇ
	stateMachine->RegisterState(static_cast<int>(Player::State::AttackNormal), new BarbarianState::AttackNormalState(this));
	stateMachine->RegisterSubState(static_cast<int>(Player::State::AttackNormal), ATTACK_STATE::ATTACK_1, new BarbarianState::AttackNormalState_1(this));
	stateMachine->RegisterSubState(static_cast<int>(Player::State::AttackNormal), ATTACK_STATE::ATTACK_2, new BarbarianState::AttackNormalState_2(this));
	stateMachine->RegisterSubState(static_cast<int>(Player::State::AttackNormal), ATTACK_STATE::ATTACK_3, new BarbarianState::AttackNormalState_3(this));

	// ì¡éÍçUåÇ
	stateMachine->RegisterState(static_cast<int>(Player::State::AttackSpecial), new BarbarianState::AttackSpecialState(this));
	stateMachine->RegisterSubState(static_cast<int>(Player::State::AttackSpecial), THROW_STATE::THROW_AIM, new BarbarianState::AttackSpecialStateAim(this));
	stateMachine->RegisterSubState(static_cast<int>(Player::State::AttackSpecial), THROW_STATE::THROW_ATTACK, new BarbarianState::AttackSpecialStateAttack(this));

	// ÉXÉLÉã1
	stateMachine->RegisterState(static_cast<int>(Player::State::Skill_1), new BarbarianState::Skill1State(this));
	stateMachine->RegisterSubState(static_cast<int>(Player::State::Skill_1), SKILL_1_STATE::SKILL_1_START, new BarbarianState::Skill1StateStart(this));
	stateMachine->RegisterSubState(static_cast<int>(Player::State::Skill_1), SKILL_1_STATE::SKILL_1_LOOP, new BarbarianState::Skill1StateLoop(this));
	mpCost[static_cast<int>(Player::State::Skill_1)] = 20.0f;
	skillTimer[static_cast<int>(Player::State::Skill_1)] = 15.0f;
}

DirectX::XMFLOAT3  Barbarian::GetShotPosition()
{
	DirectX::XMFLOAT4X4 transform = m_pmodels[0]->FindNode("2H_Axe")->worldTransform;
	return { transform._41, transform._42, transform._43 };
};

void Barbarian::AttackCollision()
{
	// Axe
	switch (stateMachine->GetStateIndex())
	{
	case static_cast<int>(State::AttackNormal): // àÍî çUåÇ
		CollisionVsEnemies(attackColliders[AttackCollider::Axe], atk, true, 1.0f, EffectManager::EFFECT_IDX::HIT_SLASH_EFFECT, 0.4f);
		break;
	case static_cast<int>(State::Skill_1): // ÉXÉLÉã1

		CollisionVsEnemies(attackColliders[AttackCollider::Axe], atk, true, 1.0f, EffectManager::EFFECT_IDX::HIT_SLASH_EFFECT, 0.4f);
		break;
	}
}

void Barbarian::UpdateColliders()
{
	// ìñÇΩÇËîªíË
	Player::UpdateColliders();

	// çUåÇîªíË
	attackColliders[AttackCollider::Axe]->SetPosition(GetNodePosition("2H_Axe", DirectX::XMFLOAT3{ 0.0f, 0.7f, 0.0f }));
}
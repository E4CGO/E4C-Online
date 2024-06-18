#include "Rogue.h"
#include "RogueState.h"

Rogue::Rogue() : Player("Data/Model/Character/Rogue_Hooded.glb", 1.0f)
{
	hp = maxHp = 150;
	type = PLAYER_CLASS::ROGUE;

	model->FindNode("1H_Crossbow")->visible = false;
	model->FindNode("Knife")->visible = false;
	model->FindNode("Knife_Offhand")->visible = false;
	model->FindNode("Throwable")->visible = false;

	stateMachine->RegisterState(static_cast<int>(Player::State::AttackNormal), new RogueState::AttackNormalState(this));
	stateMachine->RegisterState(static_cast<int>(Player::State::AttackSpecial), new RogueState::AttackSpecialState(this));

	stateMachine->RegisterSubState(static_cast<int>(Player::State::AttackSpecial), CHARGE_STATE::CHARGE_AIM, new RogueState::AttackSpecialStateAim(this));
	stateMachine->RegisterSubState(static_cast<int>(Player::State::AttackSpecial), CHARGE_STATE::CHARGE_SHOOT, new RogueState::AttackSpecialStateShot(this));

	// ƒXƒLƒ‹1
	stateMachine->RegisterState(static_cast<int>(Player::State::Skill_1), new RogueState::Skill1State(this));
	stateMachine->RegisterSubState(static_cast<int>(Player::State::Skill_1), SKILL_1_STATE::SKILL_1_AIM, new RogueState::Skill1StateAim(this));
	stateMachine->RegisterSubState(static_cast<int>(Player::State::Skill_1), SKILL_1_STATE::SKILL_1_THROW, new RogueState::Skill1StateAttack(this));
	mpCost[static_cast<int>(Player::State::Skill_1)] = 30.0f;
	skillTimer[static_cast<int>(Player::State::Skill_1)] = 10.0f;
}

DirectX::XMFLOAT3 Rogue::GetShotPosition()
{
	DirectX::XMFLOAT4X4 transform = model->FindNode("2H_Crossbow")->worldTransform;
	return { transform._41, transform._42, transform._43 };
}
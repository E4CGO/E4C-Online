//! @file PlayerCharacterPattern.cpp
//! @note

#include "PlayerCharacterPattern.h"
#include "GameObject/Character/Player/PlayerCharacter.h"
#include "GameObject/Character/Player/State/PlayerCharacterState.h"
#include "GameObject/Character/Player/Barbarian.h"
#include "GameObject/Character/Player/State/PlayerCharacterSwordState.h"

/**************************************************************************//**
	@brief		性別処理
	@param[in]	chara プレイヤーキャラクター参照ポインタ
	@return		なし
*//***************************************************************************/
void PlayerCharacterPatternGender::Execute(PlayerCharacter* chara)
{
	if (m_isMale) // MALE
	{
		chara->LoadModel("Data/Model/Character/HEAD_BARB.glb");
		chara->LoadModel("Data/Model/Character/BODY_BARB.glb");
	}
	else //FEMALE
	{
		chara->LoadModel("Data/Model/Character/HEAD_MAGE.glb");
		chara->LoadModel("Data/Model/Character/BODY_MAGE.glb");
	}

	StateMachine<PlayerCharacter>* stateMachine = chara->GetStateMachine();
	stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::IDLE), new PlayerCharacterState::IdleState(chara));
	stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::WAITING), new PlayerCharacterState::WaitState(chara));

	stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::ATTACK_NORMAL), nullptr);
	stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::ATTACK_SPECIAL), nullptr);
	stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::SKILL_1), nullptr);
	stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::SKILL_2), nullptr);
	stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::SKILL_4), nullptr);
	stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::SKILL_3), nullptr);
}

/**************************************************************************//**
	@brief		単一モデル実装
	@param[in]	chara プレイヤーキャラクター参照ポインタ
	@return		なし
*//***************************************************************************/
void PlayerCharacterPatternSingleModel::Execute(PlayerCharacter* chara)
{
	if (m_filename != "")
		chara->LoadModel(m_filename.c_str());
}

/**************************************************************************//**
	@brief	右手剣モデル実装、剣モーション付き
	@param[in]	chara プレイヤーキャラクター参照ポインタ
	@return		なし
*//***************************************************************************/
void PlayerCharacterPatternSword::Execute(PlayerCharacter* chara)
{
	PlayerCharacterPatternSingleModel::Execute(chara);

	StateMachine<PlayerCharacter>* stateMachine = chara->GetStateMachine();

	{
		using namespace PlayerCharacterState::Sword;
		stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::ATTACK_NORMAL), new AttackNormalState(chara));
		stateMachine->RegisterSubState(static_cast<int>(PlayerCharacter::STATE::ATTACK_NORMAL), ATTACK_1, new AttackNormalState_1(chara));
		stateMachine->RegisterSubState(static_cast<int>(PlayerCharacter::STATE::ATTACK_NORMAL), ATTACK_2, new AttackNormalState_2(chara));
		stateMachine->RegisterSubState(static_cast<int>(PlayerCharacter::STATE::ATTACK_NORMAL), ATTACK_3, new AttackNormalState_3(chara));
	}
}

/**************************************************************************//**
	@brief	斧モデル実装、斧モーション付き
	@param[in]	chara プレイヤーキャラクター参照ポインタ
	@return		なし
*//***************************************************************************/
void PlayerCharacterPatternAxe::Execute(PlayerCharacter* chara)
{
	PlayerCharacterPatternSingleModel::Execute(chara);

	StateMachine<PlayerCharacter>* stateMachine = chara->GetStateMachine();

	stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::IDLE), new PlayerCharacterState::Axe::IdleState(chara));
	stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::WAITING), new PlayerCharacterState::Axe::WaitState(chara));
	// 一般攻撃
	stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::ATTACK_NORMAL), new PlayerCharacterState::Axe::AttackNormalState(chara));
	stateMachine->RegisterSubState(static_cast<int>(PlayerCharacter::STATE::ATTACK_NORMAL), Barbarian::ATTACK_STATE::ATTACK_1, new PlayerCharacterState::Axe::AttackNormalState_1(chara));
	stateMachine->RegisterSubState(static_cast<int>(PlayerCharacter::STATE::ATTACK_NORMAL), Barbarian::ATTACK_STATE::ATTACK_2, new PlayerCharacterState::Axe::AttackNormalState_2(chara));
	stateMachine->RegisterSubState(static_cast<int>(PlayerCharacter::STATE::ATTACK_NORMAL), Barbarian::ATTACK_STATE::ATTACK_3, new PlayerCharacterState::Axe::AttackNormalState_3(chara));

	// 特殊攻撃
	stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::ATTACK_SPECIAL), new PlayerCharacterState::Axe::AttackSpecialState(chara));
	stateMachine->RegisterSubState(static_cast<int>(PlayerCharacter::STATE::ATTACK_SPECIAL), Barbarian::THROW_STATE::THROW_AIM, new PlayerCharacterState::Axe::AttackSpecialStateAim(chara));
	stateMachine->RegisterSubState(static_cast<int>(PlayerCharacter::STATE::ATTACK_SPECIAL), Barbarian::THROW_STATE::THROW_ATTACK, new PlayerCharacterState::Axe::AttackSpecialStateAttack(chara));

	// スキル1
	stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::SKILL_1), new PlayerCharacterState::Axe::Skill1State(chara));
	stateMachine->RegisterSubState(static_cast<int>(PlayerCharacter::STATE::SKILL_1), Barbarian::SKILL_1_STATE::SKILL_1_START, new PlayerCharacterState::Axe::Skill1StateStart(chara));
	stateMachine->RegisterSubState(static_cast<int>(PlayerCharacter::STATE::SKILL_1), Barbarian::SKILL_1_STATE::SKILL_1_LOOP, new PlayerCharacterState::Axe::Skill1StateLoop(chara));

}

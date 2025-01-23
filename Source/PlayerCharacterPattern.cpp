//! @file PlayerCharacterPattern.cpp
//! @note

#include "PlayerCharacterPattern.h"
#include "GameObject/Character/Player/PlayerCharacter.h"
#include "GameObject/Character/Player/State/PlayerCharacterState.h"
#include "GameObject/Character/Player/State/PlayerCharacterSwordState.h"
#include "GameObject/Character/Player/State/PlayerCharacterShieldState.h"
#include "GameObject/Character/Player/State/PlayerCharacterRodState.h"

/**************************************************************************//**
	@brief		性別処理
	@param[in]	chara プレイヤーキャラクター参照ポインタ
	@return		なし
*//***************************************************************************/
void PlayerCharacterPatternGender::Execute(PlayerCharacter* chara)
{
	if (m_isMale) // MALE
	{
		if (T_GRAPHICS.isDX12Active)
		{
			chara->LoadModel("Data/Model/Character/PlayerModels/MDL_PLAYER_BODY_ANIMATION.glb", 1.0f, ModelObject::RENDER_MODE::DX12);
		}
		if (T_GRAPHICS.isDX11Active)
		{
			chara->LoadModel("Data/Model/Character/PlayerModels/MDL_PLAYER_BODY_ANIMATION.glb", 1.0f, ModelObject::RENDER_MODE::DX11);
		}

		PLAYER_CHARACTER_DATA.SetMalePatterns();
	}
	else //FEMALE
	{
		if (T_GRAPHICS.isDX12Active)
		{
			chara->LoadModel("Data/Model/Character/PlayerModels/MDL_PLAYER_F_BODY_ANIMATION.glb", 1.0f, ModelObject::RENDER_MODE::DX12);
		}
		if (T_GRAPHICS.isDX11Active)
		{
			chara->LoadModel("Data/Model/Character/PlayerModels/MDL_PLAYER_F_BODY_ANIMATION.glb", 1.0f, ModelObject::RENDER_MODE::DX11);
		}

		PLAYER_CHARACTER_DATA.SetFemalePatterns();
	}

	StateMachine<PlayerCharacter>* stateMachine = chara->GetStateMachine();

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
	{
		if (T_GRAPHICS.isDX12Active)
		{
			chara->LoadModel(m_filename.c_str(), 1.0f, ModelObject::RENDER_MODE::DX12);
		}
		if (T_GRAPHICS.isDX11Active)
		{
			chara->LoadModel(m_filename.c_str(), 1.0f, ModelObject::RENDER_MODE::DX11);
		}
	}
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
		stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::WAITING), new WaitState(chara));
		stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::IDLE), new IdleState(chara));
		stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::MOVE), new MoveState(chara));

		stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::ATTACK_NORMAL), new AttackNormalState(chara));
		stateMachine->RegisterSubState(static_cast<int>(PlayerCharacter::STATE::ATTACK_NORMAL), ATTACK_1, new AttackNormalState_1(chara));
		stateMachine->RegisterSubState(static_cast<int>(PlayerCharacter::STATE::ATTACK_NORMAL), ATTACK_2, new AttackNormalState_2(chara));
		stateMachine->RegisterSubState(static_cast<int>(PlayerCharacter::STATE::ATTACK_NORMAL), ATTACK_3, new AttackNormalState_3(chara));

		stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::SKILL_1), new Skill1State(chara));
		stateMachine->RegisterSubState(static_cast<int>(PlayerCharacter::STATE::SKILL_1), ATTACK_START, new Skill1StateStart(chara));
		stateMachine->RegisterSubState(static_cast<int>(PlayerCharacter::STATE::SKILL_1), ATTACK_CONTINUE, new Skill1ContinueStart(chara));
		stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::SKILL_2), new Skill2State(chara));
	}
	chara->SetEnergyType(PlayerCharacter::ENERGY_TYPE::STAMINA);
}

/**************************************************************************//**
	@brief	左盾モデル実装、盾モーション付き
	@param[in]	chara プレイヤーキャラクター参照ポインタ
	@return		なし
*//***************************************************************************/
void PlayerCharacterPatternShield::Execute(PlayerCharacter* chara)
{
	PlayerCharacterPatternSingleModel::Execute(chara);

	StateMachine<PlayerCharacter>* stateMachine = chara->GetStateMachine();

	{
		using namespace PlayerCharacterState::Shield;
		stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::ATTACK_SPECIAL), new AttackSpecialState(chara));
	}
}

void PlayerCharacterPatternRod::Execute(PlayerCharacter* chara)
{
	PlayerCharacterPatternSingleModel::Execute(chara);

	StateMachine<PlayerCharacter>* stateMachine = chara->GetStateMachine();

	{
		using namespace PlayerCharacterState::Rod;
		stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::WAITING), new WaitState(chara));
		stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::IDLE), new IdleState(chara));
		stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::MOVE), new MoveState(chara));

		stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::ATTACK_NORMAL), new AttackNormalState(chara));
		stateMachine->RegisterSubState(static_cast<int>(PlayerCharacter::STATE::ATTACK_NORMAL), ATTACK_1, new AttackNormalState_1(chara));
		stateMachine->RegisterSubState(static_cast<int>(PlayerCharacter::STATE::ATTACK_NORMAL), ATTACK_2, new AttackNormalState_2(chara));
		stateMachine->RegisterSubState(static_cast<int>(PlayerCharacter::STATE::ATTACK_NORMAL), ATTACK_3, new AttackNormalState_3(chara));

		stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::ATTACK_SPECIAL), new AttackSpecialState(chara));
		stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::SKILL_1), new Skill1State(chara));
		stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::SKILL_2), new Skill2State(chara));
	}

	chara->SetEnergyType(PlayerCharacter::ENERGY_TYPE::MANA);
}
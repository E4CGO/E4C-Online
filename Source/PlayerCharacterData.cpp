//! @file PlayerCharacterData.cpp
//! @note

#include "PlayerCharacterData.h"
#include "GameObject/Character/Player/PlayerCharacter.h"

/**************************************************************************//**
	@brief		コンストラクタ
	@note		パターン処理登録
				TODO::JSONにするべき
*//***************************************************************************/
PlayerCharacterData::PlayerCharacterData()
{
	m_pAppearancePatternResource[PATTERN_RESOURCE::NONE] = new PlayerCharacterPatternNone;
	m_pAppearancePatternResource[PATTERN_RESOURCE::MALE] = new PlayerCharacterPatternGender(true);
	m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE] = new PlayerCharacterPatternGender(false);
	m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_SWORD_1] = new PlayerCharacterPatternSword("Data/Model/Character/PlayerModels/PlayerMale/Weapons/MDLANM_PLAYER_SWORD.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_SWORD_2] = new PlayerCharacterPatternSword("Data/Model/Character/PlayerModels/PlayerMale/Weapons/MDLANM_PLAYER_SWORD2.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_SWORD_3] = new PlayerCharacterPatternSword("Data/Model/Character/PlayerModels/PlayerMale/Weapons/MDLANM_PLAYER_SWORD3.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_ROD_1] = new PlayerCharacterPatternRod("Data/Model/Character/PlayerModels/PlayerMale/Weapons/MDLANM_PLAYER_ROD.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_ROD_2] = new PlayerCharacterPatternRod("Data/Model/Character/PlayerModels/PlayerMale/Weapons/MDLANM_PLAYER_ROD2.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_HAIR_1] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerMale/Hair/MDLANM_PLAYER_HAIR.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_HAIR_2] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerMale/Hair/MDLANM_PLAYER_HAIR2.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_HAIR_3] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerMale/Hair/MDLANM_PLAYER_HAIR3.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_TOP_1] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerMale/Costume1/MDLANM_PLAYER_TOP.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_TOP_2] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerMale/Costume2/MDLANM_PLAYER_TOP.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_TOP_3] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerMale/Costume3/MDLANM_PLAYER_TOP.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_ARM_1] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerMale/Costume1/MDLANM_PLAYER_ARM.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_ARM_2] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerMale/Costume2/MDLANM_PLAYER_ARM.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_ARM_3] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerMale/Costume3/MDLANM_PLAYER_ARM.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_BOTTOM_1] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerMale/Costume1/MDLANM_PLAYER_BOTTOM.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_BOTTOM_2] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerMale/Costume2/MDLANM_PLAYER_BOTTOM.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_BOTTOM_3] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerMale/Costume3/MDLANM_PLAYER_BOTTOM.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_SHOES_1] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerMale/Costume1/MDLANM_PLAYER_SHOES.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_SHOES_2] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerMale/Costume2/MDLANM_PLAYER_SHOES.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_SHOES_3] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerMale/Costume3/MDLANM_PLAYER_SHOES.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_SHIELD_1] = new PlayerCharacterPatternShield("Data/Model/Character/PlayerModels/PlayerMale/Weapons/MDLANM_PLAYER_SHIELD1.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_SHIELD_2] = new PlayerCharacterPatternShield("Data/Model/Character/PlayerModels/PlayerMale/Weapons/MDLANM_PLAYER_SHIELD2.glb");

	m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_SWORD_1] = new PlayerCharacterPatternSword("Data/Model/Character/PlayerModels/PlayerFemale/Weapons/MDLANM_PLAYER_F_SWORD.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_SWORD_2] = new PlayerCharacterPatternSword("Data/Model/Character/PlayerModels/PlayerFemale/Weapons/MDLANM_PLAYER_F_SWORD2.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_SWORD_3] = new PlayerCharacterPatternSword("Data/Model/Character/PlayerModels/PlayerFemale/Weapons/MDLANM_PLAYER_F_SWORD3.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_ROD_1] = new PlayerCharacterPatternRod("Data/Model/Character/PlayerModels/PlayerFemale/Weapons/MDLANM_PLAYER_F_ROD.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_ROD_2] = new PlayerCharacterPatternRod("Data/Model/Character/PlayerModels/PlayerFemale/Weapons/MDLANM_PLAYER_F_ROD2.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_HAIR_1] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerFemale/Hair/MDLANM_PLAYER_F_HAIR.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_HAIR_2] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerFemale/Hair/MDLANM_PLAYER_F_HAIR2.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_HAIR_3] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerFemale/Hair/MDLANM_PLAYER_F_HAIR3.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_HAIR_4] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerFemale/Hair/MDLANM_PLAYER_F_HAIR4.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_TOP_1] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerFemale/Costume1/MDLANM_PLAYER_F_TOP.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_TOP_2] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerFemale/Costume2/MDLANM_PLAYER_F_TOP.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_TOP_3] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerFemale/Costume3/MDLANM_PLAYER_F_TOP.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_ARM_1] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerFemale/MDLANM_PLAYER_F_ARM.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_BOTTOM_1] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerFemale/Costume1/MDLANM_PLAYER_F_BOTTOM.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_BOTTOM_2] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerFemale/Costume2/MDLANM_PLAYER_F_BOTTOM.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_BOTTOM_3] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerFemale/Costume3/MDLANM_PLAYER_F_BOTTOM.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_SHOES_1] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerFemale/Costume1/MDLANM_PLAYER_F_SHOES.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_SHOES_2] = new PlayerCharacterPatternSingleModel("Data/Model/Character/PlayerModels/PlayerFemale/Costume2/MDLANM_PLAYER_F_SHOES.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_SHIELD_1] = new PlayerCharacterPatternShield("Data/Model/Character/PlayerModels/PlayerFemale/Weapons/MDLANM_PLAYER_F_SHIELD1.glb");
	m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_SHIELD_2] = new PlayerCharacterPatternShield("Data/Model/Character/PlayerModels/PlayerFemale/Weapons/MDLANM_PLAYER_F_SHIELD2.glb");

	ClearAppearancePatterns();

	m_pAppearancePatterns[APPEARANCE_PATTERN::GENDER].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::GENDER].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE]);
}

/**************************************************************************//**
	@brief		プレイヤーキャラクターに外見設定を実装する
	@param[in]	chara			プレイヤー参照ポインタ
	@param[in]	appearance_idx	外見インデックス
	@param[in]	pattern_idx		外見パターン
	@return		なし
*//***************************************************************************/
void PlayerCharacterData::LoadAppearance(PlayerCharacter* chara, uint8_t appearance_idx, uint8_t pattern_idx)
{
	if (m_pAppearancePatterns.find(appearance_idx) == m_pAppearancePatterns.end()) return;
	if (m_pAppearancePatterns[appearance_idx].size() <= pattern_idx) return;
	m_pAppearancePatterns[appearance_idx][pattern_idx]->Execute(chara);
}

/**************************************************************************//**
	@brief		外見パターンをクリア
	@param[in]	なし
	@return		なし
*//***************************************************************************/
void PlayerCharacterData::ClearAppearancePatterns()
{
	for (auto& pair : m_pAppearancePatterns)
	{
		//for (PlayerCharacterPattern* ptr : pair.second) {
		//	delete ptr;			// vector 内のポインタを削除
		//}
		pair.second.clear();	// vector のポインタをクリア
	}
	m_pAppearancePatterns.clear();
}

void PlayerCharacterData::SetMalePatterns()
{
	ClearAppearancePatterns();

	m_pAppearancePatterns[APPEARANCE_PATTERN::GENDER].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::GENDER].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE]);

	m_pAppearancePatterns[APPEARANCE_PATTERN::RIGHT_HAND_EQUIPMENT].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_SWORD_1]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::RIGHT_HAND_EQUIPMENT].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_SWORD_2]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::RIGHT_HAND_EQUIPMENT].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_SWORD_3]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::RIGHT_HAND_EQUIPMENT].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_ROD_1]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::RIGHT_HAND_EQUIPMENT].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_ROD_2]);

	m_pAppearancePatterns[APPEARANCE_PATTERN::HAIR].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_HAIR_1]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::HAIR].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_HAIR_2]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::HAIR].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_HAIR_3]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::HAIR].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::NONE]);

	m_pAppearancePatterns[APPEARANCE_PATTERN::TOP].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_TOP_1]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::TOP].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_TOP_2]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::TOP].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_TOP_3]);

	m_pAppearancePatterns[APPEARANCE_PATTERN::ARM_GEAR].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_ARM_1]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::ARM_GEAR].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_ARM_2]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::ARM_GEAR].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_ARM_3]);

	m_pAppearancePatterns[APPEARANCE_PATTERN::BOTTOM].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_BOTTOM_1]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::BOTTOM].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_BOTTOM_2]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::BOTTOM].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_BOTTOM_3]);

	m_pAppearancePatterns[APPEARANCE_PATTERN::EYE_COLOR].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_SHOES_1]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::EYE_COLOR].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_SHOES_2]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::EYE_COLOR].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_SHOES_3]);

	m_pAppearancePatterns[APPEARANCE_PATTERN::LEFT_HAND_EQUIPMENT].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_SHIELD_1]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::LEFT_HAND_EQUIPMENT].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE_SHIELD_2]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::LEFT_HAND_EQUIPMENT].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::NONE]);
}

void PlayerCharacterData::SetFemalePatterns()
{
	ClearAppearancePatterns();

	m_pAppearancePatterns[APPEARANCE_PATTERN::GENDER].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::MALE]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::GENDER].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE]);

	m_pAppearancePatterns[APPEARANCE_PATTERN::RIGHT_HAND_EQUIPMENT].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_SWORD_1]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::RIGHT_HAND_EQUIPMENT].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_SWORD_2]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::RIGHT_HAND_EQUIPMENT].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_SWORD_3]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::RIGHT_HAND_EQUIPMENT].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_ROD_1]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::RIGHT_HAND_EQUIPMENT].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_ROD_2]);

	m_pAppearancePatterns[APPEARANCE_PATTERN::HAIR].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_HAIR_1]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::HAIR].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_HAIR_2]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::HAIR].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_HAIR_3]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::HAIR].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_HAIR_4]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::HAIR].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::NONE]);

	m_pAppearancePatterns[APPEARANCE_PATTERN::TOP].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_TOP_1]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::TOP].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_TOP_2]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::TOP].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_TOP_3]);

	m_pAppearancePatterns[APPEARANCE_PATTERN::ARM_GEAR].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_ARM_1]);

	m_pAppearancePatterns[APPEARANCE_PATTERN::BOTTOM].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_BOTTOM_1]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::BOTTOM].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_BOTTOM_2]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::BOTTOM].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_BOTTOM_3]);

	m_pAppearancePatterns[APPEARANCE_PATTERN::EYE_COLOR].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_SHOES_1]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::EYE_COLOR].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_SHOES_2]);
	
	m_pAppearancePatterns[APPEARANCE_PATTERN::LEFT_HAND_EQUIPMENT].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_SHIELD_1]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::LEFT_HAND_EQUIPMENT].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::FEMALE_SHIELD_2]);
	m_pAppearancePatterns[APPEARANCE_PATTERN::LEFT_HAND_EQUIPMENT].push_back(m_pAppearancePatternResource[PATTERN_RESOURCE::NONE]);
}

/**************************************************************************//**
	@brief		コンストラクタ
	@note		処理したパターンを解放
*//***************************************************************************/
PlayerCharacterData::~PlayerCharacterData()
{
	ClearAppearancePatterns();
	for (std::pair<uint8_t, PlayerCharacterPattern*> pair : m_pAppearancePatternResource)
	{
		delete pair.second;
	}
	m_pAppearancePatternResource.clear();
}
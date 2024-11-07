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
	ClearAppearancePatterns();
	m_pAppearancePatterns[APPEARANCE_PATTERN::GENDER].push_back(new PlayerCharacterPatternGender(true));
	m_pAppearancePatterns[APPEARANCE_PATTERN::GENDER].push_back(new PlayerCharacterPatternGender(false));

	m_pAppearancePatterns[APPEARANCE_PATTERN::RIGHT_HAND_EQUIPMENT].push_back(new PlayerCharacterPatternNone);
	m_pAppearancePatterns[APPEARANCE_PATTERN::RIGHT_HAND_EQUIPMENT].push_back(new PlayerCharacterPatternSword("Data/Model/Character/WEAPON_KNIGHT.glb"));
	//m_pAppearancePatterns[APPEARANCE_PATTERN::RIGHT_HAND_EQUIPMENT].push_back(new PlayerCharacterPatternAxe("Data/Model/Character/WEAPON_BARB.glb"));
	m_pAppearancePatterns[APPEARANCE_PATTERN::RIGHT_HAND_EQUIPMENT].push_back(new PlayerCharacterPatternSingleModel("Data/Model/Character/WEAPON_MAGE.glb"));
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
		for (PlayerCharacterPattern* ptr : pair.second) {
			delete ptr;			// vector 内のポインタを削除
		}
		pair.second.clear();	// vector のポインタをクリア
	}
	m_pAppearancePatterns.clear();
}

/**************************************************************************//**
	@brief		コンストラクタ
	@note		処理したパターンを解放
*//***************************************************************************/
PlayerCharacterData::~PlayerCharacterData()
{
	ClearAppearancePatterns();
}
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

	m_pappearancePatterns[APPEARANCE_PATTERN::GENDER].push_back(new PlayerCharacterPatternGender(true));
	m_pappearancePatterns[APPEARANCE_PATTERN::GENDER].push_back(new PlayerCharacterPatternGender(false));
	m_pappearancePatterns[APPEARANCE_PATTERN::RIGHT_HAND_EQUIPMENT].push_back(new PlayerCharacterPatternSingleModel("Data/Model/Character/WEAPON_BARB.glb"));
	m_pappearancePatterns[APPEARANCE_PATTERN::RIGHT_HAND_EQUIPMENT].push_back(new PlayerCharacterPatternSingleModel("Data/Model/Character/WEAPON_MAGE.glb"));
}

/**************************************************************************//**
	@brief		プレイヤーキャラクターに外見設定を実装する
	@param[in]	player			プレイヤー参照ポインタ
	@param[in]	appearance_idx	外見インデックス
	@param[in]	pattern_idx		外見パターン
	@retrun		なし
*//***************************************************************************/
void PlayerCharacterData::LoadAppearance(Character* chara, uint8_t appearance_idx, uint8_t pattern_idx)
{
	if (m_pappearancePatterns.find(appearance_idx) == m_pappearancePatterns.end()) return;
	if (m_pappearancePatterns[appearance_idx].size() <= pattern_idx) return;
	m_pappearancePatterns[appearance_idx][pattern_idx]->Execute(chara);
}

/**************************************************************************//**
	@brief		// 外見パターンをクリア
	@param[in]	なし
	@retrun		なし
*//***************************************************************************/
void PlayerCharacterData::ClearAppearancePatterns()
{
	for (auto& pair : m_pappearancePatterns)
	{
		for (PlayerCharacterPattern* ptr : pair.second) {
			delete ptr;			// vector 内のポインタを削除
		}
		pair.second.clear();	// vector のポインタをクリア
	}
	m_pappearancePatterns.clear();
}

/**************************************************************************//**
	@brief		コンストラクタ
	@note		処理したパターンを解放
*//***************************************************************************/
PlayerCharacterData::~PlayerCharacterData()
{
	ClearAppearancePatterns();
}
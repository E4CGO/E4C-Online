//! @file PlayerCharacterData.cpp
//! @note 

#include "PlayerCharacterData.h"
#include "GameObject/Character/Player/PlayerCharacter.h"

/**************************************************************************//**
	@brief		�R���X�g���N�^
	@note		�p�^�[�������o�^
				TODO::JSON�ɂ���ׂ�
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
	@brief		�v���C���[�L�����N�^�[�ɊO���ݒ����������
	@param[in]	player			�v���C���[�Q�ƃ|�C���^
	@param[in]	appearance_idx	�O���C���f�b�N�X
	@param[in]	pattern_idx		�O���p�^�[��
	@retrun		�Ȃ�
*//***************************************************************************/
void PlayerCharacterData::LoadAppearance(PlayerCharacter* player, uint8_t appearance_idx, uint8_t pattern_idx)
{
	if (m_pappearancePatterns.find(appearance_idx) == m_pappearancePatterns.end()) return;
	if (m_pappearancePatterns[appearance_idx].size() <= pattern_idx) return;
	m_pappearancePatterns[appearance_idx][pattern_idx]->Execute(player);
}


/**************************************************************************//**
	@brief		// �O���p�^�[�����N���A
	@param[in]	�Ȃ�
	@retrun		�Ȃ�
*//***************************************************************************/
void PlayerCharacterData::ClearAppearancePatterns()
{
	for (auto& pair : m_pappearancePatterns)
	{
		for (PlayerCharacterPattern* ptr : pair.second) {
			delete ptr;			// vector ���̃|�C���^���폜
		}
		pair.second.clear();	// vector �̃|�C���^���N���A
	}
	m_pappearancePatterns.clear();
}

/**************************************************************************//**
	@brief		�R���X�g���N�^
	@note		���������p�^�[�������
*//***************************************************************************/
PlayerCharacterData::~PlayerCharacterData()
{
	ClearAppearancePatterns();
}

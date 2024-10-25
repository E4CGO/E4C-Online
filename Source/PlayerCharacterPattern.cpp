//! @file PlayerCharacterPattern.cpp
//! @note 

#include "PlayerCharacterPattern.h"
#include "GameObject/Character/Player/PlayerCharacter.h"

/**************************************************************************//**
 	@brief		���ʏ���
	@param[in]	player	�v���C���[�Q�ƃ|�C���^
	@param[in]	gender	����	0:�j��	1:����
	@return		�Ȃ�
*//***************************************************************************/
void PlayerCharacterPatternGender::Execute(PlayerCharacter* player)
{
	player->LoadModel("Data/Model/Character/BarbarianAnim.glb");
	if (m_isMale) // MALE
	{
		player->LoadModel("Data/Model/Character/HEAD_BARB.glb");
		player->LoadModel("Data/Model/Character/BODY_BARB.glb");
	}
	else //FEMALE
	{	
		player->LoadModel("Data/Model/Character/HEAD_MAGE.glb");
		player->LoadModel("Data/Model/Character/BODY_MAGE.glb");
	}
}

/**************************************************************************//**
 	@brief		�P�ꃂ�f������
	@param[in]	player	�v���C���[�Q�ƃ|�C���^
	@return		�Ȃ�
*//***************************************************************************/
void PlayerCharacterPatternSingleModel::Execute(PlayerCharacter* player)
{
	player->LoadModel(m_filename.c_str());
}

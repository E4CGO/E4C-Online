//! @file PlayerCharacterPattern.cpp
//! @note

#include "PlayerCharacterPattern.h"
#include "GameObject/Character/Player/PlayerCharacter.h"

/**************************************************************************//**
 	@brief		性別処理
	@param[in]	player	プレイヤー参照ポインタ
	@param[in]	gender	性別	0:男性	1:女性
	@return		なし
*//***************************************************************************/
void PlayerCharacterPatternGender::Execute(Character* chara)
{
	chara->LoadModel("Data/Model/Character/BarbarianAnim.glb");
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
}

/**************************************************************************//**
 	@brief		単一モデル実装
	@param[in]	player	プレイヤー参照ポインタ
	@return		なし
*//***************************************************************************/
void PlayerCharacterPatternSingleModel::Execute(Character* chara)
{
	chara->LoadModel(m_filename.c_str());
}
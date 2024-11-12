//! @file Teleporter.cpp
//! @note

#include "Teleporter.h"

#include "TAKOEngine/Tool/XMFLOAT.h"
#include "GameObject/Character/Player/PlayerCharacterManager.h"
#include "Scene/Stage/StageManager.h"
/**************************************************************************//**
 	@brief		コンストラクタ
	@param[in]	stage	ステージ参照ポインタ
	@return	なし
*//***************************************************************************/
Teleporter::Teleporter(Stage* stage) : m_pStage(stage), ModelObject("Data/Model/Cube/testCubes.glb")
{
	m_timer = 0.0f;
}

/**************************************************************************//**
 	@brief		更新処理
	@param[in]	elapsedTime	経過時間
	@return		なし
*//***************************************************************************/
void Teleporter::Update(float elapsedTime)
{
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
	if (player != nullptr && XMFLOAT3LengthSq(player->GetPosition() - position) < m_radius * m_radius)
	{
		m_timer += elapsedTime;
		if (m_timer >= m_portalTime)
		{
			StageManager::Instance().ChangeStage(m_pStage);
		}
	}
	else
	{
		m_timer = 0.0f;
	}
	ModelObject::Update(elapsedTime);
}
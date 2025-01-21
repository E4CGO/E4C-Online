﻿//! @file Teleporter.cpp
//! @note

#include "StairToNextFloor.h"

#include "TAKOEngine/Tool/XMFLOAT.h"
#include "GameObject/Character/Player/PlayerCharacterManager.h"
#include "Scene/Stage/StageManager.h"
#include "TAKOEngine/GUI/UIManager.h"

#include "Scene/GameLoop/SceneGame/SceneGame_E4C.h"
#include "Scene/SceneManager.h"

StairToNextFloor::StairToNextFloor(Stage* stage, Online::OnlineController* onlineController)
	: Teleporter(stage, onlineController)
{
	m_cylinderRenderer = std::make_unique<CylinderRenderer>(T_GRAPHICS.GetDeviceDX12());

	m_interractionDistance = 12.0f;
}

/**************************************************************************//**
	@brief		更新処理
	@param[in]	elapsedTime	経過時間
	@return		なし
*//***************************************************************************/
void StairToNextFloor::Update(float elapsedTime)
{
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
	const float radius = m_interractionDistance * scale.x;
	if (player != nullptr && XMFLOAT3LengthSq(player->GetPosition() - (position - DirectX::XMFLOAT3{ 0.0f, 0.5f * scale.y, 0.0f })) < radius * radius)
	{
		m_timer += elapsedTime;
		if (m_timer >= m_portalTime)
		{
			GoToNextFloor();
		}
	}
	else
	{
		m_timer = 0.0f;
	}

	ModelObject::Update(elapsedTime);

	DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
	for (int i = 0; i < m_mesh.vertices.size(); i++)
	{
		ModelResource::Vertex& vertice = m_mesh.vertices[i];
		DirectX::XMStoreFloat3(&vertice.position, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&m_defaultVertices[i].position), Transform));
	}
}

/**************************************************************************//**
	@brief		描画
	@param[in]	rc
*//***************************************************************************/
void StairToNextFloor::RenderDX12(const RenderContextDX12& rc)
{
#ifdef _DEBUG
	m_cylinderRenderer->SetCylinder(position,
		{ 0.0f, 1.0f, 0.0f },
		m_interractionDistance, 1.0f,
		{ 1.0f, 1.0f, 1.0f, 1.0f });
	m_cylinderRenderer->Render(rc);

#endif
}

/**************************************************************************//**
	@brief		次の階へ進む処理
	@param[in]	なし
	@return		なし
*//***************************************************************************/
void StairToNextFloor::GoToNextFloor()
{
	// currentFloor(現在の階)を一つ進める
	DUNGEONDATA.GoToNextFloor();

	StageManager::Instance().ChangeStage(m_pStage);
	m_pStage = nullptr;
	m_timer = -10.0f;
}

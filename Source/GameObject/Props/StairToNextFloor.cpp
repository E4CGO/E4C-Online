//! @file Teleporter.cpp
//! @note

#include "StairToNextFloor.h"

#include "TAKOEngine/Tool/XMFLOAT.h"
#include "GameObject/Character/Player/PlayerCharacterManager.h"
#include "Scene/Stage/StageManager.h"
#include "TAKOEngine/GUI/UIManager.h"

#include "Scene/GameLoop/SceneGame/SceneGame_E4C.h"
#include "Scene/SceneManager.h"

/**************************************************************************//**
	@brief		更新処理
	@param[in]	elapsedTime	経過時間
	@return		なし
*//***************************************************************************/
void StairToNextFloor::Update(float elapsedTime)
{
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
	const float radius = 0.5f * scale.x;
	if (player != nullptr && XMFLOAT3LengthSq(player->GetPosition() - position) < radius * radius)
	{
		m_timer += elapsedTime;
		if (m_timer >= m_portalTime)
		{
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame_E4C));
		}
	}

	ModelObject::Update(elapsedTime);
	DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
	for (int i = 0; i < m_mesh.vertices.size(); i++)
	{
		ModelResource::Vertex& vertice = m_mesh.vertices[i];
		DirectX::XMStoreFloat3(&vertice.position, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&m_defaultVertices[i].position), Transform));
	}
}

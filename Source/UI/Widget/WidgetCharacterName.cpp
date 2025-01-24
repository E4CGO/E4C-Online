//! @file WidgetCharacterName.cpp
//! @note 

#include "WidgetCharacterName.h"
#include "GameObject/Character/Player/PlayerCharacterManager.h"

#include "TAKOEngine/Tool/Encode.h"

void WidgetCharacterName::Render(const RenderContext& rc)
{
}

/**************************************************************************//**
 	@brief		DX12描画処理
	@param[in]	rc レンダーコンテンツ
*//***************************************************************************/
void WidgetCharacterName::RenderDX12(const RenderContextDX12& rc)
{
	// 変換行列
	const DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetCamera()->GetView());
	const DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetCamera()->GetProjection());
	const DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	// カメラ
	const DirectX::XMFLOAT3 front = CameraManager::Instance().GetCamera()->GetFront();
	const DirectX::XMFLOAT3 eye = CameraManager::Instance().GetCamera()->GetEye();

	for (PlayerCharacter* player : PlayerCharacterManager::Instance().GetAll())
	{
		DirectX::XMFLOAT3 namePos = player->GetPosition()  + DirectX::XMFLOAT3{0, 2.0f, 0};
		float dot = XMFLOAT3Dot(front, namePos - eye);
		if (dot < 0.0f) continue;

		DirectX::XMFLOAT3 pos = T_GRAPHICS.GetScreenPosition(namePos, T_GRAPHICS.GetViewPort(), View, Projection, World);

		T_TEXT.RenderDX12(
			FONT_ID::HGpop,
			Encode::string_to_wstring(player->GetName()),
			pos.x, pos.y,
			1.0, 1.0f, 1.0f, 1.0f,
			0.0f,
			FONT_ALIGN::BOTTOM
		);
	}
}

//! @file CapsuleCollider.cpp
//! @note カプセルコリダー

#include "CapsuleCollider.h"

//#include <DirectXCollision.h>
#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "Source/Map/MapTileManager.h"

/**************************************************************************//**
		@brief		コンストラクタ
		@param[in]	なし
		@return		なし
*//***************************************************************************/
CapsuleCollider::CapsuleCollider()
{
	type = COLLIDER_TYPE::CAPSULE;

	m_sphere[0] = std::make_unique<SphereRenderer>(T_GRAPHICS.GetDeviceDX12());
	m_sphere[1] = std::make_unique<SphereRenderer>(T_GRAPHICS.GetDeviceDX12());

	m_cylinder = std::make_unique<CylinderRenderer>(T_GRAPHICS.GetDeviceDX12());
}

/**************************************************************************//**
		@brief		Capsule用パラメータセット
		@param[in]	Capsule capsule
		@return		なし
*//***************************************************************************/
void CapsuleCollider::SetParam(Capsule capsule)
{
	position = capsule.position;
	direction = capsule.direction;
	radius = capsule.radius;
	length = capsule.length;
}

/**************************************************************************//**
		@brief		デバッグ描画
		@param[in]	color
		@return		なし
*//***************************************************************************/
void CapsuleCollider::DrawDebugPrimitive(DirectX::XMFLOAT4 color)
{
	if (!enable) return;

	if (T_GRAPHICS.isDX11Active) T_GRAPHICS.GetDebugRenderer()->DrawCapsule(position, radius, length, color);
	else
	{
		// レンダーコンテキスト設定
		RenderContextDX12 rc;
		rc.d3d_command_list = T_GRAPHICS.GetFrameBufferManager()->GetCommandList();

		// 円柱描画
		m_cylinder->SetCylinder(position, radius, length, color);
		m_cylinder->Render(rc);

		// 下の球描画
		m_sphere[0]->SetSphere(position, radius, color);
		m_sphere[0]->Render(rc);

		DirectX::XMFLOAT3 upPos = position;
		upPos.y += length;

		// 上の球描画
		m_sphere[1]->SetSphere(upPos, radius, color);
		m_sphere[1]->Render(rc);
	}
}

/**************************************************************************//**
		@brief		マップとの当たり判定
		@param[in]	なし
		@return		衝突判定
*//***************************************************************************/
bool CapsuleCollider::CollisionVsMap()
{
	Capsule capsule;
	capsule.position = position;
	capsule.direction = direction;
	capsule.radius = radius;
	capsule.length = length;

	if (MAPTILES.IntersectCapsuleVsMap(capsule))
	{
		position = capsule.position;
		return true;
	}
	return false;
};
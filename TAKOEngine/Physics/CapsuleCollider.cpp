//! @file CapsuleCollider.cpp
//! @note カプセルコリダー

#include "CapsuleCollider.h"
#include "SphereCollider.h"

//#include <DirectXCollision.h>
#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "Source/Map/MapTileManager.h"

/**************************************************************************//**
		@brief		コンストラクタ
		@param[in]	_objType
					_transform
		@return		なし
*//***************************************************************************/
CapsuleCollider::CapsuleCollider(uint16_t _objType, DirectX::XMFLOAT4X4* _transform) : Collider(_objType, _transform)
{
	m_Wdirection = { 0.0f, 1.0f, 0.0f };
	m_Ldirection = { 0.0f, 1.0f, 0.0f };
	m_radius = 1.0f;
	m_length = 1.0f;
	m_shapeType = COLLIDER_TYPE::CAPSULE;

	m_sphere[0] = std::make_unique<SphereRenderer>(T_GRAPHICS.GetDeviceDX12());
	m_sphere[1] = std::make_unique<SphereRenderer>(T_GRAPHICS.GetDeviceDX12());

	m_cylinder = std::make_unique<CylinderRenderer>(T_GRAPHICS.GetDeviceDX12());
}

/**************************************************************************//**
		@brief		Collider更新
		@param[in]	なし
		@return		なし
*//***************************************************************************/
void CapsuleCollider::Update()
{
	if (!m_pTransform)
	{
		m_position = m_offset;
	}
	else
	{
		DirectX::XMMATRIX matrix = DirectX::XMLoadFloat4x4(m_pTransform);
		DirectX::XMStoreFloat3(&m_position, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&m_offset), matrix));

		//if (!DirectX::XMVector3Equal(DirectX::XMLoadFloat3(&m_Ldirection), { 0.0f, 1.0f, 0.0f }))
		{
			// オフセット行列抽出
			DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(matrix.r[3].m128_f32[0], matrix.r[3].m128_f32[1], matrix.r[3].m128_f32[2]);
			
			// スケール行列抽出
			DirectX::XMMATRIX S = DirectX::XMMatrixScaling(
				XMVector3Length(XMVECTOR{ matrix.r[0].m128_f32[0], matrix.r[0].m128_f32[1], matrix.r[0].m128_f32[2] }).m128_f32[0],
				XMVector3Length(XMVECTOR{ matrix.r[1].m128_f32[0], matrix.r[1].m128_f32[1], matrix.r[1].m128_f32[2] }).m128_f32[0],
				XMVector3Length(XMVECTOR{ matrix.r[2].m128_f32[0], matrix.r[2].m128_f32[1], matrix.r[2].m128_f32[2] }).m128_f32[0]);
			
			// 回転行列抽出
			DirectX::XMMATRIX R = DirectX::XMMatrixInverse(nullptr, S) * matrix * DirectX::XMMatrixInverse(nullptr, T);
			
			DirectX::XMStoreFloat3(&m_Wdirection, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&m_Ldirection), R));
		}
	}
}

/**************************************************************************//**
		@brief		Capsule用パラメータセット
		@param[in]	Capsule capsule
		@return		なし
*//***************************************************************************/
void CapsuleCollider::SetParam(const Capsule& capsule)
{
	m_offset = capsule.position;
	m_Ldirection = capsule.direction;
	m_radius = capsule.radius;
	m_length = capsule.length;
}

/**************************************************************************//**
		@brief		デバッグ描画
		@param[in]	color
		@return		なし
*//***************************************************************************/
void CapsuleCollider::DrawDebugPrimitive(DirectX::XMFLOAT4 color)
{
	if (!m_enable) return;

	if (T_GRAPHICS.isDX11Active) T_GRAPHICS.GetDebugRenderer()->DrawCapsule(m_position, m_Wdirection, m_radius, m_length, color);
	else
	{
		// レンダーコンテキスト設定
		RenderContextDX12 rc;
		rc.d3d_command_list = T_GRAPHICS.GetFrameBufferManager()->GetCommandList();

		// 円柱描画
		m_cylinder->SetCylinder(m_position, m_Wdirection, m_radius, m_length, color);
		m_cylinder->Render(rc);

		// 下の球描画
		m_sphere[0]->SetSphere(m_position, m_radius, color);
		m_sphere[0]->Render(rc);

		// 上の球描画
		DirectX::XMFLOAT3 upPos = m_position + m_Wdirection * m_length;
		m_sphere[1]->SetSphere(upPos, m_radius, color);
		m_sphere[1]->Render(rc);
	}
}

bool CapsuleCollider::CollisionVsShpere(
	SphereCollider* other,
	DirectX::XMFLOAT3& direction,
	HitResult& result
)
{
	Sphere otherSphere(other->GetPosition(), other->GetRadius());
	IntersectionResult hit;
	if (Collision::IntersectSphereVsCapsule(XMLoadFloat3(&otherSphere.position), otherSphere.radius, XMLoadFloat3(&m_position), XMLoadFloat3(&m_Wdirection), m_length, m_radius, &hit))
	{
		XMStoreFloat3(&result.normal, -hit.normal);
		XMStoreFloat3(&result.position, hit.pointA);
		result.distance = hit.penetration;
		return true;
	}
	return false;
}

/**************************************************************************//**
		@brief		マップとの当たり判定
		@param[in]	なし
		@return		衝突判定
*//***************************************************************************/
bool CapsuleCollider::CollisionVsMap()
{
	Capsule capsule(m_position, m_Wdirection, m_length, m_radius);

	if (MAPTILES.IntersectCapsuleVsMap(capsule))
	{
		m_position = capsule.position;
		return true;
	}
	return false;
};
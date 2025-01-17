//! @file OBBCollider.cpp
//! @note 

#include "TAKOEngine/Runtime/tentacle_lib.h"

#include "OBBCollider.h"

OBBCollider::OBBCollider(uint16_t _objType, DirectX::XMFLOAT4X4* _transform) : Collider(_objType, _transform)
{
	m_radii = { 1.0f, 1.0f, 1.0f };
	m_orientation = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_shapeType = COLLIDER_TYPE::OBB;
}

// OBB用パラメータセット
void OBBCollider::SetParam(const OBB& obb)
{
	m_offset = obb.position;
	m_orientation = obb.orientation;
	m_radii = obb.radii;
}

bool OBBCollider::RayCast(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	HitResult& result
)
{
	return false;
}

void OBBCollider::DrawDebugPrimitive(DirectX::XMFLOAT4 color)
{
	T_GRAPHICS.GetDebugRenderer()->DrawCube(m_position, m_radii, color);
}
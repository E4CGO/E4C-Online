//! @file MapCollider.cpp
//! @note 

#include "MapCollider.h"
#include "SphereCollider.h"

#include "TAKOEngine/Tool/XMFLOAT.h"

MapCollider::MapCollider(iModel* _model, uint16_t _objType, DirectX::XMFLOAT4X4* _transform) : Collider(_objType, _transform)
{
	model = _model;
	m_shapeType = COLLIDER_TYPE::MAP;
}

bool MapCollider::CollisionVsShpere(
	SphereCollider* other,
	DirectX::XMFLOAT3& direction,
	HitResult& result
)
{
	HitResultVector hit;
	if (Collision::IntersectSphereCastVsModel(other->GetPosition(), other->GetPosition() + direction, other->GetRadius(), model, &hit))
	{
		XMStoreFloat3(&result.normal, hit.normal);
		XMStoreFloat3(&result.position, hit.position);
		result.distance = hit.distance;
		return true;
	}
	return false;
}

bool MapCollider::RayCast(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	HitResult& result
)
{
	HitResultVector hit;
	if (Collision::IntersectRayVsModel(start, end, model, hit))
	{
		XMStoreFloat3(&result.normal, hit.normal);
		XMStoreFloat3(&result.position, hit.position);
		result.distance = hit.distance;
		XMStoreFloat3(&result.triangleVerts[0], hit.triangleVerts[0]);
		XMStoreFloat3(&result.triangleVerts[1], hit.triangleVerts[1]);
		XMStoreFloat3(&result.triangleVerts[2], hit.triangleVerts[2]);
		return true;
	}
	return false;
}
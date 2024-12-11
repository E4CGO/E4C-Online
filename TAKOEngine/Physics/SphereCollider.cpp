//! @file SphereCollider.cpp
//! @note 

#include "TAKOEngine/Runtime/tentacle_lib.h"

//#include <DirectXCollision.h>
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "Source/Map/MapTileManager.h"

SphereCollider::SphereCollider(uint16_t _objType, DirectX::XMFLOAT4X4* _transform) : Collider(_objType, _transform)
{
	m_radius = 1.0f;
	m_shapeType = COLLIDER_TYPE::SPHERE;

	// DebugPrimitive用
	m_sphere = std::make_unique<SphereRenderer>(T_GRAPHICS.GetDeviceDX12());
}

// Sphere用パラメータセット
void SphereCollider::SetParam(Sphere sphere)
{
	m_offset = sphere.position;
	m_radius = sphere.radius;
}

void SphereCollider::DrawDebugPrimitive(DirectX::XMFLOAT4 color)
{
	if (!m_enable) return;

	if (T_GRAPHICS.isDX11Active)
	{
		T_GRAPHICS.GetDebugRenderer()->SetSphere(m_position, m_radius, color);
	}
	else
	{
		// レンダーコンテキスト設定
		RenderContextDX12 rc;
		rc.d3d_command_list = T_GRAPHICS.GetFrameBufferManager()->GetCommandList();

		// 描画
		m_sphere->SetSphere(m_position, m_radius, color);
		m_sphere->Render(rc);
	}
}

bool SphereCollider::CollisionVsShpere(
	SphereCollider* other,
	DirectX::XMFLOAT3& direction,
	HitResult& result
)
{
	Sphere otherSphere(other->GetPosition(), other->GetRadius());
	IntersectionResult hit;
	if (Collision::IntersectSphereVsSphere(XMLoadFloat3(&m_position), m_radius, XMLoadFloat3(&otherSphere.position), otherSphere.radius, &hit))
	{
		XMStoreFloat3(&result.normal, hit.normal);
		XMStoreFloat3(&result.position, hit.pointB);
		result.distance = hit.penetration;
		return true;
	}
	return false;
}

bool SphereCollider::CollisionVsCapsule(
	CapsuleCollider* other,
	DirectX::XMFLOAT3& direction,
	HitResult& result
)
{
	Capsule otherCapsule(other->GetPosition(), other->GetDirection(), other->GetLength(), other->GetRadius());
	IntersectionResult hit;
	if (Collision::IntersectSphereVsCapsule(XMLoadFloat3(&m_position), m_radius, XMLoadFloat3(&otherCapsule.position), XMLoadFloat3(&otherCapsule.direction), otherCapsule.length, otherCapsule.radius, &hit))
	{
		XMStoreFloat3(&result.normal, hit.normal);
		XMStoreFloat3(&result.position, hit.pointB);
		result.distance = hit.penetration;
		return true;
	}
	return false;
}

bool SphereCollider::CollisionVsMap()
{
	Sphere sphere(m_position, m_radius);

	if (MAPTILES.IntersectSphereVsMap(sphere))
	{
		m_position = sphere.position;
		return true;
	}
	return false;
};


bool SphereCollider::RayCast(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	HitResult& result
)
{
	if (!m_enable) return false;

	XMFLOAT3 dir = end - start;
	float dist = XMFLOAT3Length(dir);
	dir = XMFLOAT3Normalize(dir);
	HitResultVector hit;
	if (Collision::IntersectRayVsSphere(XMLoadFloat3(&start), XMLoadFloat3(&dir), dist, XMLoadFloat3(&m_position), m_radius, &hit))
	{
		XMStoreFloat3(&result.normal, hit.normal);
		XMStoreFloat3(&result.position, hit.position);
		result.distance = hit.distance;
		return true;
	}
	return false;
}
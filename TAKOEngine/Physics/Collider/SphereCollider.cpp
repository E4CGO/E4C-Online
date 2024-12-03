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
	T_GRAPHICS.GetDebugRenderer()->DrawSphere(m_position, m_radius, color);
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

	//DirectX::XMFLOAT3 d = other->GetPosition() - position;
	//float distanceSq = XMFLOAT3LengthSq(d);

	//if (distanceSq < powf(other->GetScale().x + scale.x, 2.0f))
	//{
	//	result.normal = XMFLOAT3Normalize(position - other->GetPosition());
	//	result.position = other->GetPosition() + (result.normal * other->GetScale());
	//	result.distance = sqrtf(distanceSq);

	//	return true;
	//}
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

	//DirectX::XMFLOAT3 p = position - start;
	//DirectX::XMFLOAT3 v = XMFLOAT3Normalize(end - start); // Ray Direction
	////float length = XMFLOAT3Length(end - start);

	//float A = v.x * v.x + v.y * v.y + v.z * v.z;
	//float B = v.x * p.x + v.y * p.y + v.z * p.z;
	//float C = p.x * p.x + p.y * p.y + p.z * p.z - scale.x * scale.x;

	//if (A == 0.0f) return false; // Ray Error

	//float s = B * B - A * C;
	//if (s < 0.0f) return false; // 衝突していない

	//s = sqrtf(s);
	//float a1 = (B - s) / A;
	//float a2 = (B + s) / A;

	//if (a1 < 0.0f || a2 < 0.0f) return false; // レイの反対で衝突

	//result.position.x = start.x + a1 * v.x;
	//result.position.y = start.y + a1 * v.y;
	//result.position.z = start.z + a1 * v.z;

	//result.distance = XMFLOAT3Length(result.position - start);

	//if (result.distance > XMFLOAT3Length(end - start))
	//{
	//	result.distance = 0.0f;
	//	result.position = {};
	//	return false;
	//}

	//result.normal = XMFLOAT3Normalize(result.position - position);

	//return true;
}
//! @file Collider.cpp
//! @note 

#include <typeinfo>
#include "Collider.h"
#include "SphereCollider.h"
#include "AABBCollider.h"
#include "OBBCollider.h"
#include "CapsuleCollider.h"
#include "ModelCollider.h"

void Collider::Update()
{
	if (!m_pTransform)
	{
		m_position = m_offset;
	}
	else
	{
		DirectX::XMMATRIX matrix = DirectX::XMLoadFloat4x4(m_pTransform);
		DirectX::XMStoreFloat3(&m_position, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&m_offset), matrix));
	}
}

void Collider::SetPosition(const DirectX::XMFLOAT3 pos)
{
	//DirectX::XMFLOAT3 v = m_position - pos;

	m_position = pos;

	//if (m_pTransform)
	//{
	//	m_pTransform->_41 += v.x;
	//	m_pTransform->_42 += v.y;
	//	m_pTransform->_43 += v.z;
	//}
}

bool Collider::Collision(
	Collider* other,
	DirectX::XMFLOAT3 direction,
	HitResult& result
)
{
	if (!m_enable || !other->IsEnable()) return false;

	switch (other->m_shapeType)
	{
	case Collider::COLLIDER_TYPE::SPHERE:
		return CollisionVsShpere(static_cast<SphereCollider*>(other), direction, result);
		break;
	case Collider::COLLIDER_TYPE::AABB:
		return CollisionVsAABB(static_cast<AABBCollider*>(other), direction, result);
		break;
	case Collider::COLLIDER_TYPE::OBB:
		return CollisionVsOBB(static_cast<OBBCollider*>(other), direction, result);
		break;
	case Collider::COLLIDER_TYPE::CAPSULE:
		return CollisionVsCapsule(static_cast<CapsuleCollider*>(other), direction, result);
		break;
	case Collider::COLLIDER_TYPE::MODEL:
		return CollisionVsModel(static_cast<ModelCollider*>(other), direction, result);
		break;
	default:
		return false;
		break;
	}
}
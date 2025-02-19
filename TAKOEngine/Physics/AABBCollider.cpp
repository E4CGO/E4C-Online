//! @file AABBCollider.cpp
//! @note 

#include "TAKOEngine/Runtime/tentacle_lib.h"

#include "AABBCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include <imgui.h>

AABBCollider::AABBCollider(uint16_t _objType, DirectX::XMFLOAT4X4* _transform) : Collider(_objType, _transform)
{
	m_radii = { 1.0f, 1.0f, 1.0f };
	m_shapeType = COLLIDER_TYPE::AABB;

	// DebugPrimitive用
	m_cube = std::make_unique<CubeRenderer>(T_GRAPHICS.GetDeviceDX12());
}

// AABB用パラメータセット
void AABBCollider::SetParam(const AABB& aabb)
{
	m_offset = aabb.position;
	m_radii = aabb.radii;
}

bool AABBCollider::CollisionVsShpere(
	SphereCollider* other,
	DirectX::XMFLOAT3& direction,
	HitResult& result
)
{
	Sphere otherSphere(other->GetPosition(), other->GetRadius());
	IntersectionResult hit;
	if (Collision::IntersectSphereVsAABB(XMLoadFloat3(&otherSphere.position), otherSphere.radius, XMLoadFloat3(&m_position), XMLoadFloat3(&m_radii), &hit))
	{
		XMStoreFloat3(&result.normal, -hit.normal);
		XMStoreFloat3(&result.position, hit.pointA);
		result.distance = hit.penetration;
		return true;
	}
	return false;
}

bool AABBCollider::CollisionVsCapsule(
	CapsuleCollider* other,
	DirectX::XMFLOAT3& direction,
	HitResult& result
)
{
	Capsule otherCapsule(other->GetPosition(), other->GetDirection(), other->GetLength(), other->GetRadius());
	IntersectionResult hit;
	if (Collision::IntersectCapsuleVsAABB(XMLoadFloat3(&otherCapsule.position), XMLoadFloat3(&otherCapsule.direction), otherCapsule.length, otherCapsule.radius, XMLoadFloat3(&m_position), XMLoadFloat3(&m_radii), &hit))
	{
		XMStoreFloat3(&result.normal, -hit.normal);
		XMStoreFloat3(&result.position, hit.pointA);
		result.distance = hit.penetration;
		return true;
	}
	return false;
}


bool AABBCollider::CollisionVsAABB(
	AABBCollider* other,
	DirectX::XMFLOAT3& direction,
	HitResult& result
)
{
	AABB otherAABB(other->GetPosition(), other->GetRadii());
	IntersectionResult hit;
	if (Collision::IntersectAABBVsAABB(XMLoadFloat3(&m_position), XMLoadFloat3(&m_radii), XMLoadFloat3(&otherAABB.position), XMLoadFloat3(&otherAABB.radii), &hit))
	{
		XMStoreFloat3(&result.normal, hit.normal);
		XMStoreFloat3(&result.position, hit.pointB);
		result.distance = hit.penetration;
		return true;
	}
	return false;
}

bool AABBCollider::RayCast(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	HitResult& result
)
{
	return false;
}

void AABBCollider::DrawDebugGUI()
{
	if (ImGui::TreeNode("UnrotatedBox"))
	{
		ImGui::DragFloat3("position", &m_position.x);
		ImGui::DragFloat3("radii", &m_radii.x);
		ImGui::TreePop();
	}
}

void AABBCollider::DrawDebugPrimitive(DirectX::XMFLOAT4 color)
{
	if (!m_enable) return;
	XMFLOAT3 size = m_radii * 2;

	if (T_GRAPHICS.isDX11Active)
	{
		T_GRAPHICS.GetDebugRenderer()->DrawCube(m_position, size, color);
	}
	else
	{
		// レンダーコンテキスト設定
		RenderContextDX12 rc;
		rc.d3d_command_list = T_GRAPHICS.GetFrameBufferManager()->GetCommandList();

		// 描画
		m_cube->SetCube(m_position, size, color);
		m_cube->Render(rc);
	}
}
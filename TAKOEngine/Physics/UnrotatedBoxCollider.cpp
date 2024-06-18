#include "TAKOEngine/Runtime/tentacle_lib.h"

#include "UnrotatedBoxCollider.h"
#include <imgui.h>

UnrotatedBoxCollider::UnrotatedBoxCollider(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale)
{
	type = COLLIDER_TYPE::UNROTATED_BOX;
	this->position = position;
	this->scale = scale;
}

UnrotatedBoxCollider::UnrotatedBoxCollider() : UnrotatedBoxCollider({ 0, 0, 0 }, { 1, 1, 1 }) {}

bool UnrotatedBoxCollider::CollisionVsUnrotatedBox(
	Collider*& other,
	DirectX::XMFLOAT3& direction,
	HitResult& result
)
{
	DirectX::XMFLOAT3 oPosition = other->GetPosition();
	DirectX::XMFLOAT3 oScale = other->GetScale();

	if (direction.y > 0) // upward
	{
		if (position.y - scale.y / 2.0f - direction.y >= oPosition.y + oScale.y / 2.0f) return false; // bottom
		if (position.y + scale.y / 2.0f <= oPosition.y - oScale.y / 2.0f) return false; // top
	}
	else if (direction.y < 0) // downward
	{
		if (position.y - scale.y / 2.0f >= oPosition.y + oScale.y / 2.0f) return false; // bottom
		if (position.y + scale.y / 2.0f - direction.y <= oPosition.y - oScale.y / 2.0f) return false; // top
	}
	else
	{
		if (position.y - scale.y / 2.0f >= oPosition.y + oScale.y / 2.0f) return false; // bottom
		if (position.y + scale.y / 2.0f <= oPosition.y - oScale.y / 2.0f) return false; // top
	}

	if (direction.x > 0) // rightward
	{
		if (position.x - scale.x / 2.0f - direction.x >= oPosition.x + oScale.x / 2.0f) return false; // left
		if (position.x + scale.x / 2.0f <= oPosition.x - oScale.x / 2.0f) return false; // right
	}
	else if (direction.x < 0) // leftward
	{
		if (position.x - scale.x / 2.0f >= oPosition.x + oScale.x / 2.0f) return false; // left
		if (position.x + scale.x / 2.0f - direction.x <= oPosition.x - oScale.x / 2.0f) return false; // right
	}
	else
	{
		if (position.x - scale.x / 2.0f >= oPosition.x + oScale.x / 2.0f) return false; // left
		if (position.x + scale.x / 2.0f <= oPosition.x - oScale.x / 2.0f) return false; // right
	}

	if (direction.z > 0) // fontward
	{
		if (position.z - scale.z / 2.0f - direction.z >= oPosition.z + oScale.z / 2.0f) return false; // back
		if (position.z + scale.z / 2.0f <= oPosition.z - oScale.z / 2.0f) return false; // font
	}
	else if (direction.z > 0) // backward
	{
		if (position.z - scale.z / 2.0f >= oPosition.z + oScale.z / 2.0f) return false; // back
		if (position.z + scale.z / 2.0f - direction.z <= oPosition.z - oScale.z / 2.0f) return false; // font
	}
	else
	{
		if (position.z - scale.z / 2.0f >= oPosition.z + oScale.z / 2.0f) return false; // back
		if (position.z + scale.z / 2.0f <= oPosition.z - oScale.z / 2.0f) return false; // font
	}

	// Hit
	if (direction.y > 0) // upward
		result.position.y = oPosition.y - oScale.y / 2.0f - position.y - scale.y / 2.0f;
	if (direction.y < 0) // downward
		result.position.y = oPosition.y + oScale.y / 2.0f - position.y + scale.y / 2.0f;

	if (direction.x > 0) // rightward
		result.position.x = oPosition.x - oScale.x / 2.0f - position.x - scale.x / 2.0f;
	if (direction.x < 0) // leftward
		result.position.x = oPosition.x + oScale.x / 2.0f - position.x + scale.x / 2.0f;

	if (direction.z > 0) // backward
		result.position.z = oPosition.z - oScale.z / 2.0f - position.z - scale.z / 2.0f;
	if (direction.z < 0) // forward
		result.position.z = oPosition.z + oScale.z / 2.0f - position.z + scale.z / 2.0f;

	return true;
}

bool UnrotatedBoxCollider::RayCast(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	HitResult& result
)
{
	return false;
}

DirectX::XMFLOAT3 UnrotatedBoxCollider::GetTop()
{
	return position + DirectX::XMFLOAT3(0.0f, scale.y / 2.0f, 0.0f);
}

void UnrotatedBoxCollider::DrawDebugGUI()
{
	if (ImGui::TreeNode("UnrotatedBox"))
	{
		ImGui::DragFloat3("position", &position.x);
		ImGui::DragFloat3("scale", &scale.x);
		ImGui::TreePop();
	}
}

void UnrotatedBoxCollider::DrawDebugPrimitive(DirectX::XMFLOAT4 color)
{
	T_GRAPHICS.GetDebugRenderer()->DrawCube(position, scale, color);
}
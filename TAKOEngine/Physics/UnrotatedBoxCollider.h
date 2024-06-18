#pragma once
#include "Collider.h"
class UnrotatedBoxCollider : public Collider
{
public:
	UnrotatedBoxCollider(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale);
	UnrotatedBoxCollider();
private:
	bool CollisionVsUnrotatedBox(
		Collider*& other,
		DirectX::XMFLOAT3& direction,
		HitResult& result
	) override;

	bool RayCast(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		HitResult& result
	) override;

	DirectX::XMFLOAT3 GetTop() override;

	void DrawDebugGUI() override;
	void DrawDebugPrimitive(DirectX::XMFLOAT4 color = { 1, 1, 1, 1 }) override;
};

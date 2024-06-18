#pragma once

#include "Collider.h"

class SphereCollider : public Collider
{
public:
	SphereCollider(float radius = 1.0f);
	~SphereCollider() = default;

	bool CollisionVsShpere(
		Collider*& other,
		DirectX::XMFLOAT3& direction,
		HitResult& result
	) override;

	//bool CollisionVsModel(
	//	Collider*& other,				// ƒ‚ƒfƒ‹
	//	DirectX::XMFLOAT3& direction,
	//	HitResult& result
	//);

	bool RayCast(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		HitResult& result
	) override;

	void DrawDebugPrimitive(DirectX::XMFLOAT4 color = { 1, 1, 1, 1 }) override;
};

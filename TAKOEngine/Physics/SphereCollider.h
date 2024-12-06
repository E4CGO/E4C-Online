﻿#pragma once

#include "Collider.h"
#include "TAKOEngine\Rendering\DebugRenderer\SphereRenderer.h"

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
	//	Collider*& other,				// モデル
	//	DirectX::XMFLOAT3& direction,
	//	HitResult& result
	//);

	bool CollisionVsMap() override;

	bool RayCast(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		HitResult& result
	) override;

	// Sphere用パラメータセット
	void SetParam(Sphere sphere) override;

	float GetRadius() const { return radius; }

	void DrawDebugPrimitive(DirectX::XMFLOAT4 color = { 1, 1, 1, 1 }) override;

private:
	float radius;

	std::unique_ptr<SphereRenderer> m_sphere;
};

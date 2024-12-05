﻿//! @file CapsuleCollider.h
//! @note カプセルコリダー

#ifndef __CAPSULE_COLLIDER_H__
#define __CAPSULE_COLLIDER_H__

#include "Collider.h"
#include "TAKOEngine\Rendering\DebugRenderer\CylinderRenderer.h"
#include "TAKOEngine\Rendering\DebugRenderer\SphereRenderer.h"

class CapsuleCollider : public Collider
{
public:
	CapsuleCollider();
	~CapsuleCollider() = default;

	bool CollisionVsShpere(
		Collider*& other,
		DirectX::XMFLOAT3& direction,
		HitResult& result
	) override {
		return false;
	}

	bool CollisionVsMap() override;

	bool RayCast(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		HitResult& result
	) override {
		return false;
	}

	// Capsule用パラメータセット
	void SetParam(Capsule capsule) override;

	float GetRadius() const { return radius; }

	void DrawDebugPrimitive(DirectX::XMFLOAT4 color = { 1, 1, 1, 1 }) override;

private:
	float radius;
	float length;
	DirectX::XMFLOAT3 direction;	// 向き（正規化）

	std::unique_ptr<SphereRenderer> m_sphere[2];
	std::unique_ptr<CylinderRenderer> m_cylinder;
};

#endif // !__CAPSULE_COLLIDER_H__

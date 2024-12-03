//! @file CapsuleCollider.h
//! @note カプセルコリダー

#ifndef __CAPSULE_COLLIDER_H__
#define __CAPSULE_COLLIDER_H__

#include "Collider.h"

class CapsuleCollider : public Collider
{
public:
	CapsuleCollider(uint16_t _objType, DirectX::XMFLOAT4X4* _transform);
	~CapsuleCollider() = default;

	bool CollisionVsShpere(
		SphereCollider* other,
		DirectX::XMFLOAT3& direction,
		HitResult& result
	) override;

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
	// Capsule用パラメータゲット
	Capsule GetCapsule() override { return Capsule(m_offset, m_direction, m_length, m_radius); }

	const float GetLength() const { return m_length; }
	const float GetRadius() const { return m_radius; }
	const DirectX::XMFLOAT3 GetDirection() const { return m_direction; }

	void DrawDebugPrimitive(DirectX::XMFLOAT4 color = { 1, 1, 1, 1 }) override;

private:
	float m_radius = 0.0f;
	float m_length = 0.0f;
	DirectX::XMFLOAT3 m_direction = { 0.0f, 0.0f, 0.0f };	// 向き（正規化）
};

#endif // !__CAPSULE_COLLIDER_H__
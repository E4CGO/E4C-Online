//! @file CapsuleCollider.h
//! @note カプセルコリダー

#ifndef __CAPSULE_COLLIDER_H__
#define __CAPSULE_COLLIDER_H__

#include "Collider.h"
#include "TAKOEngine\Rendering\DebugRenderer\CylinderRenderer.h"
#include "TAKOEngine\Rendering\DebugRenderer\SphereRenderer.h"

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

	bool CollisionVsCapsule(
		CapsuleCollider* other,
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

	void Update() override;

	// Capsule用パラメータセット
	void SetParam(const Capsule& capsule) override;
	// Capsule用パラメータゲット
	Capsule GetCapsule() override { return Capsule(m_offset, m_Ldirection, m_length, m_radius); }

	const float GetLength() const { return m_length; }
	const float GetRadius() const { return m_radius; }
	const DirectX::XMFLOAT3 GetDirection() const { return m_Wdirection; }

	void DrawDebugPrimitive(DirectX::XMFLOAT4 color = { 1, 1, 1, 1 }) override;

private:
	float m_radius = 0.0f;
	float m_length = 0.0f;
	DirectX::XMFLOAT3 m_Wdirection = { 0.0f, 0.0f, 0.0f };	// ワールド方向（正規化）
	DirectX::XMFLOAT3 m_Ldirection = { 0.0f, 0.0f, 0.0f };	// ローカル空間での方向（正規化）

	std::unique_ptr<SphereRenderer> m_sphere[2];
	std::unique_ptr<CylinderRenderer> m_cylinder;
};

#endif // !__CAPSULE_COLLIDER_H__
//! @file AABBCollider.h
//! @note 

#ifndef __AABB_COLLIDER_H__
#define __AABB_COLLIDER_H__

#include "Collider.h"
#include "TAKOEngine/Rendering/DebugRenderer/CubeRenderer.h"

class AABBCollider : public Collider
{
public:
	AABBCollider(uint16_t _objType, DirectX::XMFLOAT4X4* _transform);
	~AABBCollider() = default;

	//void Update() override;

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

	bool CollisionVsAABB(
		AABBCollider* other,
		DirectX::XMFLOAT3& direction,
		HitResult& result
	) override;

	bool RayCast(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		HitResult& result
	) override;

	// AABB用パラメータセット
	void SetParam(const AABB& aabb) override;

	const DirectX::XMFLOAT3 GetRadii() const { return m_radii; }

	void DrawDebugGUI() override;
	void DrawDebugPrimitive(DirectX::XMFLOAT4 color = { 1, 1, 1, 1 }) override;

private:
	DirectX::XMFLOAT3 m_radii = { 0.0f, 0.0f, 0.0f };
	std::unique_ptr<CubeRenderer> m_cube;
};

#endif // !__AABB_COLLIDER_H__
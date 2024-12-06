//! @file SphereCollider.h
//! @note 

#ifndef __SPHERE_COLLIDER_H__
#define __SPHERE_COLLIDER_H__

#include "Collider.h"
#include "TAKOEngine\Rendering\DebugRenderer\SphereRenderer.h"

class SphereCollider : public Collider
{
public:
	SphereCollider(uint16_t _objType, DirectX::XMFLOAT4X4* _transform);
	~SphereCollider() = default;

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
	// Sphere用パラメータゲット
	Sphere GetSphere() override { return Sphere(m_offset, m_radius); }

	const float GetRadius() const { return m_radius; }

	void DrawDebugPrimitive(DirectX::XMFLOAT4 color = { 1, 1, 1, 1 }) override;

private:
	float m_radius = 0.0f;

	std::unique_ptr<SphereRenderer> m_sphere;
};

#endif // !__SPHERE_COLLIDER_H__
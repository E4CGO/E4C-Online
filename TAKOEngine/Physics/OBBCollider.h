//! @file OBBCollider.h
//! @note 

#ifndef __OBB_COLLIDER_H__
#define __OBB_COLLIDER_H__

//#include <DirectXCollision.h>

#include "Collider.h"

class OBBCollider : public Collider
{
public:
	OBBCollider(uint16_t _objType, DirectX::XMFLOAT4X4* _transform);
	~OBBCollider() = default;

	virtual bool RayCast(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		HitResult& result
	) override;


	// OBB用パラメータセット
	void SetParam(const OBB& obb) override;

	const DirectX::XMFLOAT3 GetRadii() const { return m_radii; }
	const DirectX::XMFLOAT4 GetOrientation() const { return m_orientation; }

	void DrawDebugPrimitive(DirectX::XMFLOAT4 color = { 1, 1, 1, 1 }) override;

protected:
	DirectX::XMFLOAT3 m_radii = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT4 m_orientation = { 0.0f, 0.0f, 0.0f, 0.0f };
};

#endif // !__OBB_COLLIDER_H__
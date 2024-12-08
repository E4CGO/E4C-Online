//! @file MapCollider.h
//! @note 

#ifndef __MAP_COLLIDER_H__
#define __MAP_COLLIDER_H__

#include "TAKOEngine/Rendering/Model/Model.h"
#include "Collider.h"

class MapCollider : public Collider
{
public:
	MapCollider(iModel* _model, uint16_t _objType, DirectX::XMFLOAT4X4* _transform);
	~MapCollider() = default;

	bool CollisionVsShpere(
		SphereCollider* other,
		DirectX::XMFLOAT3& direction,
		HitResult& result
	) override;

	bool RayCast(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		HitResult& result
	) override;

protected:
	iModel* model;

};

#endif // !__MAP_COLLIDER_H__
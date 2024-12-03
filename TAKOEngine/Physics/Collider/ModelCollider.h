//! @file ModelCollider.h
//! @note 

#ifndef __MODEL_COLLIDER_H__
#define __MODEL_COLLIDER_H__

#include "TAKOEngine/Rendering/Model/Model.h"
#include "TAKOEngine/Physics/Collider/Collider.h"

class ModelCollider : public Collider
{
public:
	ModelCollider(iModel* _model, uint16_t _objType, DirectX::XMFLOAT4X4* _transform);
	~ModelCollider() = default;

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

#endif // !__MODEL_COLLIDER_H__
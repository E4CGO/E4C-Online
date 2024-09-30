#pragma once

#include "TAKOEngine/Rendering/Model/Model.h"
#include "TAKOEngine/Physics/Collider.h"

class ModelCollider : public Collider
{
public:
	ModelCollider(iModel* model);
	~ModelCollider() = default;

	bool CollisionVsShpere(
		Collider*& other,
		DirectX::XMFLOAT3& direction,
		HitResult& result
	) override;

	virtual bool RayCast(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		HitResult& result
	) override;
protected:
	iModel* model;
};

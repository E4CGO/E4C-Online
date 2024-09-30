#pragma once

#include <DirectXCollision.h>

#include "TAKOEngine/Physics/Collider.h"
#include "TAKOEngine/Rendering/Model/Model.h"

class BoundingBoxCollider : public Collider
{
public:
	BoundingBoxCollider(iModel* model);
	~BoundingBoxCollider() = default;

	virtual bool RayCast(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		HitResult& result
	) override;

	void DrawDebugPrimitive(DirectX::XMFLOAT4 color = { 1, 1, 1, 1 }) override;

private:
	void UpdateCollider();
protected:
	iModel* model;

	DirectX::BoundingBox boundingBox;
	DirectX::XMFLOAT3 center = {};
	DirectX::XMFLOAT3 extends = {};
};

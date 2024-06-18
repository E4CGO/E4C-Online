#include "TAKOEngine/Runtime/tentacle_lib.h"

#include "BoundingBoxCollider.h"

BoundingBoxCollider::BoundingBoxCollider(Model* model)
{
	this->model = model;

	type = COLLIDER_TYPE::BOUNDING_BOX;

	// DirectX バウンディングボックス
	// 頂点
	DirectX::XMFLOAT3 highest = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
	DirectX::XMFLOAT3 lowest = { FLT_MAX, FLT_MAX, FLT_MAX };
	for (const ModelResource::Mesh& mesh : model->GetResource()->GetMeshes())
	{
		for (ModelResource::Vertex vertex : mesh.vertices)
		{
			if (vertex.position.x > highest.x) highest.x = vertex.position.x;
			if (vertex.position.y > highest.y) highest.y = vertex.position.y;
			if (vertex.position.z > highest.z) highest.z = vertex.position.z;
			if (vertex.position.x < lowest.x) lowest.x = vertex.position.x;
			if (vertex.position.y < lowest.y) lowest.y = vertex.position.y;
			if (vertex.position.z < lowest.z) lowest.z = vertex.position.z;
		}
	}

	extends = highest - lowest;
	center = (extends) * 0.5f;
	boundingBox = DirectX::BoundingBox(
		center,			// Center
		extends
	);
}

void BoundingBoxCollider::DrawDebugPrimitive(DirectX::XMFLOAT4 color)
{
	if (!enable) return;

	T_GRAPHICS.GetDebugRenderer()->DrawCube(
		center,
		extends,
		color
	);
}

bool BoundingBoxCollider::RayCast(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	HitResult& result
)
{
	return false;
}

void BoundingBoxCollider::UpdateCollider()
{
}
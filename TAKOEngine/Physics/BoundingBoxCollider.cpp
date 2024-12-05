#include "TAKOEngine/Runtime/tentacle_lib.h"

#include "BoundingBoxCollider.h"

BoundingBoxCollider::BoundingBoxCollider(iModel* model)
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

	// DebugPrimitive用
	m_cube = std::make_unique<CubeRenderer>(T_GRAPHICS.GetDeviceDX12());
}

void BoundingBoxCollider::DrawDebugPrimitive(DirectX::XMFLOAT4 color)
{
	if (!enable) return;

	if (T_GRAPHICS.isDX11Active) T_GRAPHICS.GetDebugRenderer()->DrawCube(center, extends, color);
	else
	{
		// レンダーコンテキスト設定
		RenderContextDX12 rc;
		rc.d3d_command_list = T_GRAPHICS.GetFrameBufferManager()->GetCommandList();

		// 描画
		m_cube->SetCube(center, extends, color);
		m_cube->Render(rc);
	}
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
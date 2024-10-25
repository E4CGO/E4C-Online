#include "MapTileManager.h"

// レイキャスト
bool MapTileManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit, bool camera)
{
	//HitResult temp;
	//hit.distance = FLT_MAX;
	//for (ModelObject*& item : items)
	//{
	//	if (item->GetCollider() == nullptr) continue;
	//	if (item->GetCollider()->RayCast(start, end, temp) && (temp.distance < hit.distance))
	//	{
	//		hit = temp; // 最短
	//	}
	//}
	//return hit.distance < FLT_MAX;
	
	HitResultVector result;
	bool ret = quadtree.IntersectRayVsTriangle(start, end, result);
	if (ret)
	{
		XMStoreFloat3(&hit.position, result.position);
		XMStoreFloat3(&hit.normal, result.normal);
		hit.distance = result.distance;
		hit.materialIndex = result.materialIndex;
		XMStoreFloat3(&hit.triangleVerts[0], result.triangleVerts[0]);
		XMStoreFloat3(&hit.triangleVerts[1], result.triangleVerts[1]);
		XMStoreFloat3(&hit.triangleVerts[2], result.triangleVerts[2]);
	}
	return ret;
}

// 垂直レイキャスト
bool MapTileManager::VerticalRayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	HitResultVector result;
	bool ret = quadtree.IntersectVerticalRayVsTriangle(start, end, result);

	XMStoreFloat3(&hit.position, result.position);
	XMStoreFloat3(&hit.normal, result.normal);
	hit.distance = result.distance;
	hit.materialIndex = result.materialIndex;
	XMStoreFloat3(&hit.triangleVerts[0], result.triangleVerts[0]);
	XMStoreFloat3(&hit.triangleVerts[1], result.triangleVerts[1]);
	XMStoreFloat3(&hit.triangleVerts[2], result.triangleVerts[2]);

	return ret;
}

// マップサイズ計算
void MapTileManager::CalcMapArea(DirectX::XMFLOAT3& minPos, DirectX::XMFLOAT3& maxPos)
{
	minPos = { FLT_MAX, FLT_MAX , FLT_MAX };
	maxPos = { FLT_MIN, FLT_MIN , FLT_MIN };

	for (ModelObject*& item : items)
	{
		if (item->GetCollider() == nullptr) continue;

		const ModelResource* resource = item->GetModel()->GetResource();
		for (const ModelResource::Mesh& mesh : resource->GetMeshes())
		{
			// メッシュノード取得
			const iModel::Node& node = item->GetModel()->GetNodes().at(mesh.nodeIndex);
			// ワールド行列
			XMMATRIX WorldTransform = XMLoadFloat4x4(&node.worldTransform);

			// 頂点データ取得
			const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;

			for (UINT i = 1; i < vertices.size(); i++)
			{
				const XMVECTOR V = XMVector3Transform(DirectX::XMLoadFloat3(&vertices.at(i).position), WorldTransform);
				XMFLOAT3 tmpPos;
				XMStoreFloat3(&tmpPos, V);

				if (minPos.x > tmpPos.x)
				{
					minPos.x = tmpPos.x;
				}
				else if (maxPos.x < tmpPos.x)
				{
					maxPos.x = tmpPos.x;
				}

				if (minPos.y > tmpPos.y)
				{
					minPos.y = tmpPos.y;
				}
				else if (maxPos.y < tmpPos.y)
				{
					maxPos.y = tmpPos.y;
				}

				if (minPos.z > tmpPos.z)
				{
					minPos.z = tmpPos.z;
				}
				else if (maxPos.z < tmpPos.z)
				{
					maxPos.z = tmpPos.z;
				}
			}
		}
	}
}

// 四分木空間生成
void MapTileManager::CreateQuadtree(DirectX::XMFLOAT3 center, float halfSize, uint32_t depth)
{
	quadtree.CreateQuadtree(center, halfSize, depth);

	for (ModelObject*& item : items)
	{
		int count = 0;
		int nowMeshNum = 0;

		// モデルのメッシュの三角形をワールド座標で四分木空間に登録
		const ModelResource* resource = item->GetModel()->GetResource();
		for (const ModelResource::Mesh& mesh : resource->GetMeshes())
		{
			// メッシュノード取得
			const iModel::Node& node = item->GetModel()->GetNodes().at(mesh.nodeIndex);

			DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
			const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
			const std::vector<UINT> indices = mesh.indices;

			for (UINT i = 0; i < indices.size(); i += 3)//102837

				//for (UINT i = 0; i < subset.indexCount; i += 3)
			{
				//UINT index = subset.startIndex + i;
				UINT index = i;

				// 三角形の頂点を抽出
				const ModelResource::Vertex& a = vertices.at(indices.at(index + 0));
				const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
				const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

				const DirectX::XMVECTOR A = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&a.position), WorldTransform);
				const DirectX::XMVECTOR B = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&b.position), WorldTransform);
				const DirectX::XMVECTOR C = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&c.position), WorldTransform);

				Triangle triangle = {};
				DirectX::XMStoreFloat3(&triangle.position[0], A);
				DirectX::XMStoreFloat3(&triangle.position[1], B);
				DirectX::XMStoreFloat3(&triangle.position[2], C);
				//triangle.materialIndex = mesh.materialIndex;

				// 三角形をワールド座標で四分木に登録
				quadtree.InsertTriangleObject(triangle);
				//octotree->InsertTriangleObject(triangle);

				count++;
			}
			nowMeshNum++;
		}
	}
}
void MapTileManager::CreateQuadtree(uint32_t depth)
{
	DirectX::XMFLOAT3 minPos, maxPos;
	CalcMapArea(minPos, maxPos);

	XMFLOAT3 center = (minPos + maxPos) * 0.5f;
	XMFLOAT3 size = maxPos - minPos;
	float halfSize = max(size.x, max(size.y, size.z)) * 0.5f;
	halfSize += 5.0f;
	CreateQuadtree(center, halfSize, depth);
}

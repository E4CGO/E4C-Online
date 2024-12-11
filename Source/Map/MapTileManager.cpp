#include "MapTileManager.h"

#include <iostream>

void MapTileManager::Clear()
{
	mapQuadtree.ClearQuadtree();

	Manager::Clear();
}

// レイキャスト
bool MapTileManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit, bool camera)
{
	XMFLOAT3 rayDirection = end - start;
	float rayDist = XMFLOAT3Length(rayDirection);
	rayDirection = XMFLOAT3Normalize(rayDirection);

	return RayCast(start, rayDirection, rayDist, hit, camera);
}
bool MapTileManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& direction, float dist, HitResult& hit, bool camera)
{
	return mapQuadtree.IntersectVsRay(start, direction, dist, hit);
}

// スフィアキャスト
bool MapTileManager::SphereCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& direction, float dist, float radius, HitResult& hit)
{
	return mapQuadtree.IntersectVsSphereCast(start, direction, dist, radius, hit);
}

// 球の押し戻し
bool MapTileManager::IntersectSphereVsMap(Sphere& sphere)
{
	return mapQuadtree.IntersectVsSphere(sphere);
}

// カプセルの押し戻し
bool MapTileManager::IntersectCapsuleVsMap(Capsule& capsule)
{
	return mapQuadtree.IntersectVsCapsule(capsule);
}

// マップサイズ計算
void MapTileManager::CalcMapArea(DirectX::XMFLOAT3& minPos, DirectX::XMFLOAT3& maxPos)
{
	minPos = { FLT_MAX, FLT_MAX , FLT_MAX };
	maxPos = { FLT_MIN, FLT_MIN , FLT_MIN };

	for (ModelObject*& item : items)
	{
		if (item->GetMoveCollider() == nullptr) continue;

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

void MapTileManager::CreateSpatialIndex(uint32_t quadDepth, uint32_t octDepth, DirectX::XMFLOAT3* minPos, DirectX::XMFLOAT3* maxPos)
{
	XMFLOAT3 c_minPos, c_maxPos;

	if (!minPos || !maxPos)
	{
		CalcMapArea(c_minPos, c_maxPos);
	}
	else
	{
		c_minPos = { minPos->x, minPos->y, minPos->z };
		c_maxPos = { maxPos->x, maxPos->y, maxPos->z };
	}

	mapQuadtree.CreateQuadtree(XMFLOAT3{ c_minPos.x - 1.0f, 0, c_minPos.z - 1.0f }, XMFLOAT3{ c_maxPos.x + 1.0f, 0, c_maxPos.z + 1.0f }, quadDepth);

	InsertMapMesh();
}

// マップのメッシュを登録
int MapTileManager::InsertMapMesh()
{
	int count = 0;

	for (ModelObject*& item : items)
	{
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

			for (UINT i = 0; i < indices.size(); i += 3)
			{
				// 三角形の頂点を抽出
				const ModelResource::Vertex& a = vertices.at(indices.at(i));
				const ModelResource::Vertex& b = vertices.at(indices.at(i + 1));
				const ModelResource::Vertex& c = vertices.at(indices.at(i + 2));

				const DirectX::XMVECTOR A = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&a.position), WorldTransform);
				const DirectX::XMVECTOR B = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&b.position), WorldTransform);
				const DirectX::XMVECTOR C = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&c.position), WorldTransform);

				Triangle* p_triangle = new Triangle;
				DirectX::XMStoreFloat3(&p_triangle->position[0], A);
				DirectX::XMStoreFloat3(&p_triangle->position[1], B);
				DirectX::XMStoreFloat3(&p_triangle->position[2], C);

				mapQuadtree.Regist(p_triangle);

				count++;
			}
			nowMeshNum++;
		}
	}

	return count;
}

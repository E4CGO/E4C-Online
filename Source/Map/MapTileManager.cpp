#include "MapTileManager.h"

#include <iostream>

void MapTileManager::Clear()
{
	//quadtree.ClearAllAABBObject();
	//quadtree.ClearAllCapsuleObject();
	//quadtree.ClearAllSphereObject();
	//quadtree.ClearAllTriangleObject();
	//
	//octree.ClearAllAABBObject();
	//octree.ClearAllCapsuleObject();
	//octree.ClearAllSphereObject();
	//octree.ClearAllTriangleObject();

	//tree.Finalize();

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
	// 最小値最大値
	//float minX, maxX;
	//if (direction.x > 0.0f)
	//{
	//	minX = start.x;
	//	maxX = start.x + direction.x * dist;
	//}
	//else
	//{
	//	minX = start.x + direction.x * dist;
	//	maxX = start.x;
	//}

	//float minY, maxY;
	//if (direction.y > 0.0f)
	//{
	//	minY = start.y;
	//	maxY = start.y + direction.y * dist;
	//}
	//else
	//{
	//	minY = start.y + direction.y * dist;
	//	maxY = start.y;
	//}

	//float minZ, maxZ;
	//if (direction.z > 0.0f)
	//{
	//	minZ = start.z;
	//	maxZ = start.z + direction.z * dist;
	//}
	//else
	//{
	//	minZ = start.z + direction.z * dist;
	//	maxZ = start.z;
	//}

	//// レイが通る空間の配列番号算出
	//int Elem = tree.GetLinerIndex(minX, maxX, minY, maxY, minZ, maxZ);

	//bool ret = false;
	//SearchChildren(Elem, start, direction, dist, hit, ret);
	//SearchParent(Elem, start, direction, dist, hit, ret);
	//return ret;

	return mapQuadtree.IntersectVsRay(start, direction, dist, hit);
	
	/* 作成中
	/////////////////////////////////////////////////////
	// 制御用変数
	bool ret = false;	// 全体の交差結果。一度でも三角形との交差が出ればtrue
	bool ret2 = false;	// 階層ごとの交差結果。一度でも三角形の交差が出ればtrue

	// レイの長さでヒット情報の距離を初期化
	hit.distance = dist;

	// xz軸それぞれのレイの向きをプラス→１、ゼロ→０、マイナス→－１で記録
	int directionX = direction.x > 0.0f ? 1 : (direction.x < 0.0f ? -1 : 0);
	int directionY = direction.y > 0.0f ? 1 : (direction.y < 0.0f ? -1 : 0);
	int directionZ = direction.z > 0.0f ? 1 : (direction.z < 0.0f ? -1 : 0);

	// レイの始点のモートンコードを算出
	uint32_t mortonCode = tree.Get3DMortonNumber(start.x, start.y, start.z);

	// DDAを使ってレイで辿るノードを算出するための各変数の準備

	// 空間の直方体のxyz軸の辺の長さ
	float cubeSizeX = tree.m_fUnit_W;
	float cubeSizeY = tree.m_fUnit_H;
	float cubeSizeZ = tree.m_fUnit_D;

	// レイの傾きから一つノードを進めた時の次のノードまでの距離の増加量の定数「直方体のxyz軸の辺の長さ/|レイの成分|」を算出
	float dx = directionX != 0 ? cubeSizeX / fabsf(direction.x * dist) : 0.0f;
	float dy = directionY != 0 ? cubeSizeY / fabsf(direction.y * dist) : 0.0f;
	float dz = directionZ != 0 ? cubeSizeZ / fabsf(direction.z * dist) : 0.0f;

	// レイの始点が八分木の最小点を含むノードから数えて何個目のノードに居るか算出（intにキャスト前）
	float fnowX = (start.x - tree.m_fLeft) / cubeSizeX;
	float fnowY = (start.y - tree.m_fBottom) / cubeSizeY;
	float fnowZ = (start.z - tree.m_fFront) / cubeSizeZ;

	// レイの終点が八分木の最小点を含むノードから数えて何個目のノードに居るか算出（intにキャスト前）
	float fgoalX = (start.x + direction.x * dist - tree.m_fLeft) / cubeSizeX;
	float fgoalY = (start.y + direction.y * dist - tree.m_fBottom) / cubeSizeY;
	float fgoalZ = (start.z + direction.z * dist - tree.m_fFront) / cubeSizeZ;

	// 階層数分のループ処理。ルート空間からスタート
	for (uint32_t level = tree.m_uiLevel; level >= 0; level--)
	{
		// DDAを使ってレイで辿るノードを算出するための各変数の準備

		// レイの始点が八分木の最小点を含むノードから数えて何個目のノードに居るか算出（０個目スタート）
		uint32_t nowX = static_cast<uint32_t>(fnowX);
		uint32_t nowY = static_cast<uint32_t>(fnowY);
		uint32_t nowZ = static_cast<uint32_t>(fnowZ);

		// レイの終点が八分木の最小点を含むノードから数えて何個目のノードに居るか算出
		uint32_t goalX = static_cast<uint32_t>(fgoalX);
		uint32_t goalY = static_cast<uint32_t>(fgoalY);
		uint32_t goalZ = static_cast<uint32_t>(fgoalZ);

		// レイの始点のノードの最小・最大座標の算出
		float minX = tree.m_fLeft + nowX * cubeSizeX, maxX = minX + cubeSizeX;
		float minY = tree.m_fBottom + nowY * cubeSizeY, maxY = minY + cubeSizeY;
		float minZ = tree.m_fFront + nowZ * cubeSizeZ, maxZ = minZ + cubeSizeZ;

		// レイの始点において、レイが進む際、「次のノードにぶつかるまでの距離/|レイの成分|」を算出
		float distX = directionX * (maxX - start.x) + (1 - directionX) * cubeSizeX / 2;
		float distY = directionY * (maxY - start.y) + (1 - directionY) * cubeSizeY / 2;
		float distZ = directionZ * (maxZ - start.z) + (1 - directionZ) * cubeSizeZ / 2;

		float tx = directionX != 0 ? distX / fabsf(direction.x * dist) : FLT_MAX;
		float ty = directionY != 0 ? distY / fabsf(direction.y * dist) : FLT_MAX;
		float tz = directionZ != 0 ? distZ / fabsf(direction.z * dist) : FLT_MAX;

		// ループ処理で八分木の同一階層内のレイvs三角形の交差判定を行う
		while (1)
		{
			ret2 = false;

			// 線形（配列）のインデックスへ変換
			uint32_t linerIndex = mortonCode + (tree.m_iPow[level] - 1) / 7;

			// この空間が存在しないならbreakして隣の空間に移動
			if (!tree.ppCellAry[linerIndex])	break;

			Liner8TreeManager<Triangle>::OFT<Triangle>* oft = tree.ppCellAry[linerIndex]->pLatest;
			while (oft)
			{
				if (Collision::IntersectRayVsTriangle(start, direction, dist, oft->m_pObject->position, result))
				{
					hit = true;
				}

				oft = oft->m_pNext;
			}

			// 階層内に収まっているか確認
			if (linerIndex <= GetLevelStart(level + 1) - 1)
			{
				OctreeNode targetNode = m_octreeNodes[linerIndex];

				// 各ノードが持つ三角形全てとレイの当たり判定を行う
				for (int i = 0; i < targetNode.GetTriangles().size(); i++)
				{
					HitResultVector tmpResult;
					DirectX::XMVECTOR triangleVerts[3] = {
						DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[0]),
						DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[1]),
						DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[2])
					};
					if (Collision::IntersectRayVsTriangle(DirectX::XMLoadFloat3(&rayStart), DirectX::XMLoadFloat3(&rayDirection), triangleVerts, tmpResult))
					{
						if (hit.distance > tmpResult.distance)
						{
							hit.distance = tmpResult.distance;
							hit.normal = tmpResult.normal;
							hit.position = tmpResult.position;
							hit.triangleVerts[0] = tmpResult.triangleVerts[0];
							hit.triangleVerts[1] = tmpResult.triangleVerts[1];
							hit.triangleVerts[2] = tmpResult.triangleVerts[2];
							hit.materialIndex = targetNode.GetTriangles().at(i).materialIndex;
							ret = true;
						}
						ret2 = true;
					}
				}
			}
			else
			{
				// 階層外なら終了
				break;
			}
			// １度でもヒットしていたら、この階層のチェックは終了。（レイの開始点に近いノード順にチェックしているので）
			if (hit) break;

			// DDAを用いて、次にチェックする隣の区画を算出し、そのノードを示すモートンコードを上書きする
			// x方向
			if (tx <= ty && tx <= tz)
			{
				// 終端の空間まで来ていたら現在の階層の処理を終了させる
				if (nowX == goalX)	break;

				// 既に見つかっている最短交差距離よりも探索する空間までの距離が長くなっていれば、
				// これ以上進む必要はないため現在の階層の処理を終了させる
				if (result.distance < rayDist * tx)	break;

				// 移動するのでtxとnowXを更新する
				tx += dx;
				nowX += directionX;

				// 隣の空間のモートンコード（非線形）を算出し上書きする
				mortonCode = GetNextMortonCode(mortonCode, 0, directionX == -1);
			}
			// y方向
			else if (ty <= tx && ty <= tz)
			{
				// 終端の空間まで来ていたら処理を終了させる
				if (nowY == goalY)	break;

				// 既に見つかっている最短交差距離よりも探索する空間までの距離が長くなっていれば、
				// これ以上進む必要はないため現在の階層の処理を終了させる
				if (result.distance < rayDist * ty)	break;

				// 移動するのでtyとnowYを更新する
				ty += dy;
				nowY += directionY;

				// 隣の空間のモートンコード（非線形）を算出し上書きする
				mortonCode = GetNextMortonCode(mortonCode, 1, directionY == -1);
			}
			// z方向
			else
			{
				// 終端の空間まで来ていたら処理を終了させる
				if (nowZ == goalZ)	break;

				// 既に見つかっている最短交差距離よりも探索する空間までの距離が長くなっていれば、
				// これ以上進む必要はないため現在の階層の処理を終了させる
				if (result.distance < rayDist * tz)	break;

				// 移動するのでtzとnowZを更新する
				tz += dz;
				nowZ += directionZ;

				// 隣の空間のモートンコード（非線形）を算出し上書きする
				mortonCode = GetNextMortonCode(mortonCode, 2, directionZ == -1);
			}

		}
	}*/
}

// 垂直レイキャスト
bool MapTileManager::VerticalRayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	//HitResultVector result;
	//bool ret = quadtree.IntersectVerticalRayVsTriangle(start, end, result);

	//XMStoreFloat3(&hit.position, result.position);
	//XMStoreFloat3(&hit.normal, result.normal);
	//hit.distance = result.distance;
	//hit.materialIndex = result.materialIndex;
	//XMStoreFloat3(&hit.triangleVerts[0], result.triangleVerts[0]);
	//XMStoreFloat3(&hit.triangleVerts[1], result.triangleVerts[1]);
	//XMStoreFloat3(&hit.triangleVerts[2], result.triangleVerts[2]);

	//return ret;
	return false;
}

// 球の押し戻し
bool MapTileManager::IntersectSphereVsMap(Sphere& sphere, bool wallCheck)
{
	//XMFLOAT3 minPos, maxPos;
	//sphere.GetBoundPoints(&minPos, &maxPos);

	////球の空間配列番号
	//int Elem = tree.GetLinerIndex(minPos.x, maxPos.x, minPos.y, maxPos.y, minPos.z, maxPos.z);

	//DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&sphere.position);
	bool ret = false;
	//SearchChildren(Elem, position, sphere.radius, wallCheck, ret);
	//SearchParent(Elem, position, sphere.radius, wallCheck, ret);

	//if (!wallCheck)
	//{
	//	DirectX::XMStoreFloat3(&sphere.position, position);
	//}

	return ret;
}

// カプセルの押し戻し
bool MapTileManager::IntersectCapsuleVsMap(Capsule& capsule, bool wallCheck)
{
	//XMFLOAT3 minPos, maxPos;
	//capsule.GetBoundPoints(&minPos, &maxPos);

	////カプセルの空間配列番号
	//int Elem = tree.GetLinerIndex(minPos.x, maxPos.x, minPos.y, maxPos.y, minPos.z, maxPos.z);

	//DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&capsule.position);
	//DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&capsule.direction);
	//bool ret = false;
	//SearchChildren(Elem, position, direction, capsule.radius, capsule.length, wallCheck, ret);
	//SearchParent(Elem, position, direction, capsule.radius, capsule.length, wallCheck, ret);

	//if (!wallCheck)
	//{
	//	DirectX::XMStoreFloat3(&capsule.position, position);
	//}

	//return ret;

	return mapQuadtree.IntersectVsCapsule(capsule, wallCheck);
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

	//tree.Initialize(octDepth,
	//	c_minPos.x - 1.0f, c_maxPos.x + 1.0f,
	//	c_minPos.y - 20.0f, c_maxPos.y + 20.0f,
	//	c_minPos.z - 1.0f, c_maxPos.z + 1.0f);	// エリアを少し大きめに作成

	//XMFLOAT3 center = (c_minPos + c_maxPos) * 0.5f;
	//XMFLOAT3 size = c_maxPos - c_minPos;
	//float quadHalfSize = max(size.x, size.z) * 0.5f;
	//float octHalfSize = max(size.y * 0.5f, quadHalfSize);
	//quadHalfSize += 1.0f;
	//octHalfSize += 1.0f;

	//quadtree.CreateQuadtree(center, quadHalfSize, quadDepth);
	//octree.CreateOctree(center, octHalfSize, octDepth);
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

				//Triangle triangle = {};
				//DirectX::XMStoreFloat3(&triangle.position[0], A);
				//DirectX::XMStoreFloat3(&triangle.position[1], B);
				//DirectX::XMStoreFloat3(&triangle.position[2], C);
				//triangle.materialIndex = mesh.materialIndex;
				Triangle* p_triangle = new Triangle;
				DirectX::XMStoreFloat3(&p_triangle->position[0], A);
				DirectX::XMStoreFloat3(&p_triangle->position[1], B);
				DirectX::XMStoreFloat3(&p_triangle->position[2], C);

				// 三角形をワールド座標で登録
				//quadtree.InsertTriangleObject(triangle);
				//octree.InsertTriangleObject(triangle);

				//XMFLOAT3 minPos, maxPos;
				//triangle.GetBoundPoints(&minPos, &maxPos);
				//tree.Regist(minPos.x, maxPos.x, minPos.y, maxPos.y, minPos.z, maxPos.z, p_triangle);
				mapQuadtree.Regist(p_triangle);

				count++;
			}
			nowMeshNum++;
		}
	}

	return count;
}

bool MapTileManager::SearchChildren(	// 子空間探索
	int Elem,
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& direction,
	float dist,
	HitResult& result,
	bool& hit)
{
	// 空間外ならreturn
	if (Elem < 0) return hit;
	if (Elem > tree.m_iPow[tree.m_uiLevel]) return hit;

	// 子空間が存在するなら子空間を探索
	if ((Elem + 1) << 3 < tree.m_iCellNum)
	{
		int childElem = (Elem << 3) + 1;

		for (int i = 0; i < 8; i++)
		{
			if (SearchChildren(childElem + i, start, direction, dist, result, hit))
				break;
		}
	}

	// この空間が存在しないならreturn
	if (!tree.ppCellAry[Elem])	return hit;
	// この空間に登録されているポリゴンとの当たり判定
	Liner8TreeManager<Triangle>::OFT<Triangle>* oft = tree.ppCellAry[Elem]->pLatest;
	while (oft)
	{
		if (Collision::IntersectRayVsTriangle(start, direction, dist, oft->m_pObject->position, result))
		{
			hit = true;
		}
		
		oft = oft->m_pNext;
	}

	return hit;
}
bool MapTileManager::SearchChildren(
	int Elem,
	DirectX::XMVECTOR& spherePos,
	float radius,
	bool wallCheck,
	bool& hit)
{
	// 空間外ならreturn
	if (Elem < 0) return hit;

	// 子空間が存在するなら子空間を探索
	if ((Elem + 1) << 3 < tree.m_iCellNum)
	{
		int childElem = (Elem << 3) + 1;

		for (int i = 0; i < 8; i++)
		{
			if (SearchChildren(childElem + i, spherePos, radius, wallCheck, hit))
				break;
		}
	}

	// この空間が存在しないならreturn
	if (!tree.ppCellAry[Elem])	return hit;

	// この空間に登録されているポリゴンとの当たり判定
	Liner8TreeManager<Triangle>::OFT<Triangle>* oft = tree.ppCellAry[Elem]->pLatest;
	IntersectionResult result;
	
	while (oft)
	{
		DirectX::XMVECTOR triPos[3] =
		{
			DirectX::XMLoadFloat3(&oft->m_pObject->position[0]),
			DirectX::XMLoadFloat3(&oft->m_pObject->position[1]),
			DirectX::XMLoadFloat3(&oft->m_pObject->position[2])
		};

		if(wallCheck)
		{
			if (Collision::IntersectSphereVsTriangle(spherePos, radius, triPos, &result, true))
			{
				// wallCheckがtrueのときは壁に当たっている時true
				if (result.normal.m128_f32[1] < 0.4f)	// 66度以上の壁
				{
					hit = true;
				}
			}
		}
		else
		{
			if (Collision::IntersectSphereVsTriangle(spherePos, radius, triPos, &result, true))
			{
				// 球体のみ押し戻し処理
				spherePos = DirectX::XMVectorAdd(spherePos, DirectX::XMVectorScale(result.normal, result.penetration));
				hit = true;
			}
		}
		
		oft = oft->m_pNext;
	}

	return hit;
}
bool MapTileManager::SearchChildren(	// カプセル探索
	int Elem,
	DirectX::XMVECTOR& capsulePos,
	const DirectX::XMVECTOR& direction,
	float radius,
	float length,
	bool wallCheck,
	bool& hit)
{
	// 空間外ならreturn
	if (Elem < 0) return hit;

	// 子空間が存在するなら子空間を探索
	if ((Elem + 1) << 3 < tree.m_iCellNum)
	{
		int childElem = (Elem << 3) + 1;

		for (int i = 0; i < 8; i++)
		{
			if (SearchChildren(childElem + i, capsulePos, direction, radius, length, wallCheck, hit))
				break;
		}
	}

	// この空間が存在しないならreturn
	if (!tree.ppCellAry[Elem])	return hit;

	// この空間に登録されているポリゴンとの当たり判定
	Liner8TreeManager<Triangle>::OFT<Triangle>* oft = tree.ppCellAry[Elem]->pLatest;
	IntersectionResult result;

	while (oft)
	{
		DirectX::XMVECTOR triPos[3] =
		{
			DirectX::XMLoadFloat3(&oft->m_pObject->position[0]),
			DirectX::XMLoadFloat3(&oft->m_pObject->position[1]),
			DirectX::XMLoadFloat3(&oft->m_pObject->position[2])
		};

		if (wallCheck)
		{
			if (Collision::IntersectCapsuleVsTriangle(capsulePos, direction, radius, length, triPos, &result))
			{
				// wallCheckがtrueのときは壁に当たっている時true
				if (result.normal.m128_f32[1] < 0.4f)	// 66度以上の壁
				{
					hit = true;
				}
			}
		}
		else
		{
			if (Collision::IntersectCapsuleVsTriangle(capsulePos, direction, radius, length, triPos, &result))
			{
				// カプセルのみ押し戻し処理
				capsulePos = DirectX::XMVectorAdd(capsulePos, DirectX::XMVectorScale(result.normal, result.penetration));
				hit = true;
			}
		}

		oft = oft->m_pNext;
	}
	return hit;
}

bool MapTileManager::SearchParent(	// 親空間探索
	int Elem,
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& direction,
	float dist,
	HitResult& result,
	bool& hit)
{
	// 空間外ならreturn
	if (Elem <= 0) return hit;

	int parentElem = (Elem - 1) >> 3;
	while (1)
	{
		// この空間が存在しないなら親空間に移動
		if (!tree.ppCellAry[parentElem])
		{
			// ルート空間ならbreak
			if (parentElem == 0)	break;
			parentElem = (parentElem - 1) >> 3;
			continue;
		}
		Liner8TreeManager<Triangle>::OFT<Triangle>* oft = tree.ppCellAry[parentElem]->pLatest;
		while (oft)
		{
			if (Collision::IntersectRayVsTriangle(start, direction, dist, oft->m_pObject->position, result))
			{
				hit = true;
			}

			oft = oft->m_pNext;
		}

		// ルート空間ならbreak
		if (parentElem == 0)	break;

		// 親空間に移動
		parentElem = (parentElem - 1) >> 3;
	}

	return hit;
}
bool MapTileManager::SearchParent(	// 球探索
	int Elem,
	DirectX::XMVECTOR& spherePos,
	float radius,
	bool wallCheck,
	bool& hit)
{
	// 空間外ならreturn
	if (Elem <= 0) return hit;

	int parentElem = (Elem - 1) >> 3;
	while (1)
	{
		// この空間が存在しないなら親空間に移動
		if (!tree.ppCellAry[parentElem])
		{
			// ルート空間ならbreak
			if (parentElem == 0)	break;
			parentElem = (parentElem - 1) >> 3;
			continue;
		}

		Liner8TreeManager<Triangle>::OFT<Triangle>* oft = tree.ppCellAry[parentElem]->pLatest;
		IntersectionResult result;
		while (oft)
		{
			DirectX::XMVECTOR triPos[3] =
			{
				DirectX::XMLoadFloat3(&oft->m_pObject->position[0]),
				DirectX::XMLoadFloat3(&oft->m_pObject->position[1]),
				DirectX::XMLoadFloat3(&oft->m_pObject->position[2])
			};
			
			if (wallCheck)
			{
				if (Collision::IntersectSphereVsTriangle(spherePos, radius, triPos, &result, true))
				{
					// wallCheckがtrueのときは壁に当たっている時true
					if (result.normal.m128_f32[1] < 0.4f)	// 66度以上の壁
					{
						hit = true;
					}
				}
			}
			else
			{
				if (Collision::IntersectSphereVsTriangle(spherePos, radius, triPos, &result, true))
				{
					// 球体のみ押し戻し処理
					spherePos = DirectX::XMVectorAdd(spherePos, DirectX::XMVectorScale(result.normal, result.penetration));
					hit = true;
				}
			}

			oft = oft->m_pNext;
		}

		// ルート空間ならbreak
		if (parentElem == 0)	break;

		// 親空間に移動
		parentElem = (parentElem - 1) >> 3;
	}

	return hit;
}
bool MapTileManager::SearchParent(	// カプセル探索
	int Elem,
	DirectX::XMVECTOR& capsulePos,
	const DirectX::XMVECTOR& direction,
	float radius,
	float length,
	bool wallCheck,
	bool& hit)
{
	// 空間外ならreturn
	if (Elem <= 0) return hit;

	int parentElem = (Elem - 1) >> 3;
	while (1)
	{
		// この空間が存在しないなら親空間に移動
		if (!tree.ppCellAry[parentElem])
		{
			// ルート空間ならbreak
			if (parentElem == 0)	break;
			parentElem = (parentElem - 1) >> 3;
			continue;
		}

		Liner8TreeManager<Triangle>::OFT<Triangle>* oft = tree.ppCellAry[parentElem]->pLatest;
		IntersectionResult result;
		while (oft)
		{
			DirectX::XMVECTOR triPos[3] =
			{
				DirectX::XMLoadFloat3(&oft->m_pObject->position[0]),
				DirectX::XMLoadFloat3(&oft->m_pObject->position[1]),
				DirectX::XMLoadFloat3(&oft->m_pObject->position[2])
			};

			if (wallCheck)
			{
				if (Collision::IntersectCapsuleVsTriangle(capsulePos, direction, radius, length, triPos, &result))
				{
					// wallCheckがtrueのときは壁に当たっている時true
					if (result.normal.m128_f32[1] < 0.4f)	// 66度以上の壁
					{
						hit = true;
					}
				}
			}
			else
			{
				if (Collision::IntersectCapsuleVsTriangle(capsulePos, direction, radius, length, triPos, &result))
				{
					// カプセルのみ押し戻し処理
					capsulePos = DirectX::XMVectorAdd(capsulePos, DirectX::XMVectorScale(result.normal, result.penetration));
					hit = true;
				}
			}

			oft = oft->m_pNext;
		}

		// ルート空間ならbreak
		if (parentElem == 0)	break;

		// 親空間に移動
		parentElem = (parentElem - 1) >> 3;
	}
	return hit;
}

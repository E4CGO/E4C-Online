﻿//! @file OctreeNode.cpp
//! @note 八分木空間
#include "TAKOEngine/Physics/OctreeNode.h"

/**************************************************************************//**
	@brief		八分木空間の生成 インデックスの増加方向はｘ→y→ｚの順番
	@param[in]	XMFLOAT3 center : ルート空間の中心座標
				float halfSize : ルート空間の半辺長
				uint32_t depth : 空間分割数
	@return		なし
*//***************************************************************************/
void OctreeNodeManager::CreateOctree(DirectX::XMFLOAT3 center, float halfSize, uint32_t depth)
{
	// 最大階層数チェック
	if (depth > MAX_DEPTH)
	{
		depth = MAX_DEPTH;
	}

	// 階層数保存
	this->m_depth = depth;

	// ルート空間を登録
	OctreeNode tmp(center, halfSize);
	m_octreeNodes.push_back(tmp);

	// ルート以下の全てのノードを作成
	for (uint32_t level = 1; level <= depth; level++)
	{
		float nodeHalfSize = halfSize / (1 << level);
		tmp.SetHalfSize(nodeHalfSize);

		for (uint32_t linerIndex = GetLevelStart(level - 1); linerIndex < GetLevelStart(level); linerIndex++)
		{
			tmp.SetCenter({ m_octreeNodes[linerIndex].GetCenter().x - nodeHalfSize, m_octreeNodes[linerIndex].GetCenter().y - nodeHalfSize, m_octreeNodes[linerIndex].GetCenter().z - nodeHalfSize });
			m_octreeNodes.push_back(tmp);

			tmp.SetCenter({ m_octreeNodes[linerIndex].GetCenter().x + nodeHalfSize, m_octreeNodes[linerIndex].GetCenter().y - nodeHalfSize, m_octreeNodes[linerIndex].GetCenter().z - nodeHalfSize });
			m_octreeNodes.push_back(tmp);

			tmp.SetCenter({ m_octreeNodes[linerIndex].GetCenter().x - nodeHalfSize, m_octreeNodes[linerIndex].GetCenter().y + nodeHalfSize, m_octreeNodes[linerIndex].GetCenter().z - nodeHalfSize });
			m_octreeNodes.push_back(tmp);

			tmp.SetCenter({ m_octreeNodes[linerIndex].GetCenter().x + nodeHalfSize, m_octreeNodes[linerIndex].GetCenter().y + nodeHalfSize, m_octreeNodes[linerIndex].GetCenter().z - nodeHalfSize });
			m_octreeNodes.push_back(tmp);

			tmp.SetCenter({ m_octreeNodes[linerIndex].GetCenter().x - nodeHalfSize, m_octreeNodes[linerIndex].GetCenter().y - nodeHalfSize, m_octreeNodes[linerIndex].GetCenter().z + nodeHalfSize });
			m_octreeNodes.push_back(tmp);

			tmp.SetCenter({ m_octreeNodes[linerIndex].GetCenter().x + nodeHalfSize, m_octreeNodes[linerIndex].GetCenter().y - nodeHalfSize, m_octreeNodes[linerIndex].GetCenter().z + nodeHalfSize });
			m_octreeNodes.push_back(tmp);

			tmp.SetCenter({ m_octreeNodes[linerIndex].GetCenter().x - nodeHalfSize, m_octreeNodes[linerIndex].GetCenter().y + nodeHalfSize, m_octreeNodes[linerIndex].GetCenter().z + nodeHalfSize });
			m_octreeNodes.push_back(tmp);

			tmp.SetCenter({ m_octreeNodes[linerIndex].GetCenter().x + nodeHalfSize, m_octreeNodes[linerIndex].GetCenter().y + nodeHalfSize, m_octreeNodes[linerIndex].GetCenter().z + nodeHalfSize });
			m_octreeNodes.push_back(tmp);
		}
	}
}

/**************************************************************************//**
	@brief		モートンコード(あるの階層の空間番号)の生成
	@param[in]	XMFLOAT3 point : モートンコードを求めたい座標
				OctreeNode route : ルート空間
				float halfSize : その階層の半辺長
	@return		uint32_t
*//***************************************************************************/
uint32_t OctreeNodeManager::GetMortonCode(const DirectX::XMFLOAT3& point, const OctreeNode& route, float halfSize)
{
	float x = (point.x - (route.GetCenter().x - route.GetHalfSize())) / (2 * halfSize);
	float y = (point.y - (route.GetCenter().y - route.GetHalfSize())) / (2 * halfSize);
	float z = (point.z - (route.GetCenter().z - route.GetHalfSize())) / (2 * halfSize);
	return bitSeparete(static_cast<uint32_t>(x)) + (bitSeparete(static_cast<uint32_t>(y)) << 1) + (bitSeparete(static_cast<uint32_t>(z)) << 2);
}
uint32_t OctreeNodeManager::bitSeparete(uint32_t n)
{
	n = (n | n << 8) & 0x0000f00f;
	n = (n | n << 4) & 0x000c30c3;
	n = (n | n << 2) & 0x00249249;
	return n;
}

/**************************************************************************//**
	@brief		同階層の隣のモートンコードを取得
	@param[in]	uint32_t before : 元のモートンコード（非線形化の各階層のモートンコード）
				uint32_t shiftXYZ : どの軸の隣のモートンコードが欲しいか指定　x=0, y=1, z=2
				bool minus : マイナス方向に進む場合にture
	@return		int
*//***************************************************************************/
int OctreeNodeManager::GetNextMortonCode(uint32_t before, uint32_t shiftXYZ, bool minus)
{
	int ret = before;

	uint32_t filter = 0x01 << shiftXYZ;
	uint32_t value = 1 << shiftXYZ;

	// beforeがfilterより大きい間ループする
	while (before >= filter)
	{
		// フィルターをかけて値をチェックし、breakする
		if ((!(before & filter) && (!minus)) || ((before & filter) && minus))	break;

		// beforeを3bit右シフトさせて再チェック。併せてモートンコードの更新用の差分値のvalueも更新する
		before >>= 3;
		value = value * 8 - filter;
	}

	if (minus)
	{
		ret -= value;
	}
	else
	{
		ret += value;
	}

	return ret;
}

/**************************************************************************//**
	@brief		八分木空間へオブジェクトを登録する際の線形インデックスを取得
	@param[in]	XMFLOAT3 minPoint : AABBの最小点
				XMFLOAT3 maxPoint : AABBの最大点
	@return		uint32_t
*//***************************************************************************/
uint32_t OctreeNodeManager::GetLinerIndexInsertObject(DirectX::XMFLOAT3 minPoint, DirectX::XMFLOAT3 maxPoint)
{
	// AABBの最小点のモートンコード
	uint32_t mortonCodeMin = GetMortonCode(minPoint, m_octreeNodes.front(), m_octreeNodes.back().GetHalfSize());
	// AABBの最大点のモートンコード
	uint32_t mortonCodeMax = GetMortonCode(maxPoint, m_octreeNodes.front(), m_octreeNodes.back().GetHalfSize());
	// 上記二つのモートンコードの排他的論理和
	uint32_t mortonCodeXOR = mortonCodeMin ^ mortonCodeMax;

	// モートンコードの排他的論理和を使い、上位空間に登録するかのチェック
	uint32_t count = 1;
	uint32_t shift = 0;

	while (mortonCodeXOR != 0)
	{
		// 下位3bitずつマスクして値をチェックする
		if (mortonCodeXOR & 0x07)
		{
			// シフト数を上書き保存
			shift = count;
		}
		// 排他的論理和を3bitシフトさせて再チェック
		mortonCodeXOR >>= 3;

		count++;
	}

	// 線形（配列）のインデックスへ変換
	return  (mortonCodeMin >> (shift * 3)) + GetLevelStart(m_depth - shift);
}

/**************************************************************************//**
	@brief		八分木空間へ三角形オブジェクトを登録
	@param[in]	Triangle triangle : 登録する三角形
	@return		bool : 登録成功時true
*//***************************************************************************/
bool OctreeNodeManager::InsertTriangleObject(Triangle& triangle)
{
	// 三角形を含むAABBを構成する最小点と最大点を作成（y座標を除く）
	DirectX::XMFLOAT3 minPoint = triangle.position[0];
	DirectX::XMFLOAT3 maxPoint = triangle.position[0];

	if (minPoint.x > triangle.position[1].x)
	{
		minPoint.x = triangle.position[1].x;
	}
	else if (maxPoint.x < triangle.position[1].x)
	{
		maxPoint.x = triangle.position[1].x;
	}

	if (minPoint.x > triangle.position[2].x)
	{
		minPoint.x = triangle.position[2].x;
	}
	else if (maxPoint.x < triangle.position[2].x)
	{
		maxPoint.x = triangle.position[2].x;
	}

	if (minPoint.y > triangle.position[1].y)
	{
		minPoint.y = triangle.position[1].y;
	}
	else if (maxPoint.y < triangle.position[1].y)
	{
		maxPoint.y = triangle.position[1].y;
	}

	if (minPoint.y > triangle.position[2].y)
	{
		minPoint.y = triangle.position[2].y;
	}
	else if (maxPoint.y < triangle.position[2].y)
	{
		maxPoint.y = triangle.position[2].y;
	}

	if (minPoint.z > triangle.position[1].z)
	{
		minPoint.z = triangle.position[1].z;
	}
	else if (maxPoint.z < triangle.position[1].z)
	{
		maxPoint.z = triangle.position[1].z;
	}

	if (minPoint.z > triangle.position[2].z)
	{
		minPoint.z = triangle.position[2].z;
	}
	else if (maxPoint.z < triangle.position[2].z)
	{
		maxPoint.z = triangle.position[2].z;
	}

	// 最小点・最大点が八分木のルート空間の外にある場合、登録失敗
	DirectX::XMFLOAT3 routeMinPoint = m_octreeNodes.front().GetCenter();
	routeMinPoint.x -= m_octreeNodes.front().GetHalfSize();
	routeMinPoint.y -= m_octreeNodes.front().GetHalfSize();
	routeMinPoint.z -= m_octreeNodes.front().GetHalfSize();

	DirectX::XMFLOAT3 routeMaxPoint = m_octreeNodes.front().GetCenter();
	routeMaxPoint.x += m_octreeNodes.front().GetHalfSize();
	routeMaxPoint.y += m_octreeNodes.front().GetHalfSize();
	routeMaxPoint.z += m_octreeNodes.front().GetHalfSize();

	if (minPoint.x < routeMinPoint.x)	return false;
	if (minPoint.y < routeMinPoint.y)	return false;
	if (minPoint.z < routeMinPoint.z)	return false;
	if (maxPoint.x > routeMaxPoint.x)	return false;
	if (maxPoint.y > routeMaxPoint.y)	return false;
	if (maxPoint.z > routeMaxPoint.z)	return false;

	// 線形（配列）のインデックスへ変換
	int linerIndex = GetLinerIndexInsertObject(minPoint, maxPoint);
	if (linerIndex >= m_octreeNodes.size()) return false;

	// 登録
	m_octreeNodes[linerIndex].InsertTriangleObject(triangle);

	return true;
}

/**************************************************************************//**
	@brief		八分木空間へ球体オブジェクトを登録
	@param[in]	Sphere sphere : 登録する球体
	@return		bool : 登録成功時true
*//***************************************************************************/
bool OctreeNodeManager::InsertSphereObject(Sphere& sphere)
{
	// 球を含むAABBを構成する最小点と最大点を作成
	DirectX::XMFLOAT3 minPoint = {};
	DirectX::XMFLOAT3 maxPoint = {};
	sphere.GetBoundPoints(&minPoint, &maxPoint);

	// 最小点・最大点が八分木のルート空間の外にある場合、登録失敗
	DirectX::XMFLOAT3 routeMinPoint = m_octreeNodes.front().GetCenter();
	routeMinPoint.x -= m_octreeNodes.front().GetHalfSize();
	routeMinPoint.y -= m_octreeNodes.front().GetHalfSize();
	routeMinPoint.z -= m_octreeNodes.front().GetHalfSize();

	DirectX::XMFLOAT3 routeMaxPoint = m_octreeNodes.front().GetCenter();
	routeMaxPoint.x += m_octreeNodes.front().GetHalfSize();
	routeMaxPoint.y += m_octreeNodes.front().GetHalfSize();
	routeMaxPoint.z += m_octreeNodes.front().GetHalfSize();

	if (minPoint.x < routeMinPoint.x)	return false;
	if (minPoint.y < routeMinPoint.y)	return false;
	if (minPoint.z < routeMinPoint.z)	return false;
	if (maxPoint.x > routeMaxPoint.x)	return false;
	if (maxPoint.y > routeMaxPoint.y)	return false;
	if (maxPoint.z > routeMaxPoint.z)	return false;

	// 線形（配列）のインデックスへ変換
	int linerIndex = GetLinerIndexInsertObject(minPoint, maxPoint);
	if (linerIndex >= m_octreeNodes.size()) return false;

	// 登録
	m_octreeNodes[linerIndex].InsertSphereObject(sphere);

	return true;
}

/**************************************************************************//**
	@brief		八分木空間へAABBオブジェクトを登録
	@param[in]	AABB aabb : 登録するAABB
	@return		bool : 登録成功時true
*//***************************************************************************/
bool OctreeNodeManager::InsertAABBObject(AABB& aabb)
{
	// AABBを構成する最小点と最大点を作成（y座標を除く）
	DirectX::XMFLOAT3 minPoint = { aabb.position.x - aabb.radii.x, 0.0f, aabb.position.z - aabb.radii.z };
	DirectX::XMFLOAT3 maxPoint = { aabb.position.x + aabb.radii.x, 0.0f, aabb.position.z + aabb.radii.z };

	// 最小点・最大点が八分木のルート空間の外にある場合、登録失敗
	DirectX::XMFLOAT3 routeMinPoint = m_octreeNodes.front().GetCenter();
	routeMinPoint.x -= m_octreeNodes.front().GetHalfSize();
	routeMinPoint.y -= m_octreeNodes.front().GetHalfSize();
	routeMinPoint.z -= m_octreeNodes.front().GetHalfSize();

	DirectX::XMFLOAT3 routeMaxPoint = m_octreeNodes.front().GetCenter();
	routeMaxPoint.x += m_octreeNodes.front().GetHalfSize();
	routeMaxPoint.y += m_octreeNodes.front().GetHalfSize();
	routeMaxPoint.z += m_octreeNodes.front().GetHalfSize();

	if (minPoint.x < routeMinPoint.x)	return false;
	if (minPoint.y < routeMinPoint.y)	return false;
	if (minPoint.z < routeMinPoint.z)	return false;
	if (maxPoint.x > routeMaxPoint.x)	return false;
	if (maxPoint.y > routeMaxPoint.y)	return false;
	if (maxPoint.z > routeMaxPoint.z)	return false;

	// 線形（配列）のインデックスへ変換
	int linerIndex = GetLinerIndexInsertObject(minPoint, maxPoint);
	if (linerIndex >= m_octreeNodes.size()) return false;

	// 登録
	m_octreeNodes[linerIndex].InsertAABBObject(aabb);

	return true;
}

/**************************************************************************//**
	@brief		八分木空間へカプセルオブジェクトを登録
	@param[in]	Capsule capsule : 登録するカプセル
	@return		bool : 登録成功時true
*//***************************************************************************/
bool OctreeNodeManager::InsertCapsuleObject(Capsule& capsule)
{
	// カプセルを含むAABBを構成する最小点と最大点を作成（y座標を除く）
	DirectX::XMFLOAT3 minPoint = {};
	DirectX::XMFLOAT3 maxPoint = {};
	capsule.GetBoundPoints(&minPoint, &maxPoint);

	// 最小点・最大点が八分木のルート空間の外にある場合、登録失敗
	DirectX::XMFLOAT3 routeMinPoint = m_octreeNodes.front().GetCenter();
	routeMinPoint.x -= m_octreeNodes.front().GetHalfSize();
	routeMinPoint.y -= m_octreeNodes.front().GetHalfSize();
	routeMinPoint.z -= m_octreeNodes.front().GetHalfSize();

	DirectX::XMFLOAT3 routeMaxPoint = m_octreeNodes.front().GetCenter();
	routeMaxPoint.x += m_octreeNodes.front().GetHalfSize();
	routeMaxPoint.y += m_octreeNodes.front().GetHalfSize();
	routeMaxPoint.z += m_octreeNodes.front().GetHalfSize();

	if (minPoint.x < routeMinPoint.x)	return false;
	if (minPoint.y < routeMinPoint.y)	return false;
	if (minPoint.z < routeMinPoint.z)	return false;
	if (maxPoint.x > routeMaxPoint.x)	return false;
	if (maxPoint.y > routeMaxPoint.y)	return false;
	if (maxPoint.z > routeMaxPoint.z)	return false;

	// 線形（配列）のインデックスへ変換
	int linerIndex = GetLinerIndexInsertObject(minPoint, maxPoint);
	if (linerIndex >= m_octreeNodes.size()) return false;

	// 登録
	m_octreeNodes[linerIndex].InsertCapsuleObject(capsule);

	return true;
}

/**************************************************************************//**
	@brief		オブジェクトを八分木空間から削除
	@param[in]	なし
	@return		なし
*//***************************************************************************/
// 全ての三角形オブジェクトを八分木空間から削除
void OctreeNodeManager::ClearAllTriangleObject()
{
	for (auto it = m_octreeNodes.begin(); it != m_octreeNodes.end(); it++)
	{
		it->ClearTriangleObject();
	}
}
// 全ての球体オブジェクトを八分木空間から削除
void OctreeNodeManager::ClearAllSphereObject()
{
	for (auto it = m_octreeNodes.begin(); it != m_octreeNodes.end(); it++)
	{
		it->ClearSphereObject();
	}
}
// 全てのAABBオブジェクトを八分木空間から削除
void OctreeNodeManager::ClearAllAABBObject()
{
	for (auto it = m_octreeNodes.begin(); it != m_octreeNodes.end(); it++)
	{
		it->ClearAABBObject();
	}
}
// 全てのカプセルオブジェクトを八分木空間から削除
void OctreeNodeManager::ClearAllCapsuleObject()
{
	for (auto it = m_octreeNodes.begin(); it != m_octreeNodes.end(); it++)
	{
		it->ClearCapsuleObject();
	}
}

/**************************************************************************//**
	@brief		全てのノードで三角形と垂直レイの交差判定を行う（最初に交差する三角形のみを返す方式）
	@param[in]	XMFLOAT3 rayStart : レイの始点
				XMFLOAT3 rayEnd : レイの終点
				HitResultVector result : ヒット結果記録用変数
	@return		bool : 交差するときtrue
*//***************************************************************************/
bool OctreeNodeManager::IntersectVerticalRayVsTriangle(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd, HitResultVector& result)
{
	// 八分木空間の最小値
	DirectX::XMFLOAT3 routeMinPoint = m_octreeNodes.front().GetCenter();
	routeMinPoint.x -= m_octreeNodes.front().GetHalfSize();
	routeMinPoint.y -= m_octreeNodes.front().GetHalfSize();
	routeMinPoint.z -= m_octreeNodes.front().GetHalfSize();
	// 制御用変数
	bool ret = false;	// 全体の交差結果。一度でも三角形との交差が出ればtrue
	bool hit = false;	// 階層ごとの交差結果。一度でも三角形の交差が出ればtrue

	// レイの向きを用意し、併せてレイの長さでヒット情報の距離を初期化
	DirectX::XMFLOAT3 rayDirection = { 0.0f, rayEnd.y - rayStart.y , 0.0f };
	result.distance = fabsf(rayDirection.y);

	// y軸のレイの向きをプラス→１、ゼロ→０、マイナス→－１で記録
	int directionY = rayDirection.y > 0.0f ? 1 : (rayDirection.y < 0.0f ? -1 : 0);

	// 八分木の階層数分ループ処理
	for (int level = m_depth; level >= 0; level--)
	{
		// 現在の階層の線形インデックスを取得
		int levelStart = GetLevelStart(level);

		// レイの始点のモートンコードを算出
		uint32_t mortonCode = GetMortonCode(rayStart, m_octreeNodes.front(), m_octreeNodes[levelStart].GetHalfSize());

		// DDAを使ってレイで辿るノードを算出するための各変数の準備

		// 空間の直方体のy軸の辺の長さ
		float cubeSize = m_octreeNodes[levelStart].GetHalfSize() * 2;

		// レイの始点が八分木の最小点を含むノードから数えて何個目のノードに居るか算出（０個目スタート）
		uint32_t nowY = static_cast<uint32_t>((rayStart.y - routeMinPoint.y) / cubeSize);

		// レイの終点が八分木の最小点を含むノードから数えて何個目のノードに居るか算出
		uint32_t goalY = static_cast<uint32_t>((rayEnd.y - routeMinPoint.y) / cubeSize);

		// レイの始点のノードの最小・最大座標の算出
		float minY = routeMinPoint.y + nowY * cubeSize, maxY = minY + cubeSize;

		// レイの始点において、レイが進む際、次のノードにぶつかるまでの距離を算出
		float distY = directionY * (maxY - rayStart.y) + (1 - directionY) * cubeSize / 2;

		// ループ処理で八分木の同一階層内のレイvs三角形の交差判定を行う
		while (1)
		{
			hit = false;

			// 線形（配列）のインデックスへ変換
			uint32_t linerIndex = mortonCode + levelStart;

			// 階層内に収まっているか確認
			if (linerIndex <= GetLevelStart(level + 1) - 1)
			{
				OctreeNode targetNode = m_octreeNodes.at(linerIndex);

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
						if (result.distance > tmpResult.distance)
						{
							result.distance = tmpResult.distance;
							result.normal = tmpResult.normal;
							result.position = tmpResult.position;
							result.triangleVerts[0] = tmpResult.triangleVerts[0];
							result.triangleVerts[1] = tmpResult.triangleVerts[1];
							result.triangleVerts[2] = tmpResult.triangleVerts[2];
							result.materialIndex = targetNode.GetTriangles().at(i).materialIndex;
							ret = true;
						}
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

			// 終端の空間まで来ていたら処理を終了させる
			if (nowY == goalY)	break;

			// 既に見つかっている最短交差距離よりも探索する空間までの距離が長くなっていれば、
			// これ以上進む必要はないため現在の階層の処理を終了させる
			if (result.distance < distY)	break;

			// 移動するのでdistYとnowYを更新する
			distY += cubeSize;
			nowY += directionY;

			// 隣の空間のモートンコード（非線形）を算出し上書きする
			mortonCode = GetNextMortonCode(mortonCode, 1, directionY == -1);
		}
	}

	return ret;
}

/**************************************************************************//**
	@brief		全てのノードで三角形とレイの交差判定を行う（最初に交差する三角形のみを返す方式）
	@param[in]	XMFLOAT3 rayStart : レイの始点
				XMFLOAT3 rayEnd : レイの終点
				HitResultVector result : ヒット結果記録用変数
	@return		bool : 交差するときtrue
*//***************************************************************************/
bool OctreeNodeManager::IntersectRayVsTriangle(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd, HitResultVector& result)
{
	// 四分木空間の最小値（x,z座標）
	DirectX::XMFLOAT3 routeMinPoint = m_octreeNodes.front().GetCenter();
	routeMinPoint.x -= m_octreeNodes.front().GetHalfSize();
	routeMinPoint.y -= m_octreeNodes.front().GetHalfSize();
	routeMinPoint.z -= m_octreeNodes.front().GetHalfSize();
	// 制御用変数
	bool ret = false;	// 全体の交差結果。一度でも三角形との交差が出ればtrue
	bool hit = false;	// 階層ごとの交差結果。一度でも三角形の交差が出ればtrue

	// レイの向きを用意し、併せてレイの長さでヒット情報の距離を初期化
	DirectX::XMFLOAT3 rayDirection = { rayEnd.x - rayStart.x, rayEnd.y - rayStart.y, rayEnd.z - rayStart.z };
	float rayDist = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&rayDirection)));
	result.distance = rayDist;

	// xz軸それぞれのレイの向きをプラス→１、ゼロ→０、マイナス→－１で記録
	int directionX = rayDirection.x > 0.0f ? 1 : (rayDirection.x < 0.0f ? -1 : 0);
	int directionY = rayDirection.y > 0.0f ? 1 : (rayDirection.y < 0.0f ? -1 : 0);
	int directionZ = rayDirection.z > 0.0f ? 1 : (rayDirection.z < 0.0f ? -1 : 0);


	// 階層数分のループ処理。ルート空間からスタート
	for (uint32_t level = 0; level <= m_depth; level++)
	{
		// 現在の階層の線形インデックスを取得
		int levelStart = GetLevelStart(level);

		// レイの始点のモートンコードを算出
		uint32_t mortonCode = GetMortonCode(rayStart, m_octreeNodes.front(), m_octreeNodes[levelStart].GetHalfSize());

		// DDAを使ってレイで辿るノードを算出するための各変数の準備

		// 空間の直方体のxz軸の辺の長さ
		float cubeSize = m_octreeNodes[levelStart].GetHalfSize() * 2;

		// レイの傾きから一つノードを進めた時の次のノードまでの距離の増加量の定数「直方体のxyz軸の辺の長さ/|レイの成分|」を算出
		float dx = directionX != 0 ? cubeSize / fabsf(rayDirection.x) : 0.0f;
		float dy = directionY != 0 ? cubeSize / fabsf(rayDirection.y) : 0.0f;
		float dz = directionZ != 0 ? cubeSize / fabsf(rayDirection.z) : 0.0f;

		// レイの始点が四分木の最小点を含むノードから数えて何個目のノードに居るか算出（０個目スタート）
		uint32_t nowX = static_cast<uint32_t>((rayStart.x - routeMinPoint.x) / cubeSize);
		uint32_t nowY = static_cast<uint32_t>((rayStart.y - routeMinPoint.y) / cubeSize);
		uint32_t nowZ = static_cast<uint32_t>((rayStart.z - routeMinPoint.z) / cubeSize);

		// レイの終点が四分木の最小点を含むノードから数えて何個目のノードに居るか算出
		uint32_t goalX = static_cast<uint32_t>((rayEnd.x - routeMinPoint.x) / cubeSize);
		uint32_t goalY = static_cast<uint32_t>((rayEnd.y - routeMinPoint.y) / cubeSize);
		uint32_t goalZ = static_cast<uint32_t>((rayEnd.z - routeMinPoint.z) / cubeSize);

		// レイの始点のノードの最小・最大座標の算出
		float minX = routeMinPoint.x + nowX * cubeSize, maxX = minX + cubeSize;
		float minY = routeMinPoint.y + nowY * cubeSize, maxY = minY + cubeSize;
		float minZ = routeMinPoint.z + nowZ * cubeSize, maxZ = minZ + cubeSize;

		// レイの始点において、レイが進む際、「次のノードにぶつかるまでの距離/|レイの成分|」を算出
		float distX = directionX * (maxX - rayStart.x) + (1 - directionX) * cubeSize / 2;
		float distY = directionY * (maxY - rayStart.y) + (1 - directionY) * cubeSize / 2;
		float distZ = directionZ * (maxZ - rayStart.z) + (1 - directionZ) * cubeSize / 2;

		float tx = directionX != 0 ? distX / fabsf(rayDirection.x) : FLT_MAX;
		float ty = directionY != 0 ? distY / fabsf(rayDirection.y) : FLT_MAX;
		float tz = directionZ != 0 ? distZ / fabsf(rayDirection.z) : FLT_MAX;


		// ループ処理で四分木の同一階層内のレイvs三角形の交差判定を行う
		while (1)
		{
			hit = false;

			// 線形（配列）のインデックスへ変換
			uint32_t linerIndex = mortonCode + levelStart;

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
						if (result.distance > tmpResult.distance)
						{
							result.distance = tmpResult.distance;
							result.normal = tmpResult.normal;
							result.position = tmpResult.position;
							result.triangleVerts[0] = tmpResult.triangleVerts[0];
							result.triangleVerts[1] = tmpResult.triangleVerts[1];
							result.triangleVerts[2] = tmpResult.triangleVerts[2];
							result.materialIndex = targetNode.GetTriangles().at(i).materialIndex;
							ret = true;
						}
						hit = true;
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
	}

	return ret;
}

/**************************************************************************//**
	@brief		全てのノードで三角形とスフィアキャストの交差判定を行う（最初に交差する三角形のみを返す方式）
	@param[in]	XMFLOAT3 rayStart : スフィアキャストの始点
				XMFLOAT3 rayEnd : スフィアキャストの終点
				float radius : スフィアの半径
				HitResultVector result : ヒット結果記録用変数
				vector<CrossedNode>* nodeList : 交差したノードリスト
	@return		bool : 交差するときtrue
*//***************************************************************************/
bool OctreeNodeManager::IntersectSphereCastVsTriangle(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd, float radius, HitResultVector& result)
{
	std::vector<OctreeNodeManager::CrossedNode>	mortonList;	// 交差したノードを記録する
	return IntersectSphereCastVsTriangle(rayStart, rayEnd, radius, result, &mortonList);
}
bool OctreeNodeManager::IntersectSphereCastVsTriangle(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd, float radius, HitResultVector& result, std::vector<CrossedNode>* nodeList)
{
	// 八分木の最下層の辺長よりもスフィアキャストの半径が大きい場合、今回のアルゴリズムでは判定出来ないため、最下層を一つ上の階層に移動させる
	float minSize = m_octreeNodes.back().GetHalfSize() * 2.0f;
	uint32_t targetDepth = m_depth;
	while (minSize < radius)
	{
		minSize *= 2.0f;
		targetDepth--;
	}

	// 制御用変数
	bool ret = false;			// 全体の交差結果。一度でも三角形との交差が出ればtrue
	bool hitNowLevel = false;	// 現在の階層での交差結果。一度でも三角形との交差が出ればtrue
	bool hitRayCast = false;	// 単一のレイキャストでの交差結果。一度でも三角形との交差が出ればtrue

	uint32_t level = 0;

	// レイの長さでヒット情報の距離を初期化し、正規化もしておく
	DirectX::XMFLOAT3 rayDirection =
	{
		rayEnd.x - rayStart.x,
		rayEnd.y - rayStart.y,
		rayEnd.z - rayStart.z,
	};
	float rayDist = sqrtf(rayDirection.x * rayDirection.x + rayDirection.z * rayDirection.z);
	rayDirection.x /= rayDist;
	rayDirection.y /= rayDist;
	rayDirection.z /= rayDist;
	result.distance = rayDist;
	// 横方向のレイの成分の大きさ
	float horizontalRayDirection = sqrtf(rayDirection.x * rayDirection.x + rayDirection.z * rayDirection.z);

	// 八分木空間の最小値（x,z座標）
	DirectX::XMFLOAT3 routeMinPoint = m_octreeNodes.front().GetCenter();
	routeMinPoint.x -= m_octreeNodes.front().GetHalfSize();
	routeMinPoint.y -= m_octreeNodes.front().GetHalfSize();
	routeMinPoint.z -= m_octreeNodes.front().GetHalfSize();

	// レイの向きを１・０・－１で記録
	int directionX = rayDirection.x > 0.0f ? 1 : (rayDirection.x < 0.0f ? -1 : 0);
	int directionY = rayDirection.y > 0.0f ? 1 : (rayDirection.y < 0.0f ? -1 : 0);
	int directionZ = rayDirection.z > 0.0f ? 1 : (rayDirection.z < 0.0f ? -1 : 0);

	// ノードを指定し、そのノード内の全ての三角形とスフィアキャストを行うラムダ式
	auto IntersectShpereCastVsTriangleInTargetNode = [&](int linerIndex)
	{
		OctreeNode targetNode = m_octreeNodes[linerIndex];

		// 各ノードが持つ三角形全てとレイの当たり判定を行う
		for (int i = 0; i < targetNode.GetTriangles().size(); i++)
		{
			HitResult tmpResult;
			DirectX::XMVECTOR triangleVerts[3] = {
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[0]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[1]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[2])
			};
			if (Collision::IntersectSphereCastVsTriangle(DirectX::XMLoadFloat3(&rayStart), DirectX::XMLoadFloat3(&rayDirection), rayDist, radius, triangleVerts, &tmpResult, true))
			{
				if (result.distance > tmpResult.distance)
				{
					result.distance = tmpResult.distance;
					result.normal = DirectX::XMLoadFloat3(&tmpResult.normal);
					result.position = DirectX::XMLoadFloat3(&tmpResult.position);
					result.triangleVerts[0] = DirectX::XMLoadFloat3(&tmpResult.triangleVerts[0]);
					result.triangleVerts[1] = DirectX::XMLoadFloat3(&tmpResult.triangleVerts[1]);
					result.triangleVerts[2] = DirectX::XMLoadFloat3(&tmpResult.triangleVerts[2]);
					result.materialIndex = targetNode.GetTriangles().at(i).materialIndex;
					ret = true;
				}
				hitRayCast = true;
				hitNowLevel = true;
			}
		}
	};
	// 既に登録済みのインデックスか確認してから、上記ラムダ式を呼び出すラムダ式。これだけでOK
	auto resisterAndIntersectShpereCastVsTriangleInTargetNode = [&](uint32_t linerIndex)
	{
		if (linerIndex <= GetLevelStart(level + 1) - 1)
		{
			if (!std::any_of(nodeList->begin(), nodeList->end(),
				[linerIndex](const CrossedNode& data) { return data.m_linerIndex == linerIndex; }))
			{
				CrossedNode mortonData;
				mortonData.m_linerIndex = linerIndex;
				mortonData.m_level = level;
				nodeList->emplace_back(mortonData);
				IntersectShpereCastVsTriangleInTargetNode(linerIndex);
			}
		}
		else
		{
			return false;
		}

		return true;
	};

	// 階層（level）分のループ
	for (level = 0; level <= targetDepth; level++)
	{
		hitNowLevel = false;

		uint32_t levelStart = GetLevelStart(level);

		// レイで辿るノードを算出するための各変数の準備
		float cubeSize = m_octreeNodes[levelStart].GetHalfSize() * 2;

		// レイの傾きから一つノードを進めた時の次のノードまでの距離の増加量の定数「直方体のxz軸の辺の長さ/|レイの成分|」を算出
		float dx = directionX != 0 ? cubeSize / fabsf(rayDirection.x * rayDist) : 0.0f;
		float dy = directionY != 0 ? cubeSize / fabsf(rayDirection.y * rayDist) : 0.0f;
		float dz = directionZ != 0 ? cubeSize / fabsf(rayDirection.z * rayDist) : 0.0f;

		// レイの始点の交差判定
		uint32_t mortonCode = GetMortonCode(rayStart, m_octreeNodes.front(), m_octreeNodes[levelStart].GetHalfSize());
		if (!resisterAndIntersectShpereCastVsTriangleInTargetNode(mortonCode + levelStart))
		{
			continue;
		}

		// レイの座標補正値
		DirectX::XMFLOAT3 fixAddVec = {};
		// 中心、左、右、上、下からのレイと八分木の交差判定
		for (int i = 0; i < 5; i++)
		{
			// レイの座標補正値を算出
			if (i == 0)	// 中心
			{
				fixAddVec = { rayDirection.x * radius, rayDirection.y * radius, rayDirection.z * radius };
			}
			else if (i == 1)	// 左
			{
				fixAddVec = { rayDirection.z * radius, 0.0f, -rayDirection.x * radius };
			}
			else if (i == 2)	// 右
			{
				fixAddVec = { -rayDirection.z * radius, 0.0f, rayDirection.x * radius };
			}
			else if (i == 3)	// 上
			{
				fixAddVec = { -rayDirection.x / horizontalRayDirection * rayDirection.y * radius, horizontalRayDirection * radius, -rayDirection.z / horizontalRayDirection * rayDirection.y * radius };
			}
			else if (i == 4)	// 下
			{
				fixAddVec = { rayDirection.x / horizontalRayDirection * rayDirection.y * radius, -horizontalRayDirection * radius, rayDirection.z / horizontalRayDirection * rayDirection.y * radius };
			}

			// 座標補正後のレイの始点のモートンコード
			mortonCode = GetMortonCode(DirectX::XMFLOAT3{ rayStart.x + fixAddVec.x, rayStart.y + fixAddVec.y, rayStart.z + fixAddVec.z }, m_octreeNodes.front(), m_octreeNodes[levelStart].GetHalfSize());

			// レイの始点が八分木の最小点を含むノードから数えて何個目のノードに居るか算出（０個目スタート）
			uint32_t nowX = static_cast<uint32_t>((rayStart.x + fixAddVec.x - routeMinPoint.x) / cubeSize);
			uint32_t nowY = static_cast<uint32_t>((rayStart.y + fixAddVec.y - routeMinPoint.y) / cubeSize);
			uint32_t nowZ = static_cast<uint32_t>((rayStart.z + fixAddVec.z - routeMinPoint.z) / cubeSize);
			// レイの終点が八分木の最小点を含むノードから数えて何個目のノードに居るか算出
			uint32_t goalX = static_cast<uint32_t>((rayEnd.x + fixAddVec.x - routeMinPoint.x) / cubeSize);
			uint32_t goalY = static_cast<uint32_t>((rayEnd.y + fixAddVec.y - routeMinPoint.y) / cubeSize);
			uint32_t goalZ = static_cast<uint32_t>((rayEnd.z + fixAddVec.z - routeMinPoint.z) / cubeSize);
			// レイの始点のノードの最小・最大座標の算出
			float minX = routeMinPoint.x + nowX * cubeSize, maxX = minX + cubeSize;
			float minY = routeMinPoint.y + nowY * cubeSize, maxY = minY + cubeSize;
			float minZ = routeMinPoint.z + nowZ * cubeSize, maxZ = minZ + cubeSize;
			// レイが進む際、次のノードにぶつかるまでの距離（レイの比率）を算出
			float distX = directionX * (maxX - (rayStart.x + fixAddVec.x)) + (1 - directionX) * cubeSize / 2;
			float distY = directionY * (maxY - (rayStart.y + fixAddVec.y)) + (1 - directionY) * cubeSize / 2;
			float distZ = directionZ * (maxZ - (rayStart.z + fixAddVec.z)) + (1 - directionZ) * cubeSize / 2;

			float tx = directionX != 0 ? distX / fabsf(rayDirection.x * rayDist) : FLT_MAX;
			float ty = directionY != 0 ? distY / fabsf(rayDirection.y * rayDist) : FLT_MAX;
			float tz = directionZ != 0 ? distZ / fabsf(rayDirection.z * rayDist) : FLT_MAX;

			// ループ処理で四分木の同一階層内のスフィアキャストvs三角形の交差判定を行う
			while (1)
			{
				hitRayCast = false;

				// 登録と交差判定処理
				if (!resisterAndIntersectShpereCastVsTriangleInTargetNode(mortonCode + levelStart))
				{
					break;
				}

				// １度でもヒットしていたら、この階層のチェックは終了。（レイの開始点に近いノード順にチェックしているので）
				if (hitRayCast) break;

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
		}

		// ここまでで交差がなければ、スフィアキャストの終端の球と周囲の空間の判定を行う
		if (!hitNowLevel)
		{
			uint32_t mortonCode = GetMortonCode(rayEnd, m_octreeNodes.front(), m_octreeNodes[levelStart].GetHalfSize());
			uint32_t tmpMortonCode = mortonCode;
			// レイの終点が八分木の最小点を含むノードから数えて何個目のノードに居るか算出
			uint32_t endX = static_cast<uint32_t>((rayEnd.x - routeMinPoint.x) / cubeSize);
			uint32_t endY = static_cast<uint32_t>((rayEnd.y - routeMinPoint.y) / cubeSize);
			uint32_t endZ = static_cast<uint32_t>((rayEnd.z - routeMinPoint.z) / cubeSize);
			// レイの終点のノードの最小・最大座標の算出
			float minX = routeMinPoint.x + endX * cubeSize, maxX = minX + cubeSize;
			float minY = routeMinPoint.y + endY * cubeSize, maxY = minY + cubeSize;
			float minZ = routeMinPoint.z + endZ * cubeSize, maxZ = minZ + cubeSize;

			// 現在の階層の1辺の分割数
			uint32_t divisions = 1 << level;

			// 26方向の隣接する空間をチェックする

			// 隣の空間までの距離
			float distLeft = rayEnd.x - minX;
			float distRight = maxX - rayEnd.x;
			float distDown = rayEnd.y - minY;
			float distUp = maxY - rayEnd.y;
			float distFront = rayEnd.z - minZ;
			float distBack = maxZ - rayEnd.z;
			float sqDist[6] = { distLeft * distLeft ,distRight * distRight, distDown * distDown, distUp * distUp, distFront * distFront, distBack * distBack };
			float sqRadius = radius * radius;
			
			// 隣の空間(1次元方向)のモートンコードの変化量
			int32_t nextMortonCode[6] = {};
			
			if (distLeft < radius && endX > 0)
			{
				nextMortonCode[0] = GetNextMortonCode(mortonCode, 0, true) - mortonCode;
			}
			if (distRight < radius && endX < divisions)
			{
				nextMortonCode[1] = GetNextMortonCode(mortonCode, 0, false) - mortonCode;
			}
			if (distDown < radius && endY > 0)
			{
				nextMortonCode[2] = GetNextMortonCode(mortonCode, 1, true) - mortonCode;
			}
			if (distUp < radius && endY < divisions)
			{
				nextMortonCode[3] = GetNextMortonCode(mortonCode, 1, false) - mortonCode;
			}
			if (distFront < radius && endZ > 0)
			{
				nextMortonCode[4] = GetNextMortonCode(mortonCode, 2, true) - mortonCode;
			}
			if (distBack < radius && endZ < divisions)
			{
				nextMortonCode[5] = GetNextMortonCode(mortonCode, 2, false) - mortonCode;
			}
			
			// 1次元方向の空間をチェック
			for (int i = 0; i < 6; i++)
			{
				if (nextMortonCode[i] != 0)
				{
					resisterAndIntersectShpereCastVsTriangleInTargetNode(mortonCode + nextMortonCode[i] + levelStart);
				}
			}
			
			// 2次元方向の空間をチェック
			for (int i = 0; i < 6; i++)
			{
				if (nextMortonCode[i] == 0) continue;

				int j = ((i / 2 + 1) & 3) * 2;
				if (nextMortonCode[j] != 0)
				{
					if (sqDist[i] + sqDist[j] < sqRadius)
					{
						resisterAndIntersectShpereCastVsTriangleInTargetNode(mortonCode + nextMortonCode[i] + nextMortonCode[j] + levelStart);
					}
				}

				j++;
				if (nextMortonCode[j] != 0)
				{
					if (sqDist[i] + sqDist[j] < sqRadius)
					{
						resisterAndIntersectShpereCastVsTriangleInTargetNode(mortonCode + nextMortonCode[i] + nextMortonCode[j] + levelStart);
					}
				}
			}

			// 3次元方向の空間をチェック
			for (int i = 0; i < 2; i++)
			{
				if (nextMortonCode[i] == 0) continue;

				for (int j = 2; j < 4; j++)
				{
					if (nextMortonCode[j] == 0) continue;

					for (int k = 4; k < 6; k++)
					{
						if (nextMortonCode[i] == 0) continue;

						if (sqDist[i] + sqDist[j] + sqDist[k] < sqRadius)
						{
							resisterAndIntersectShpereCastVsTriangleInTargetNode(mortonCode + nextMortonCode[i] + nextMortonCode[j] + nextMortonCode[k] + levelStart);
						}
					}
				}
			}
		}
	}

	return ret;
}

/**************************************************************************//**
	@brief		指定したノード以下全てのノードでオブジェクトとオブジェクトの押し戻し処理を実行
	@param[in]	uint32_t target : 指定ノード
				bool singleNode : 単一空間指定
				vector<Sphere*>* upperListSphere : 上位ノードの球体
				vector<Capsule*>* upperListCapsule : 上位ノードのカプセル
				vector<AABB*>* upperListAABB : 上位ノードのAABB
				vector<Triangle>* upperListTriangle : 上位ノードの三角形
	@return		bool : 交差するときtrue
*//***************************************************************************/
// 全てのオブジェクトの押し戻し処理を実行
void OctreeNodeManager::CollisionAllObjects(
	uint32_t target,
	bool singleNode,
	std::vector<Sphere*>* upperListSphere,
	std::vector<Capsule*>* upperListCapsule,
	std::vector<AABB*>* upperListAABB,
	std::vector<Triangle>* upperListTriangle)
{
	CollisionSphereVsSphere(target, singleNode, upperListSphere);
	CollisionCapsuleVsCapsule(target, singleNode, upperListCapsule);
	CollisionAABBVsAABB(target, singleNode, upperListAABB);
	CollisionSphereVsCapsule(target, singleNode, upperListSphere, upperListCapsule);
	CollisionSphereVsAABB(target, singleNode, upperListSphere, upperListAABB);
	CollisionCapsuleVsAABB(target, singleNode, upperListCapsule, upperListAABB);
	CollisionSphereVsTriangle(target, singleNode, upperListSphere, upperListTriangle);
	CollisionCapsuleVsTriangle(target, singleNode, upperListCapsule, upperListTriangle);
	CollisionAABBVsTriangle(target, singleNode, upperListAABB, upperListTriangle);
}
// 球体 VS 球体
void OctreeNodeManager::CollisionSphereVsSphere(uint32_t target, bool singleNode, std::vector<Sphere*>* upperListSphere)
{
	OctreeNode targetNode = m_octreeNodes[target];
	IntersectionResult result;

	for (int i = 0; i < targetNode.GetSpheres().size(); i++)
	{
		// 同一空間内の衝突処理
		for (int j = i + 1; j < targetNode.GetSpheres().size(); j++)
		{
			if (Collision::IntersectSphereVsSphere(
				DirectX::XMLoadFloat3(&targetNode.GetSpheres().at(i)->position),
				targetNode.GetSpheres().at(i)->radius,
				DirectX::XMLoadFloat3(&targetNode.GetSpheres().at(j)->position),
				targetNode.GetSpheres().at(j)->radius,
				&result))
			{
				// 押し戻し処理
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetSpheres().at(i)->position.x += addVecF.x * 0.5f;
				targetNode.GetSpheres().at(i)->position.y += addVecF.y * 0.5f;
				targetNode.GetSpheres().at(i)->position.z += addVecF.z * 0.5f;
				targetNode.GetSpheres().at(j)->position.x -= addVecF.x * 0.5f;
				targetNode.GetSpheres().at(j)->position.y -= addVecF.y * 0.5f;
				targetNode.GetSpheres().at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
		// 上位空間との衝突処理
		for (int j = 0; upperListSphere && j < upperListSphere->size(); j++)
		{
			if (Collision::IntersectSphereVsSphere(
				DirectX::XMLoadFloat3(&targetNode.GetSpheres().at(i)->position),
				targetNode.GetSpheres().at(i)->radius,
				DirectX::XMLoadFloat3(&upperListSphere->at(j)->position),
				upperListSphere->at(j)->radius, &result))
			{
				// 押し戻し処理
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetSpheres().at(i)->position.x += addVecF.x * 0.5f;
				targetNode.GetSpheres().at(i)->position.y += addVecF.y * 0.5f;
				targetNode.GetSpheres().at(i)->position.z += addVecF.z * 0.5f;
				upperListSphere->at(j)->position.x -= addVecF.x * 0.5f;
				upperListSphere->at(j)->position.y -= addVecF.y * 0.5f;
				upperListSphere->at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
	}

	// 下位空間が存在しなければreturn
	if (target >= GetLevelStart(m_depth))	return;

	// 単一空間指定でなければ、下位空間へ移動し再起呼び出し
	if (!singleNode)
	{
		// 所持している球体をUpperListに登録してから、再起処理で子空間に移動する
		bool addUpperList = false;	// 登録があったかどうかのフラグ。削除に利用する

		for (int i = 0; i < targetNode.GetSpheres().size(); i++)
		{
			upperListSphere->emplace_back(targetNode.GetSpheres().at(i));
			addUpperList = true;
		}

		for (int i = 1; i <= 8; i++)
		{
			uint32_t nextTarget = target * 8 + i;
			CollisionSphereVsSphere(nextTarget, false, upperListSphere);
		}

		// 下位空間を全て処理し終わったら、UpperListから削除する
		if (addUpperList)
		{
			for (int i = 0; i < targetNode.GetSpheres().size(); i++)
			{
				upperListSphere->pop_back();
			}
		}
	}
}
// カプセル VS カプセル
void OctreeNodeManager::CollisionCapsuleVsCapsule(uint32_t target, bool singleNode, std::vector<Capsule*>* upperListCapsule)
{
	OctreeNode targetNode = m_octreeNodes[target];
	IntersectionResult result;

	for (int i = 0; i < targetNode.GetCapsules().size(); i++)
	{
		// 同一空間内の衝突処理
		for (int j = i + 1; j < targetNode.GetCapsules().size(); j++)
		{
			if (Collision::IntersectCapsuleVsCapsule(
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(i)->position),
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(i)->direction),
				targetNode.GetCapsules().at(i)->length,
				targetNode.GetCapsules().at(i)->radius,
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(j)->position),
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(j)->direction),
				targetNode.GetCapsules().at(j)->length,
				targetNode.GetCapsules().at(j)->radius,
				&result))
			{
				// 押し戻し処理
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetCapsules().at(i)->position.x += addVecF.x * 0.5f;
				targetNode.GetCapsules().at(i)->position.y += addVecF.y * 0.5f;
				targetNode.GetCapsules().at(i)->position.z += addVecF.z * 0.5f;
				targetNode.GetCapsules().at(j)->position.x -= addVecF.x * 0.5f;
				targetNode.GetCapsules().at(j)->position.y -= addVecF.y * 0.5f;
				targetNode.GetCapsules().at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
		// 上位空間との衝突処理
		for (int j = 0; upperListCapsule && j < upperListCapsule->size(); j++)
		{
			if (Collision::IntersectCapsuleVsCapsule(
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(i)->position),
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(i)->direction),
				targetNode.GetCapsules().at(i)->length,
				targetNode.GetCapsules().at(i)->radius,
				DirectX::XMLoadFloat3(&upperListCapsule->at(j)->position),
				DirectX::XMLoadFloat3(&upperListCapsule->at(j)->direction),
				upperListCapsule->at(j)->length,
				upperListCapsule->at(j)->radius,
				&result))
			{
				// 押し戻し処理
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetCapsules().at(i)->position.x += addVecF.x * 0.5f;
				targetNode.GetCapsules().at(i)->position.y += addVecF.y * 0.5f;
				targetNode.GetCapsules().at(i)->position.z += addVecF.z * 0.5f;
				upperListCapsule->at(j)->position.x -= addVecF.x * 0.5f;
				upperListCapsule->at(j)->position.y -= addVecF.y * 0.5f;
				upperListCapsule->at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
	}

	// 下位空間が存在しなければreturn
	if (target >= GetLevelStart(m_depth))	return;

	// 単一空間指定でなければ、下位空間へ移動し再起呼び出し
	if (!singleNode)
	{
		// 所持している球体をUpperListに登録してから、再起処理で子空間に移動する
		bool addUpperList = false;	// 登録があったかどうかのフラグ。削除に利用する

		for (int i = 0; i < targetNode.GetCapsules().size(); i++)
		{
			upperListCapsule->emplace_back(targetNode.GetCapsules().at(i));
			addUpperList = true;
		}

		for (int i = 1; i <= 8; i++)
		{
			uint32_t nextTarget = target * 8 + i;
			CollisionCapsuleVsCapsule(nextTarget, false, upperListCapsule);
		}

		// 下位空間を全て処理し終わったら、UpperListから削除する
		if (addUpperList)
		{
			for (int i = 0; i < targetNode.GetCapsules().size(); i++)
			{
				upperListCapsule->pop_back();
			}
		}
	}
}
// AABB VS AABB
void OctreeNodeManager::CollisionAABBVsAABB(uint32_t target, bool singleNode, std::vector<AABB*>* upperListAABB)
{
	OctreeNode targetNode = m_octreeNodes[target];
	IntersectionResult result;

	for (int i = 0; i < targetNode.GetAABBs().size(); i++)
	{
		// 同一空間内の衝突処理
		for (int j = i + 1; j < targetNode.GetAABBs().size(); j++)
		{
			if (Collision::IntersectAABBVsAABB(
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(i)->position),
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(i)->radii),
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(j)->position),
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(j)->radii),
				&result))
			{
				// 押し戻し処理
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetAABBs().at(i)->position.x += addVecF.x * 0.5f;
				targetNode.GetAABBs().at(i)->position.y += addVecF.y * 0.5f;
				targetNode.GetAABBs().at(i)->position.z += addVecF.z * 0.5f;
				targetNode.GetAABBs().at(j)->position.x -= addVecF.x * 0.5f;
				targetNode.GetAABBs().at(j)->position.y -= addVecF.y * 0.5f;
				targetNode.GetAABBs().at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
		// 上位空間との衝突処理
		for (int j = 0; upperListAABB && j < upperListAABB->size(); j++)
		{
			if (Collision::IntersectAABBVsAABB(
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(i)->position),
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(i)->radii),
				DirectX::XMLoadFloat3(&upperListAABB->at(j)->position),
				DirectX::XMLoadFloat3(&upperListAABB->at(j)->radii),
				&result))
			{
				// 押し戻し処理
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetAABBs().at(i)->position.x += addVecF.x * 0.5f;
				targetNode.GetAABBs().at(i)->position.y += addVecF.y * 0.5f;
				targetNode.GetAABBs().at(i)->position.z += addVecF.z * 0.5f;
				upperListAABB->at(j)->position.x -= addVecF.x * 0.5f;
				upperListAABB->at(j)->position.y -= addVecF.y * 0.5f;
				upperListAABB->at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
	}

	// 下位空間が存在しなければreturn
	if (target >= GetLevelStart(m_depth))	return;

	// 単一空間指定でなければ、下位空間へ移動し再起呼び出し
	if (!singleNode)
	{
		// 所持している球体をUpperListに登録してから、再起処理で子空間に移動する
		bool addUpperList = false;	// 登録があったかどうかのフラグ。削除に利用する

		for (int i = 0; i < targetNode.GetAABBs().size(); i++)
		{
			upperListAABB->emplace_back(targetNode.GetAABBs().at(i));
			addUpperList = true;
		}

		for (int i = 1; i <= 8; i++)
		{
			uint32_t nextTarget = target * 8 + i;
			CollisionAABBVsAABB(nextTarget, false, upperListAABB);
		}

		// 下位空間を全て処理し終わったら、UpperListから削除する
		if (addUpperList)
		{
			for (int i = 0; i < targetNode.GetAABBs().size(); i++)
			{
				upperListAABB->pop_back();
			}
		}
	}
}
// 球体 VS 三角形
void OctreeNodeManager::CollisionSphereVsTriangle(uint32_t target, bool singleNode, std::vector<Sphere*>* upperListSphere, std::vector<Triangle>* upperListTriangle)
{
	OctreeNode targetNode = m_octreeNodes[target];
	IntersectionResult result;

	// 球体側の処理
	for (int i = 0; i < targetNode.GetSpheres().size(); i++)
	{
		// 同一空間内の三角形との衝突処理
		for (int j = 0; j < targetNode.GetTriangles().size(); j++)
		{
			DirectX::XMVECTOR triPos[3] =
			{
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(j).position[0]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(j).position[1]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(j).position[2])
			};
			if (Collision::IntersectSphereVsTriangle(
				DirectX::XMLoadFloat3(&targetNode.GetSpheres().at(i)->position),
				targetNode.GetSpheres().at(i)->radius,
				triPos,
				&result))
			{
				// 球体のみ押し戻し処理
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetSpheres().at(i)->position.x += addVecF.x;
				targetNode.GetSpheres().at(i)->position.y += addVecF.y;
				targetNode.GetSpheres().at(i)->position.z += addVecF.z;
			}
		}
		// 上位空間の三角形との衝突処理
		for (int j = 0; upperListTriangle && j < upperListTriangle->size(); j++)
		{
			DirectX::XMVECTOR triPos[3] =
			{
				DirectX::XMLoadFloat3(&upperListTriangle->at(j).position[0]),
				DirectX::XMLoadFloat3(&upperListTriangle->at(j).position[1]),
				DirectX::XMLoadFloat3(&upperListTriangle->at(j).position[2])
			};
			if (Collision::IntersectSphereVsTriangle(
				DirectX::XMLoadFloat3(&targetNode.GetSpheres().at(i)->position),
				targetNode.GetSpheres().at(i)->radius,
				triPos,
				&result))
			{
				// 球体のみ押し戻し処理
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetSpheres().at(i)->position.x += addVecF.x;
				targetNode.GetSpheres().at(i)->position.y += addVecF.y;
				targetNode.GetSpheres().at(i)->position.z += addVecF.z;
			}
		}
	}

	// 三角形側の処理
	for (int i = 0; upperListSphere && i < upperListSphere->size(); i++)
	{
		// 上位空間の球体との衝突処理
		for (int j = 0; j < targetNode.GetTriangles().size(); j++)
		{
			DirectX::XMVECTOR triPos[3] =
			{
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(j).position[0]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(j).position[1]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(j).position[2])
			};
			if (Collision::IntersectSphereVsTriangle(
				DirectX::XMLoadFloat3(&upperListSphere->at(i)->position),
				upperListSphere->at(i)->radius,
				triPos,
				&result))
			{
				// 球体のみ押し戻し処理
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				upperListSphere->at(i)->position.x += addVecF.x;
				upperListSphere->at(i)->position.y += addVecF.y;
				upperListSphere->at(i)->position.z += addVecF.z;
			}
		}
	}

	// 下位空間が存在しなければreturn
	if (target >= GetLevelStart(m_depth))	return;

	// 単一空間指定でなければ、下位空間へ移動し再起呼び出し
	if (!singleNode)
	{
		// 所持している球体と三角形をUpperListに登録してから、再起処理で子空間に移動する
		bool addUpperList = false;	// 登録があったかどうかのフラグ。削除に利用する

		for (int i = 0; i < targetNode.GetSpheres().size(); i++)
		{
			upperListSphere->emplace_back(targetNode.GetSpheres().at(i));
			addUpperList = true;
		}
		for (int i = 0; i < targetNode.GetTriangles().size(); i++)
		{
			upperListTriangle->emplace_back(targetNode.GetTriangles().at(i));
			addUpperList = true;
		}

		for (int i = 1; i <= 8; i++)
		{
			uint32_t nextTarget = target * 8 + i;
			CollisionSphereVsTriangle(nextTarget, false, upperListSphere, upperListTriangle);
		}

		// 下位空間を全て処理し終わったら、UpperListから削除する
		if (addUpperList)
		{
			for (int i = 0; i < targetNode.GetSpheres().size(); i++)
			{
				upperListSphere->pop_back();
			}
			for (int i = 0; i < targetNode.GetTriangles().size(); i++)
			{
				upperListTriangle->pop_back();
			}
		}
	}
}
// カプセル VS 三角形
void OctreeNodeManager::CollisionCapsuleVsTriangle(uint32_t target, bool singleNode, std::vector<Capsule*>* upperListCapsule, std::vector<Triangle>* upperListTriangle)
{
	OctreeNode targetNode = m_octreeNodes[target];
	IntersectionResult result;

	// カプセル側の処理
	for (int i = 0; i < targetNode.GetCapsules().size(); i++)
	{
		// 同一空間内の三角形との衝突処理
		for (int j = 0; j < targetNode.GetTriangles().size(); j++)
		{
			DirectX::XMVECTOR triPos[3] =
			{
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(j).position[0]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(j).position[1]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(j).position[2])
			};
			if (Collision::IntersectCapsuleVsTriangle(
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(i)->position),
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(i)->direction),
				targetNode.GetCapsules().at(i)->radius,
				targetNode.GetCapsules().at(i)->length,
				triPos,
				&result))
			{
				// カプセルのみ押し戻し処理
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetCapsules().at(i)->position.x += addVecF.x;
				targetNode.GetCapsules().at(i)->position.y += addVecF.y;
				targetNode.GetCapsules().at(i)->position.z += addVecF.z;
			}
		}
		// 上位空間の三角形との衝突処理
		for (int j = 0; upperListTriangle && j < upperListTriangle->size(); j++)
		{
			DirectX::XMVECTOR triPos[3] =
			{
				DirectX::XMLoadFloat3(&upperListTriangle->at(j).position[0]),
				DirectX::XMLoadFloat3(&upperListTriangle->at(j).position[1]),
				DirectX::XMLoadFloat3(&upperListTriangle->at(j).position[2])
			};
			if (Collision::IntersectCapsuleVsTriangle(
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(i)->position),
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(i)->direction),
				targetNode.GetCapsules().at(i)->radius,
				targetNode.GetCapsules().at(i)->length,
				triPos,
				&result))
			{
				// カプセルのみ押し戻し処理
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetCapsules().at(i)->position.x += addVecF.x;
				targetNode.GetCapsules().at(i)->position.y += addVecF.y;
				targetNode.GetCapsules().at(i)->position.z += addVecF.z;
			}
		}
	}

	// 三角形側の処理
	for (int i = 0; upperListCapsule && i < upperListCapsule->size(); i++)
	{
		// 上位空間のカプセルとの衝突処理
		for (int j = 0; j < targetNode.GetTriangles().size(); j++)
		{
			DirectX::XMVECTOR triPos[3] =
			{
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(j).position[0]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(j).position[1]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(j).position[2])
			};
			if (Collision::IntersectCapsuleVsTriangle(
				DirectX::XMLoadFloat3(&upperListCapsule->at(i)->position),
				DirectX::XMLoadFloat3(&upperListCapsule->at(i)->direction),
				upperListCapsule->at(i)->radius,
				upperListCapsule->at(i)->length,
				triPos,
				&result))
			{
				// カプセルのみ押し戻し処理
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				upperListCapsule->at(i)->position.x += addVecF.x;
				upperListCapsule->at(i)->position.y += addVecF.y;
				upperListCapsule->at(i)->position.z += addVecF.z;
			}
		}
	}

	// 下位空間が存在しなければreturn
	if (target >= GetLevelStart(m_depth))	return;

	// 単一空間指定でなければ、下位空間へ移動し再起呼び出し
	if (!singleNode)
	{
		// 所持しているカプセルと三角形をUpperListに登録してから、再起処理で子空間に移動する
		bool addUpperList = false;	// 登録があったかどうかのフラグ。削除に利用する

		for (int i = 0; i < targetNode.GetCapsules().size(); i++)
		{
			upperListCapsule->emplace_back(targetNode.GetCapsules().at(i));
			addUpperList = true;
		}
		for (int i = 0; i < targetNode.GetTriangles().size(); i++)
		{
			upperListTriangle->emplace_back(targetNode.GetTriangles().at(i));
			addUpperList = true;
		}

		for (int i = 1; i <= 8; i++)
		{
			uint32_t nextTarget = target * 8 + i;
			CollisionCapsuleVsTriangle(nextTarget, false, upperListCapsule, upperListTriangle);
		}

		// 下位空間を全て処理し終わったら、UpperListから削除する
		if (addUpperList)
		{
			for (int i = 0; i < targetNode.GetCapsules().size(); i++)
			{
				upperListCapsule->pop_back();
			}
			for (int i = 0; i < targetNode.GetTriangles().size(); i++)
			{
				upperListTriangle->pop_back();
			}
		}
	}
}
// AABB VS 三角形
void OctreeNodeManager::CollisionAABBVsTriangle(uint32_t target, bool singleNode, std::vector<AABB*>* upperListAABB, std::vector<Triangle>* upperListTriangle)
{
	OctreeNode targetNode = m_octreeNodes[target];
	IntersectionResult result;

	// AABB側の処理
	for (int i = 0; i < targetNode.GetAABBs().size(); i++)
	{
		// 同一空間内の三角形との衝突処理
		for (int j = 0; j < targetNode.GetTriangles().size(); j++)
		{
			DirectX::XMVECTOR triPos[3] =
			{
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(j).position[0]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(j).position[1]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(j).position[2])
			};
			if (Collision::IntersectAABBVsTriangle(
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(i)->position),
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(i)->radii),
				triPos,
				&result))
			{
				// AABBのみ押し戻し処理
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetAABBs().at(i)->position.x += addVecF.x;
				targetNode.GetAABBs().at(i)->position.y += addVecF.y;
				targetNode.GetAABBs().at(i)->position.z += addVecF.z;
			}
		}
		// 上位空間の三角形との衝突処理
		for (int j = 0; upperListTriangle && j < upperListTriangle->size(); j++)
		{
			DirectX::XMVECTOR triPos[3] =
			{
				DirectX::XMLoadFloat3(&upperListTriangle->at(j).position[0]),
				DirectX::XMLoadFloat3(&upperListTriangle->at(j).position[1]),
				DirectX::XMLoadFloat3(&upperListTriangle->at(j).position[2])
			};
			if (Collision::IntersectAABBVsTriangle(
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(i)->position),
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(i)->radii),
				triPos,
				&result))
			{
				// AABBのみ押し戻し処理
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetAABBs().at(i)->position.x += addVecF.x;
				targetNode.GetAABBs().at(i)->position.y += addVecF.y;
				targetNode.GetAABBs().at(i)->position.z += addVecF.z;
			}
		}
	}

	// 三角形側の処理
	for (int i = 0; upperListAABB && i < upperListAABB->size(); i++)
	{
		// 上位空間のAABBとの衝突処理
		for (int j = 0; j < targetNode.GetTriangles().size(); j++)
		{
			DirectX::XMVECTOR triPos[3] =
			{
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(j).position[0]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(j).position[1]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(j).position[2])
			};
			if (Collision::IntersectAABBVsTriangle(
				DirectX::XMLoadFloat3(&upperListAABB->at(i)->position),
				DirectX::XMLoadFloat3(&upperListAABB->at(i)->radii),
				triPos,
				&result))
			{
				// AABBのみ押し戻し処理
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				upperListAABB->at(i)->position.x += addVecF.x;
				upperListAABB->at(i)->position.y += addVecF.y;
				upperListAABB->at(i)->position.z += addVecF.z;
			}
		}
	}

	// 下位空間が存在しなければreturn
	if (target >= GetLevelStart(m_depth))	return;

	// 単一空間指定でなければ、下位空間へ移動し再起呼び出し
	if (!singleNode)
	{
		// 所持しているAABBと三角形をUpperListに登録してから、再起処理で子空間に移動する
		bool addUpperList = false;	// 登録があったかどうかのフラグ。削除に利用する

		for (int i = 0; i < targetNode.GetAABBs().size(); i++)
		{
			upperListAABB->emplace_back(targetNode.GetAABBs().at(i));
			addUpperList = true;
		}
		for (int i = 0; i < targetNode.GetTriangles().size(); i++)
		{
			upperListTriangle->emplace_back(targetNode.GetTriangles().at(i));
			addUpperList = true;
		}

		for (int i = 1; i <= 8; i++)
		{
			uint32_t nextTarget = target * 8 + i;
			CollisionAABBVsTriangle(nextTarget, false, upperListAABB, upperListTriangle);
		}

		// 下位空間を全て処理し終わったら、UpperListから削除する
		if (addUpperList)
		{
			for (int i = 0; i < targetNode.GetAABBs().size(); i++)
			{
				upperListAABB->pop_back();
			}
			for (int i = 0; i < targetNode.GetTriangles().size(); i++)
			{
				upperListTriangle->pop_back();
			}
		}
	}
}
// 球体 VS カプセル
void OctreeNodeManager::CollisionSphereVsCapsule(uint32_t target, bool singleNode, std::vector<Sphere*>* upperListSphere, std::vector<Capsule*>* upperListCapsule)
{
	OctreeNode targetNode = m_octreeNodes[target];
	IntersectionResult result;

	// 球体側の処理
	for (int i = 0; i < targetNode.GetSpheres().size(); i++)
	{
		// 同一空間内のカプセルとの衝突処理
		for (int j = 0; j < targetNode.GetCapsules().size(); j++)
		{
			if (Collision::IntersectSphereVsCapsule(
				DirectX::XMLoadFloat3(&targetNode.GetSpheres().at(i)->position),
				targetNode.GetSpheres().at(i)->radius,
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(j)->position),
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(j)->direction),
				targetNode.GetCapsules().at(j)->length,
				targetNode.GetCapsules().at(j)->radius,
				&result))
			{
				// 押し戻し処理
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetSpheres().at(i)->position.x += addVecF.x * 0.5f;
				targetNode.GetSpheres().at(i)->position.y += addVecF.y * 0.5f;
				targetNode.GetSpheres().at(i)->position.z += addVecF.z * 0.5f;
				targetNode.GetCapsules().at(j)->position.x -= addVecF.x * 0.5f;
				targetNode.GetCapsules().at(j)->position.y -= addVecF.y * 0.5f;
				targetNode.GetCapsules().at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
		// 上位空間のカプセルとの衝突処理
		for (int j = 0; upperListCapsule && j < upperListCapsule->size(); j++)
		{
			if (Collision::IntersectSphereVsCapsule(
				DirectX::XMLoadFloat3(&targetNode.GetSpheres().at(i)->position),
				targetNode.GetSpheres().at(i)->radius,
				DirectX::XMLoadFloat3(&upperListCapsule->at(j)->position),
				DirectX::XMLoadFloat3(&upperListCapsule->at(j)->direction),
				upperListCapsule->at(j)->length,
				upperListCapsule->at(j)->radius,
				&result))
			{
				// 押し戻し処理
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetSpheres().at(i)->position.x += addVecF.x * 0.5f;
				targetNode.GetSpheres().at(i)->position.y += addVecF.y * 0.5f;
				targetNode.GetSpheres().at(i)->position.z += addVecF.z * 0.5f;
				upperListCapsule->at(j)->position.x -= addVecF.x * 0.5f;
				upperListCapsule->at(j)->position.y -= addVecF.y * 0.5f;
				upperListCapsule->at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
	}

	// カプセル側の処理
	for (int i = 0; upperListSphere && i < upperListSphere->size(); i++)
	{
		// 上位空間の球体との衝突処理
		for (int j = 0; j < targetNode.GetCapsules().size(); j++)
		{
			if (Collision::IntersectSphereVsCapsule(
				DirectX::XMLoadFloat3(&upperListSphere->at(i)->position),
				upperListSphere->at(i)->radius,
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(j)->position),
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(j)->direction),
				targetNode.GetCapsules().at(j)->length,
				targetNode.GetCapsules().at(j)->radius,
				&result))
			{
				// 押し戻し処理
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				upperListSphere->at(i)->position.x += addVecF.x * 0.5f;
				upperListSphere->at(i)->position.y += addVecF.y * 0.5f;
				upperListSphere->at(i)->position.z += addVecF.z * 0.5f;
				targetNode.GetCapsules().at(j)->position.x -= addVecF.x * 0.5f;
				targetNode.GetCapsules().at(j)->position.y -= addVecF.y * 0.5f;
				targetNode.GetCapsules().at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
	}

	// 下位空間が存在しなければreturn
	if (target >= GetLevelStart(m_depth))	return;

	// 単一空間指定でなければ、下位空間へ移動し再起呼び出し
	if (!singleNode)
	{
		// 所持している球体とカプセルをUpperListに登録してから、再起処理で子空間に移動する
		bool addUpperList = false;	// 登録があったかどうかのフラグ。削除に利用する

		for (int i = 0; i < targetNode.GetSpheres().size(); i++)
		{
			upperListSphere->emplace_back(targetNode.GetSpheres().at(i));
			addUpperList = true;
		}
		for (int i = 0; i < targetNode.GetCapsules().size(); i++)
		{
			upperListCapsule->emplace_back(targetNode.GetCapsules().at(i));
			addUpperList = true;
		}

		for (int i = 1; i <= 8; i++)
		{
			uint32_t nextTarget = target * 8 + i;
			CollisionSphereVsCapsule(nextTarget, false, upperListSphere, upperListCapsule);
		}

		// 下位空間を全て処理し終わったら、UpperListから削除する
		if (addUpperList)
		{
			for (int i = 0; i < targetNode.GetSpheres().size(); i++)
			{
				upperListSphere->pop_back();
			}
			for (int i = 0; i < targetNode.GetCapsules().size(); i++)
			{
				upperListCapsule->pop_back();
			}
		}
	}
}
// 球体 VS AABB
void OctreeNodeManager::CollisionSphereVsAABB(uint32_t target, bool singleNode, std::vector<Sphere*>* upperListSphere, std::vector<AABB*>* upperListAABB)
{
	OctreeNode targetNode = m_octreeNodes[target];
	IntersectionResult result;

	// 球体側の処理
	for (int i = 0; i < targetNode.GetSpheres().size(); i++)
	{
		// 同一空間内のAABBとの衝突処理
		for (int j = 0; j < targetNode.GetAABBs().size(); j++)
		{
			if (Collision::IntersectSphereVsAABB(
				DirectX::XMLoadFloat3(&targetNode.GetSpheres().at(i)->position),
				targetNode.GetSpheres().at(i)->radius,
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(j)->position),
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(j)->radii),
				&result))
			{
				// 押し戻し処理
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetSpheres().at(i)->position.x += addVecF.x * 0.5f;
				targetNode.GetSpheres().at(i)->position.y += addVecF.y * 0.5f;
				targetNode.GetSpheres().at(i)->position.z += addVecF.z * 0.5f;
				targetNode.GetAABBs().at(j)->position.x -= addVecF.x * 0.5f;
				targetNode.GetAABBs().at(j)->position.y -= addVecF.y * 0.5f;
				targetNode.GetAABBs().at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
		// 上位空間のAABBとの衝突処理
		for (int j = 0; upperListAABB && j < upperListAABB->size(); j++)
		{
			if (Collision::IntersectSphereVsAABB(
				DirectX::XMLoadFloat3(&targetNode.GetSpheres().at(i)->position),
				targetNode.GetSpheres().at(i)->radius,
				DirectX::XMLoadFloat3(&upperListAABB->at(j)->position),
				DirectX::XMLoadFloat3(&upperListAABB->at(j)->radii),
				&result))
			{
				// 押し戻し処理
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetSpheres().at(i)->position.x += addVecF.x * 0.5f;
				targetNode.GetSpheres().at(i)->position.y += addVecF.y * 0.5f;
				targetNode.GetSpheres().at(i)->position.z += addVecF.z * 0.5f;
				upperListAABB->at(j)->position.x -= addVecF.x * 0.5f;
				upperListAABB->at(j)->position.y -= addVecF.y * 0.5f;
				upperListAABB->at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
	}

	// AABB側の処理
	for (int i = 0; upperListSphere && i < upperListSphere->size(); i++)
	{
		// 上位空間の球体との衝突処理
		for (int j = 0; j < targetNode.GetAABBs().size(); j++)
		{
			if (Collision::IntersectSphereVsAABB(
				DirectX::XMLoadFloat3(&upperListSphere->at(i)->position),
				upperListSphere->at(i)->radius,
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(j)->position),
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(j)->radii),
				&result))
			{
				// 押し戻し処理
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				upperListSphere->at(i)->position.x += addVecF.x * 0.5f;
				upperListSphere->at(i)->position.y += addVecF.y * 0.5f;
				upperListSphere->at(i)->position.z += addVecF.z * 0.5f;
				targetNode.GetAABBs().at(j)->position.x -= addVecF.x * 0.5f;
				targetNode.GetAABBs().at(j)->position.y -= addVecF.y * 0.5f;
				targetNode.GetAABBs().at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
	}

	// 下位空間が存在しなければreturn
	if (target >= GetLevelStart(m_depth))	return;

	// 単一空間指定でなければ、下位空間へ移動し再起呼び出し
	if (!singleNode)
	{
		// 所持している球体とAABBをUpperListに登録してから、再起処理で子空間に移動する
		bool addUpperList = false;	// 登録があったかどうかのフラグ。削除に利用する

		for (int i = 0; i < targetNode.GetSpheres().size(); i++)
		{
			upperListSphere->emplace_back(targetNode.GetSpheres().at(i));
			addUpperList = true;
		}
		for (int i = 0; i < targetNode.GetAABBs().size(); i++)
		{
			upperListAABB->emplace_back(targetNode.GetAABBs().at(i));
			addUpperList = true;
		}

		for (int i = 1; i <= 8; i++)
		{
			uint32_t nextTarget = target * 8 + i;
			CollisionSphereVsAABB(nextTarget, false, upperListSphere, upperListAABB);
		}

		// 下位空間を全て処理し終わったら、UpperListから削除する
		if (addUpperList)
		{
			for (int i = 0; i < targetNode.GetSpheres().size(); i++)
			{
				upperListSphere->pop_back();
			}
			for (int i = 0; i < targetNode.GetAABBs().size(); i++)
			{
				upperListAABB->pop_back();
			}
		}
	}
}
// カプセル VS AABB
void OctreeNodeManager::CollisionCapsuleVsAABB(uint32_t target, bool singleNode, std::vector<Capsule*>* upperListCapsule, std::vector<AABB*>* upperListAABB)
{
	OctreeNode targetNode = m_octreeNodes[target];
	IntersectionResult result;

	// AABB側の処理
	for (int i = 0; i < targetNode.GetCapsules().size(); i++)
	{
		// 同一空間内のカプセルとの衝突処理
		for (int j = 0; j < targetNode.GetAABBs().size(); j++)
		{
			if (Collision::IntersectCapsuleVsAABB(
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(i)->position),
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(i)->direction),
				targetNode.GetCapsules().at(i)->length,
				targetNode.GetCapsules().at(i)->radius,
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(j)->position),
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(j)->radii),
				&result))
			{
				// 押し戻し処理
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetCapsules().at(i)->position.x += addVecF.x * 0.5f;
				targetNode.GetCapsules().at(i)->position.y += addVecF.y * 0.5f;
				targetNode.GetCapsules().at(i)->position.z += addVecF.z * 0.5f;
				targetNode.GetAABBs().at(j)->position.x -= addVecF.x * 0.5f;
				targetNode.GetAABBs().at(j)->position.y -= addVecF.y * 0.5f;
				targetNode.GetAABBs().at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
		// 上位空間のカプセルとの衝突処理
		for (int j = 0; upperListAABB && j < upperListAABB->size(); j++)
		{
			if (Collision::IntersectCapsuleVsAABB(
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(i)->position),
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(i)->direction),
				targetNode.GetCapsules().at(i)->length,
				targetNode.GetCapsules().at(i)->radius,
				DirectX::XMLoadFloat3(&upperListAABB->at(j)->position),
				DirectX::XMLoadFloat3(&upperListAABB->at(j)->radii),
				&result))
			{
				// 押し戻し処理
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetCapsules().at(i)->position.x += addVecF.x * 0.5f;
				targetNode.GetCapsules().at(i)->position.y += addVecF.y * 0.5f;
				targetNode.GetCapsules().at(i)->position.z += addVecF.z * 0.5f;
				upperListAABB->at(j)->position.x -= addVecF.x * 0.5f;
				upperListAABB->at(j)->position.y -= addVecF.y * 0.5f;
				upperListAABB->at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
	}

	// カプセル側の処理
	for (int i = 0; upperListCapsule && i < upperListCapsule->size(); i++)
	{
		// 上位空間のAABBとの衝突処理
		for (int j = 0; j < targetNode.GetAABBs().size(); j++)
		{
			if (Collision::IntersectCapsuleVsAABB(
				DirectX::XMLoadFloat3(&upperListCapsule->at(i)->position),
				DirectX::XMLoadFloat3(&upperListCapsule->at(i)->direction),
				upperListCapsule->at(i)->length,
				upperListCapsule->at(i)->radius,
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(j)->position),
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(j)->radii),
				&result))
			{
				// 押し戻し処理
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				upperListCapsule->at(i)->position.x += addVecF.x * 0.5f;
				upperListCapsule->at(i)->position.y += addVecF.y * 0.5f;
				upperListCapsule->at(i)->position.z += addVecF.z * 0.5f;
				targetNode.GetAABBs().at(j)->position.x -= addVecF.x * 0.5f;
				targetNode.GetAABBs().at(j)->position.y -= addVecF.y * 0.5f;
				targetNode.GetAABBs().at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
	}

	// 下位空間が存在しなければreturn
	if (target >= GetLevelStart(m_depth))	return;

	// 単一空間指定でなければ、下位空間へ移動し再起呼び出し
	if (!singleNode)
	{
		// 所持しているAABBとカプセルをUpperListに登録してから、再起処理で子空間に移動する
		bool addUpperList = false;	// 登録があったかどうかのフラグ。削除に利用する

		for (int i = 0; i < targetNode.GetCapsules().size(); i++)
		{
			upperListCapsule->emplace_back(targetNode.GetCapsules().at(i));
			addUpperList = true;
		}
		for (int i = 0; i < targetNode.GetAABBs().size(); i++)
		{
			upperListAABB->emplace_back(targetNode.GetAABBs().at(i));
			addUpperList = true;
		}

		for (int i = 1; i <= 8; i++)
		{
			uint32_t nextTarget = target * 8 + i;
			CollisionCapsuleVsAABB(nextTarget, false, upperListCapsule, upperListAABB);
		}

		// 下位空間を全て処理し終わったら、UpperListから削除する
		if (addUpperList)
		{
			for (int i = 0; i < targetNode.GetCapsules().size(); i++)
			{
				upperListCapsule->pop_back();
			}
			for (int i = 0; i < targetNode.GetAABBs().size(); i++)
			{
				upperListAABB->pop_back();
			}
		}
	}
}
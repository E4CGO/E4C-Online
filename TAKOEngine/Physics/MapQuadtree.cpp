//! @file MapQuadtree.cpp
//! @note マップ用四分木空間
#include "TAKOEngine/Physics/MapQuadtree.h"
#include "TAKOEngine/Tool/XMFLOAT.h"
#include <iostream>

/**************************************************************************//**
	@brief		四分木空間の生成
	@param[in]	minPos	: 最小点
				maxPos	: 最大点
				level	: 分割数
	@return		なし
*//***************************************************************************/
void MapQuadtree::CreateQuadtree(DirectX::XMFLOAT3 minPos, DirectX::XMFLOAT3 maxPos, uint32_t level)
{
	XMFLOAT3 routeSize = maxPos - minPos;

	// 最大分割数チェック
	if (level > MAX_LEVEL)
	{
		level = MAX_LEVEL;
	}

	// 分割数保存
	m_level = level;

	// 空間配列サイズ保存
	//m_linerLength = ((1 << ((m_level + 1) << 1)) - 1) / 3;	// (4 ^ (m_level + 1) - 1) / (4 - 1)
	m_linerLength = GetLevelStart(m_level + 1);

	// 空間配列作成
	m_quadtreeNodes = new Node[m_linerLength];
	
	// 各空間のメンバ変数を設定
	m_quadtreeNodes[0].m_minPos = minPos;
	m_quadtreeNodes[0].m_size = routeSize;

	for (uint32_t level = 1; level <= m_level; level++)
	{
		XMFLOAT3 nodeSize = routeSize / static_cast<float>(1 << level);
		uint32_t mortonCode = 0;
		
		for (uint32_t linerIndex = GetLevelStart(level - 1); linerIndex < GetLevelStart(level); linerIndex++)
		{
			m_quadtreeNodes[GetLevelStart(level) + mortonCode].m_mortonCode = mortonCode;
			m_quadtreeNodes[GetLevelStart(level) + mortonCode].m_size = nodeSize;
			m_quadtreeNodes[GetLevelStart(level) + mortonCode].m_minPos = m_quadtreeNodes[linerIndex].m_minPos;
			mortonCode++;

			m_quadtreeNodes[GetLevelStart(level) + mortonCode].m_mortonCode = mortonCode;
			m_quadtreeNodes[GetLevelStart(level) + mortonCode].m_size = nodeSize;
			m_quadtreeNodes[GetLevelStart(level) + mortonCode].m_minPos = m_quadtreeNodes[linerIndex].m_minPos + XMFLOAT3{nodeSize.x, 0, 0};
			mortonCode++;

			m_quadtreeNodes[GetLevelStart(level) + mortonCode].m_mortonCode = mortonCode;
			m_quadtreeNodes[GetLevelStart(level) + mortonCode].m_size = nodeSize;
			m_quadtreeNodes[GetLevelStart(level) + mortonCode].m_minPos = m_quadtreeNodes[linerIndex].m_minPos + XMFLOAT3{0, 0, nodeSize.z};
			mortonCode++;

			m_quadtreeNodes[GetLevelStart(level) + mortonCode].m_mortonCode = mortonCode;
			m_quadtreeNodes[GetLevelStart(level) + mortonCode].m_size = nodeSize;
			m_quadtreeNodes[GetLevelStart(level) + mortonCode].m_minPos = m_quadtreeNodes[linerIndex].m_minPos + XMFLOAT3{nodeSize.x, 0, nodeSize.z};
			mortonCode++;
		}
	}
}

/**************************************************************************//**
	@brief		四分木空間のクリア
	@param[in]	なし
	@return		なし
*//***************************************************************************/
void MapQuadtree::ClearQuadtree()
{
	if (m_linerLength == 0) return;
	for (uint32_t i = 0; i < m_linerLength; i++)
	{
		OFT* pOFT = m_quadtreeNodes[i].m_pLatest;
		while (pOFT != nullptr)
		{
			OFT* next_pOFT = pOFT->m_pNext;
			
			delete pOFT->m_pMesh; // OFT内のオブジェクトを削除
			pOFT->m_pMesh = nullptr;
			
			delete pOFT; // 生成したOFTオブジェクトを削除
			pOFT = next_pOFT;
		}
	}

	delete[] m_quadtreeNodes;	// 四分木ノード配列削除
	m_quadtreeNodes = nullptr;
	m_level = 0;
	m_linerLength = 0;
}

/**************************************************************************//**
	@brief		マップメッシュ登録
	@param[in]	mesh : 登録するメッシュ
	@return		登録可否
*//***************************************************************************/
bool MapQuadtree::Regist(Triangle* mesh)
{
	if (mesh == nullptr)	return false;

	// 三角形を含むAABBを構成する最小点と最大点を作成（y座標を除く）
	XMFLOAT3 minPoint = {};
	XMFLOAT3 maxPoint = {};
	mesh->GetBoundPoints(&minPoint, &maxPoint);

	// 最小点・最大点が四分木のルート空間の外にある場合、登録失敗
	XMFLOAT3 routeMinPoint = m_quadtreeNodes[0].m_minPos;
	XMFLOAT3 routeMaxPoint = m_quadtreeNodes[0].m_minPos + m_quadtreeNodes[0].m_size;
	if (minPoint.x < routeMinPoint.x)	return false;
	if (minPoint.z < routeMinPoint.z)	return false;
	if (maxPoint.x > routeMaxPoint.x)	return false;
	if (maxPoint.z > routeMaxPoint.z)	return false;

	// AABBの最小点のモートンコード
	uint32_t mortonCodeMin = GetMortonCode(minPoint, m_quadtreeNodes[GetLevelStart(m_level)].m_size);
	// AABBの最大点のモートンコード
	uint32_t mortonCodeMax = GetMortonCode(maxPoint, m_quadtreeNodes[GetLevelStart(m_level)].m_size);
	// 上記二つのモートンコードの排他的論理和
	uint32_t mortonCodeXOR = mortonCodeMin ^ mortonCodeMax;

	// モートンコードの排他的論理和を使い、上位空間に登録するかのチェック
	uint32_t count = 1;
	uint32_t shift = 0;

	while (mortonCodeXOR != 0)
	{
		// 下位2bitずつマスクして値をチェックする
		if (mortonCodeXOR & 0x03)
		{
			// シフト数を上書き保存
			shift = count;
		}
		// 排他的論理和を2bitシフトさせて再チェック
		mortonCodeXOR >>= 2;

		count++;
	}

	// 線形（配列）のインデックスへ変換
	uint32_t linerIndex = (mortonCodeMin >> (shift * 2)) + GetLevelStart(m_level - shift);
	if (linerIndex >= m_linerLength) return false;

	uint8_t mortonArea = 0b1111;	// その空間を分割した時に位置するエリア
	
	if (((mortonCodeMin << 2) >> (shift * 2)) & 0x01)	// 最小点が空間の1, 3側にある時
	{
		mortonArea &= ~0b0101;	// 0,2側のフラグを消す
	}
	if (((~mortonCodeMax << 2) >> (shift * 2)) & 0x01)	// 最大点が空間の0, 2側にある時
	{
		mortonArea &= ~0b1010;	// 1,3側のフラグを消す
	}
	if (((mortonCodeMin << 2) >> (shift * 2)) & 0x02)	// 最小点が空間の2, 3側にある時
	{
		mortonArea &= ~0b0011;	// 0,1側のフラグを消す
	}
	if (((~mortonCodeMax << 2) >> (shift * 2)) & 0x02)	// 最大点が空間の0, 1側にある時
	{
		mortonArea &= ~0b1100;	// 2,3側のフラグを消す
	}

	// OFTオブジェクト生成
	OFT* pOFT = new OFT;
	pOFT->m_index = linerIndex;
	pOFT->m_mortonArea = mortonArea;
	pOFT->m_pMesh = mesh;
	
	// 登録
	if (!m_quadtreeNodes[linerIndex].m_pLatest)
	{
		// 新規登録
		m_quadtreeNodes[linerIndex].m_pLatest = pOFT;
	}
	else
	{
		// 最新OFTオブジェクトを更新
		pOFT->m_pNext = m_quadtreeNodes[linerIndex].m_pLatest;
		m_quadtreeNodes[linerIndex].m_pLatest->m_pPre = pOFT;
		m_quadtreeNodes[linerIndex].m_pLatest = pOFT;
	}

	return true;
}

/**************************************************************************//**
	@brief		全てのノードで三角形と垂直レイの交差判定を行う（最初に交差する三角形のみを返す方式）
	@param[in]	rayStart		: 始点位置
				rayDirection	: 向き
				rayDist			: 長さ
				result			: ヒット結果
	@return		交差判定
*//***************************************************************************/
bool MapQuadtree::IntersectVerticalRayVsTriangle(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayDirection, float rayDist, HitResult& result)
{
	bool hit = false;

	// レイの始点のモートンコードと線形インデックスを算出
	uint32_t mortonCode = GetMortonCode(rayStart, m_quadtreeNodes[GetLevelStart(m_level)].m_size);
	uint32_t index = GetLevelStart(m_level) + mortonCode;
	
	if (index >= GetLevelStart(m_level + 1))	return false;

	uint32_t level = m_level;		// 探索する階層（最下層から）
	uint8_t mortonArea = 0b1111;	// その空間内で探索するエリア

	// 四分木のルート空間までループ処理
	while(1)
	{
		IntersectVsRayInNode(index, mortonArea, rayStart, rayDirection, rayDist, result, hit);

		// ルート空間ならbreak
		if (level == 0)	break;

		// 親階層へ
		level--;
		mortonArea = 1 << (mortonCode & 0x03);	// 下位2bitをマスクした分左シフト
		mortonCode >>= 2;
		index = GetLevelStart(level) + mortonCode;
	}

	return hit;
}

/**************************************************************************//**
	@brief		全てのノードで三角形とレイの交差判定を行う（最初に交差する三角形のみを返す方式）
	@param[in]	rayStart		: 始点位置
				rayDirection	: 向き
				rayDist			: 長さ
				result			: ヒット結果
	@return		交差判定
*//***************************************************************************/
bool MapQuadtree::IntersectVsRay(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayDirection, float rayDist, HitResult& result)
{	
	// 制御用変数
	bool hit = false;	// 全体の交差結果。一度でも三角形との交差が出ればtrue
	bool ret = false;	// 階層ごとの交差結果。一度でも三角形の交差が出ればtrue

	// レイの長さでヒット情報の距離を初期化
	result.distance = rayDist;

	// レイの水平方向の成分のベクトル
	XMFLOAT3 horizontalRayDirection = { rayDirection.x * rayDist , 0.0f, rayDirection.z * rayDist };
	float horizontalRayDist = XMFLOAT3Length(horizontalRayDirection);

	// xz軸それぞれのレイの向きをプラス→１、ゼロ→０、マイナス→－１で記録
	int directionX = rayDirection.x > 0.0f ? 1 : (rayDirection.x < 0.0f ? -1 : 0);
	int directionZ = rayDirection.z > 0.0f ? 1 : (rayDirection.z < 0.0f ? -1 : 0);

	// 垂直レイの時
	if (directionX == 0 && directionZ == 0)
	{
		return IntersectVerticalRayVsTriangle(rayStart, rayDirection, rayDist, result);
	}

	// 最小空間においてレイの始点と終点が四分木の最小点を含むノードから数えて何個目のノードに居るか算出（０個目スタート）
	const uint32_t startX = static_cast<uint32_t>((rayStart.x - m_quadtreeNodes[0].m_minPos.x) / m_quadtreeNodes[GetLevelStart(m_level)].m_size.x);
	const uint32_t startZ = static_cast<uint32_t>((rayStart.z - m_quadtreeNodes[0].m_minPos.z) / m_quadtreeNodes[GetLevelStart(m_level)].m_size.z);
	const uint32_t endX = static_cast<uint32_t>((rayStart.x + horizontalRayDirection.x - m_quadtreeNodes[0].m_minPos.x) / m_quadtreeNodes[GetLevelStart(m_level)].m_size.x);
	const uint32_t endZ = static_cast<uint32_t>((rayStart.z + horizontalRayDirection.z - m_quadtreeNodes[0].m_minPos.z) / m_quadtreeNodes[GetLevelStart(m_level)].m_size.z);

	uint32_t level = 0;			// 探索する階層（ルート空間から）
	uint32_t mortonCode = 0;	// 探索するモートンコード
	uint32_t index = 0;			// 探索する空間配列番号
	uint8_t mortonArea = 0;	// その空間内で探索するエリア
	
	// モートンエリアチェック用定数
	uint32_t sX, sZ, eX, eZ;
	uint8_t  mX, mZ;
	if (directionX >= 0)
	{
		sX = startX;	// 最下位bitが1の時チェック
		eX = ~endX;		// 最下位bitが0の時チェック
		mX = 0b1010;	// 1, 3
	}
	else
	{
		sX = ~startX;	// 最下位bitが0の時チェック
		eX = endX;		// 最下位bitが1の時チェック
		mX = 0b0101;	// 0, 2
	}
	if (directionZ >= 0)
	{
		sZ = startZ;	// 最下位bitが1の時チェック
		eZ = ~endZ;		// 最下位bitが0の時チェック
		mZ = 0b1100;	// 2, 3
	}
	else
	{
		sZ = ~startZ;	// 最下位bitが0の時チェック
		eZ = endZ;		// 最下位bitが1の時チェック
		mZ = 0b0011;	// 0, 1
	}

	// 階層数分のループ処理。ルート空間からスタート
	for (level = 0; level <= m_level; level++)
	{
		// 現在の階層の線形インデックスを取得
		uint32_t levelStart = GetLevelStart(level);

		// DDAを使ってレイで辿るノードを算出するための各変数の準備

		// 空間の直方体の辺の長さ
		XMFLOAT3 cubeSize = m_quadtreeNodes[levelStart].m_size;

		// レイの傾きから一つノードを進めた時の次のノードまでの距離の増加量の定数「直方体のxz軸の辺の長さ/|レイの成分|」を算出
		float dx = directionX != 0 ? cubeSize.x / fabsf(horizontalRayDirection.x) : 0.0f;
		float dz = directionZ != 0 ? cubeSize.z / fabsf(horizontalRayDirection.z) : 0.0f;

		// 探索する階層においてレイの始点と終点が四分木の最小点を含むノードから数えて何個目のノードに居るか算出（０個目スタート）
		uint32_t nowX = startX >> (m_level - level);
		uint32_t nowZ = startZ >> (m_level - level);
		uint32_t goalX = endX >> (m_level - level);
		uint32_t goalZ = endZ >> (m_level - level);

		// レイの始点のノードの最小・最大座標の算出
		float minX = m_quadtreeNodes[0].m_minPos.x + nowX * cubeSize.x, maxX = minX + cubeSize.x;
		float minZ = m_quadtreeNodes[0].m_minPos.z + nowZ * cubeSize.z, maxZ = minZ + cubeSize.z;

		// レイの始点において、レイが進む際、「次のノードにぶつかるまでの距離/|レイの成分|」を算出
		float distX = directionX * (maxX - rayStart.x) + ((1 - directionX) / 2) * cubeSize.x;
		float distZ = directionZ * (maxZ - rayStart.z) + ((1 - directionZ) / 2) * cubeSize.z;

		float tx = directionX != 0 ? distX / fabsf(horizontalRayDirection.x) : FLT_MAX;
		float tz = directionZ != 0 ? distZ / fabsf(horizontalRayDirection.z) : FLT_MAX;

		// モートンエリアチェック
		mortonArea = 0b1111;
		if (level < m_level)
		{
			if (sX >> (m_level - level - 1) & 0x01)
			{
				mortonArea &= mX;
			}
			if (sZ >> (m_level - level - 1) & 0x01)
			{
				mortonArea &= mZ;
			}
		}

		// ループ処理で四分木の同一階層内のレイvs三角形の交差判定を行う
		while (1)
		{
			// モートンコード算出
			mortonCode = bitSeparete(nowX) | (bitSeparete(nowZ) << 1);

			// 線形（配列）のインデックスへ変換
			index = mortonCode + levelStart;

			// 階層内に収まっているか確認
			if (index <= GetLevelStart(level + 1) - 1)
			{
				// 空間内の当たり判定処理
				ret = IntersectVsRayInNode(index, mortonArea, rayStart, rayDirection, rayDist, result, hit);
			}
			else
			{
				// 階層外なら終了
				break;
			}

			// １度でもヒットしていたら、この階層のチェックは終了。（レイの開始点に近いノード順にチェックしているので）
			if (ret)
			{
				break;
			}

			// DDAを用いて、次にチェックする隣の区画を算出し、そのノードを示すモートンコードを上書きする
			// x方向
			if (tx <= tz)
			{
				// 終端の空間まで来ていたら現在の階層の処理を終了させる
				if (nowX == goalX)	break;

				// 既に見つかっている最短交差距離よりも探索する空間までの距離が長くなっていれば、
				// これ以上進む必要はないため現在の階層の処理を終了させる
				if (result.distance < rayDist * tx)	break;

				// 移動するのでtxとnowXを更新する
				tx += dx;
				nowX += directionX;
				
				mortonArea |= ((mortonArea & mX) << 1) >> (directionX + 1);	// 隣のフラグを立てる
				if (level < m_level && nowX == goalX)	// 最小空間ではなく、xが終点の空間の時
				{
					if (eX >> (m_level - level - 1) & 0x01)	// モートンエリアチェック
					{
						mortonArea &= ~mX;
					}
				}
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

				mortonArea |= ((mortonArea & mZ) << 2) >> (2 * (directionZ + 1));	// 隣のフラグを立てる
				if (level < m_level && nowZ == goalZ)	// 最小空間ではなく、zが終点の空間の時
				{
					if (eZ >> (m_level - level - 1) & 0x01)	// モートンエリアチェック
					{
						mortonArea &= ~mZ;
					}
				}
			}
		}
	}

	return hit;
}

/**************************************************************************//**
	@brief		ある空間内でのレイの交差判定
	@param[in]	index		: 探索する空間
				mortonArea	: その空間内で探索するエリア
				start		: 始点位置
				direction	: 向き
				dist		: 長さ
				result		: ヒット結果
				hit			: 全体の返り値となるフラグ
	@return		衝突判定
*//***************************************************************************/
bool MapQuadtree::IntersectVsRayInNode(
	uint32_t index,
	uint8_t mortonArea,
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& direction,
	float dist,
	HitResult& result,
	bool& hit)
{
	bool ret = false; //この空間内での衝突判定
	OFT* oft = m_quadtreeNodes[index].m_pLatest;
	
	while (oft)
	{
		if (oft->m_mortonArea & mortonArea)
		{
			if (Collision::IntersectRayVsTriangle(start, direction, dist, oft->m_pMesh->position, result))
			{
				hit = true;
				ret = true;
			}
		}
		oft = oft->m_pNext;
	}

	return ret;
}

/**************************************************************************//**
	@brief		全てのノードで三角形とスフィアキャストの交差判定を行う（最初に交差する三角形のみを返す方式）
	@param[in]	rayStart		: 始点位置
				rayDirection	: 向き
				rayDist			: 長さ
				rayRadius		: 半径
				result			: ヒット結果
	@return		交差判定
*//***************************************************************************/
bool MapQuadtree::IntersectVsSphereCast(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayDirection, float rayDist, float rayRadius, HitResult& result)
{
	Capsule spherecast(rayStart, rayDirection, rayDist, rayRadius);

	return IntersectVsCapsule(spherecast, &result);
}

/**************************************************************************//**
	@brief		球の衝突判定
	@param[in]	sphere		: 球オブジェクト
	@return		衝突判定
*//***************************************************************************/
bool MapQuadtree::IntersectVsSphere(Sphere& sphere)
{
	XMFLOAT3 minPos, maxPos;
	sphere.GetBoundPoints(&minPos, &maxPos);
	XMVECTOR position = XMLoadFloat3(&sphere.position);
	bool hit = false;

	// カプセルの最小点が四分木の最小点を含むノードから数えて何個目のノードに居るか算出（０個目スタート）
	const uint32_t minX = static_cast<uint32_t>((minPos.x - m_quadtreeNodes[0].m_minPos.x) / m_quadtreeNodes[GetLevelStart(m_level)].m_size.x);
	if (minX >= 1u << m_level) return false;	// 範囲外ならfalse
	const uint32_t minZ = static_cast<uint32_t>((minPos.z - m_quadtreeNodes[0].m_minPos.z) / m_quadtreeNodes[GetLevelStart(m_level)].m_size.z);
	if (minZ >= 1u << m_level) return false;	// 範囲外ならfalse

	// カプセルの最大点が四分木の最小点を含むノードから数えて何個目のノードに居るか算出（０個目スタート）
	const uint32_t maxX = static_cast<uint32_t>((maxPos.x - m_quadtreeNodes[0].m_minPos.x) / m_quadtreeNodes[GetLevelStart(m_level)].m_size.x);
	if (maxX >= 1u << m_level) return false;	// 範囲外ならfalse
	const uint32_t maxZ = static_cast<uint32_t>((maxPos.z - m_quadtreeNodes[0].m_minPos.z) / m_quadtreeNodes[GetLevelStart(m_level)].m_size.z);
	if (maxZ >= 1u << m_level) return false;	// 範囲外ならfalse

	// 探索用変数の準備
	uint32_t nowX = minX;
	uint32_t nowZ = minZ;
	uint32_t level = m_level;	// 探索する階層（最下層から）
	int32_t  depth = m_level - level;
	uint32_t mortonCode = 0;	// 探索するモートンコード
	uint32_t index = 0;			// 探索する空間配列番号
	uint8_t mortonArea = 0;	// その空間内で探索するエリア	

	while (1)
	{
		mortonArea = 0b1111;
		if (depth)
		{
			if (minX >> (depth - 1) & 0x01)	// minXが空間の1,3側にある時
			{
				mortonArea &= ~0b0101;	// 0,2側のフラグを消す
			}
			if (minZ >> (depth - 1) & 0x01)	// minZが空間の2,3側にある時
			{
				mortonArea &= ~0b0011;	// 0,1側のフラグを消す
			}
		}

		while (nowZ <= maxZ >> depth)
		{
			while (nowX <= maxX >> depth)
			{
				// モートンコードと線形インデックスを算出
				mortonCode = bitSeparete(nowX) | (bitSeparete(nowZ) << 1);
				index = GetLevelStart(level) + mortonCode;

				// 球押し戻し
				IntersectVsSphereInNode(index, mortonArea, position, sphere.radius, hit);

				// 隣の空間へ
				nowX++;
				mortonArea |= (mortonArea & 0b1010) >> 1;	// 隣のフラグを立てる
				if (depth && nowX == maxX >> depth)	// 最小空間ではなく、xが最大点の空間の時
				{
					if (~maxX >> (depth - 1) & 0x01)	// maxXが空間の0,2側にある時
					{
						mortonArea &= ~0b1010;	// 1,3側のフラグを消す
					}
				}
			}
			// 隣の列の空間へ
			nowX = minX >> depth;
			nowZ++;
			mortonArea = 0b1111;
			if (depth)
			{
				if (minX >> (depth - 1) & 0x01)	// minXが空間の1,3側にある時
				{
					mortonArea &= ~0b0101;	// 0,2側のフラグを消す
				}

				if (nowZ == maxZ >> depth)	// zが最大点の空間の時
				{
					if (~maxZ >> (depth - 1) & 0x01)	// maxZが空間の0,1側にある時
					{
						mortonArea &= ~0b1100;	// 2,3側のフラグを消す
					}
				}

			}
		}

		// ルート空間の時break
		if (level == 0)	break;

		// 親空間へ
		level--;
		depth++;
		nowX = minX >> depth;
		nowZ = minZ >> depth;
	}

	XMStoreFloat3(&sphere.position, position);

	return hit;
}

/**************************************************************************//**
	@brief		ある空間内での球の衝突判定
	@param[in]	index		: 探索する空間
				mortonArea	: その空間内で探索するエリア
				spherePos	: 位置
				radius		: 半径
				hit			: 全体の返り値となるフラグ
				result		: 最短距離のメッシュを返す（スフィアキャスト利用時に使用）
	@return		衝突判定
*//***************************************************************************/
bool MapQuadtree::IntersectVsSphereInNode(
	uint32_t index,
	uint8_t mortonArea,
	DirectX::XMVECTOR& spherePos,
	float radius,
	bool& hit)
{
	bool ret = false; //この空間内での衝突判定
	OFT* oft = m_quadtreeNodes[index].m_pLatest;
	IntersectionResult intersect;

	while (oft)
	{
		if (oft->m_mortonArea & mortonArea)
		{
			XMVECTOR triPos[3] =
			{
				XMLoadFloat3(&oft->m_pMesh->position[0]),
				XMLoadFloat3(&oft->m_pMesh->position[1]),
				XMLoadFloat3(&oft->m_pMesh->position[2])
			};

			if (Collision::IntersectSphereVsTriangle(spherePos, radius, triPos, &intersect, true))
			{
				// カプセルのみ押し戻し処理
				spherePos = XMVectorAdd(spherePos, XMVectorScale(intersect.normal, intersect.penetration));
				hit = true;
				ret = true;
			}
		}
		oft = oft->m_pNext;
	}

	return ret;
}

/**************************************************************************//**
	@brief		カプセルの衝突判定
	@param[in]	capsule		: カプセルオブジェクト
				result		: 最短距離のメッシュを返す（スフィアキャスト利用時に使用）
	@return		衝突判定
*//***************************************************************************/
bool MapQuadtree::IntersectVsCapsule(Capsule& capsule, HitResult* result)
{
	XMFLOAT3 minPos, maxPos;
	capsule.GetBoundPoints(&minPos, &maxPos);
	XMVECTOR position = XMLoadFloat3(&capsule.position);
	XMVECTOR direction = XMLoadFloat3(&capsule.direction);
	bool hit = false;

	// カプセルの最小点が四分木の最小点を含むノードから数えて何個目のノードに居るか算出（０個目スタート）
	const uint32_t minX = static_cast<uint32_t>((minPos.x - m_quadtreeNodes[0].m_minPos.x) / m_quadtreeNodes[GetLevelStart(m_level)].m_size.x);
	if (minX >= 1u << m_level) return false;	// 範囲外ならfalse
	const uint32_t minZ = static_cast<uint32_t>((minPos.z - m_quadtreeNodes[0].m_minPos.z) / m_quadtreeNodes[GetLevelStart(m_level)].m_size.z);
	if (minZ >= 1u << m_level) return false;	// 範囲外ならfalse
	
	// カプセルの最大点が四分木の最小点を含むノードから数えて何個目のノードに居るか算出（０個目スタート）
	const uint32_t maxX = static_cast<uint32_t>((maxPos.x - m_quadtreeNodes[0].m_minPos.x) / m_quadtreeNodes[GetLevelStart(m_level)].m_size.x);
	if (maxX >= 1u << m_level) return false;	// 範囲外ならfalse
	const uint32_t maxZ = static_cast<uint32_t>((maxPos.z - m_quadtreeNodes[0].m_minPos.z) / m_quadtreeNodes[GetLevelStart(m_level)].m_size.z);
	if (maxZ >= 1u << m_level) return false;	// 範囲外ならfalse

	// 探索用変数の準備
	uint32_t nowX = minX;
	uint32_t nowZ = minZ;
	uint32_t level = m_level;	// 探索する階層（最下層から）
	int32_t  depth = m_level - level;
	uint32_t mortonCode = 0;	// 探索するモートンコード
	uint32_t index = 0;			// 探索する空間配列番号
	uint8_t mortonArea = 0;	// その空間内で探索するエリア	

	while (1)
	{
		mortonArea = 0b1111;
		if (depth)
		{
			if (minX >> (depth - 1) & 0x01)	// minXが空間の1,3側にある時
			{
				mortonArea &= ~0b0101;	// 0,2側のフラグを消す
			}
			if (minZ >> (depth - 1) & 0x01)	// minZが空間の2,3側にある時
			{
				mortonArea &= ~0b0011;	// 0,1側のフラグを消す
			}
		}
	
		while (nowZ <= maxZ >> depth)
		{
			while (nowX <= maxX >> depth)
			{
				// モートンコードと線形インデックスを算出
				mortonCode = bitSeparete(nowX) | (bitSeparete(nowZ) << 1);
				index = GetLevelStart(level) + mortonCode;
				
				if (result)
				{
					// スフィアキャスト利用
					IntersectVsCapsuleInNode(index, mortonArea, position, direction, capsule.radius, capsule.length, hit, result);
				}
				else
				{
					// カプセル押し戻し
					IntersectVsCapsuleInNode(index, mortonArea, position, direction, capsule.radius, capsule.length, hit);
				}
	
				// 隣の空間へ
				nowX++;
				mortonArea |= (mortonArea & 0b1010) >> 1;	// 隣のフラグを立てる
				if (depth && nowX == maxX >> depth)	// 最小空間ではなく、xが最大点の空間の時
				{
					if (~maxX >> (depth - 1) & 0x01)	// maxXが空間の0,2側にある時
					{
						mortonArea &= ~0b1010;	// 1,3側のフラグを消す
					}
				}
			}
			// 隣の列の空間へ
			nowX = minX >> depth;
			nowZ++;
			mortonArea = 0b1111;
			if (depth)
			{
				if (minX >> (depth - 1) & 0x01)	// minXが空間の1,3側にある時
				{
					mortonArea &= ~0b0101;	// 0,2側のフラグを消す
				}
	
				if(nowZ == maxZ >> depth)	// zが最大点の空間の時
				{
					if (~maxZ >> (depth - 1) & 0x01)	// maxZが空間の0,1側にある時
					{
						mortonArea &= ~0b1100;	// 2,3側のフラグを消す
					}
				}
	
			}
		}
		
		// ルート空間の時break
		if (level == 0)	break;

		// 親空間へ
		level--;
		depth++;
		nowX = minX >> depth;
		nowZ = minZ >> depth;
	}
	
	if (!result)
	{
		XMStoreFloat3(&capsule.position, position);
	}

	return hit;
}

/**************************************************************************//**
	@brief		ある空間内でのカプセルの衝突判定
	@param[in]	index		: 探索する空間
				mortonArea	: その空間内で探索するエリア
				capsulePos	: 位置
				direction	: 向き
				radius		: 半径
				length		: 長さ
				hit			: 全体の返り値となるフラグ
				result		: 最短距離のメッシュを返す（スフィアキャスト利用時に使用）
	@return		衝突判定
*//***************************************************************************/
bool MapQuadtree::IntersectVsCapsuleInNode(
	uint32_t index,
	uint8_t mortonArea,
	DirectX::XMVECTOR& capsulePos,
	const DirectX::XMVECTOR& direction,
	float radius,
	float length,
	bool& hit,
	HitResult* result)
{
	bool ret = false; //この空間内での衝突判定
	OFT* oft = m_quadtreeNodes[index].m_pLatest;
	
	while (oft)
	{
		if (oft->m_mortonArea & mortonArea)
		{
			XMVECTOR triPos[3] =
			{
				XMLoadFloat3(&oft->m_pMesh->position[0]),
				XMLoadFloat3(&oft->m_pMesh->position[1]),
				XMLoadFloat3(&oft->m_pMesh->position[2])
			};

			if (result)
			{
				HitResult tmpResult;
				if (Collision::IntersectSphereCastVsTriangle(capsulePos, direction, length, radius, triPos, &tmpResult, true))
				{
					if (result->distance > tmpResult.distance)
					{
						result->distance = tmpResult.distance;
						result->normal = tmpResult.normal;
						result->position = tmpResult.position;
						result->triangleVerts[0] = tmpResult.triangleVerts[0];
						result->triangleVerts[1] = tmpResult.triangleVerts[1];
						result->triangleVerts[2] = tmpResult.triangleVerts[2];
						result->materialIndex = oft->m_pMesh->materialIndex;
						hit = true;
						ret = true;
					}
				}
			}
			else
			{
				IntersectionResult intersect;
				if (Collision::IntersectCapsuleVsTriangle(capsulePos, direction, radius, length, triPos, &intersect))
				{
					// カプセルのみ押し戻し処理
					capsulePos = XMVectorAdd(capsulePos, XMVectorScale(intersect.normal, intersect.penetration));
					hit = true;
					ret = true;
				}
			}
		}
		oft = oft->m_pNext;
	}
	
	return ret;
}
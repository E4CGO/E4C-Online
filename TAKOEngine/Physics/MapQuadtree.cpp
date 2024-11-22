//! @file MapQuadtree.cpp
//! @note マップ用四分木空間
#include "TAKOEngine/Physics/MapQuadtree.h"
#include "TAKOEngine/Tool/XMFLOAT.h"

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
	m_linerLength = ((1 << ((m_level + 1) << 1)) - 1) / 3;	// (4 ^ (m_level + 1) - 1) / (4 - 1)

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
	uint32_t maskedXOR = 0x03;

	while (mortonCodeXOR != 0)
	{
		// 下位2bitずつマスクして値をチェックする
		maskedXOR = mortonCodeXOR & 0x03;
		if (maskedXOR)
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

	uint32_t mortonArea = 0b1111;	// その空間を分割した時に位置するエリア
	
	if ((mortonCodeMin >> ((shift - 1) * 2)) & 0x01)	// 最小点が空間の1, 3側にある時
	{
		mortonArea &= ~0b0101;	// 0,2側のフラグを消す
	}
	if ((~mortonCodeMax >> ((shift - 1) * 2)) & 0x01)	// 最大点が空間の0, 2側にある時
	{
		mortonArea &= ~0b1010;	// 1,3側のフラグを消す
	}
	if ((mortonCodeMin >> ((shift - 1) * 2)) & 0x02)	// 最小点が空間の2, 3側にある時
	{
		mortonArea &= ~0b0011;	// 0,1側のフラグを消す
	}
	if ((~mortonCodeMax >> ((shift - 1) * 2)) & 0x02)	// 最大点が空間の0, 1側にある時
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
	@brief		全てのノードで三角形とレイの交差判定を行う（最初に交差する三角形のみを返す方式）
	@param[in]	rayStart		: 始点位置
				rayDirection	: 向き
				float rayDist	: 長さ
				result			: ヒット結果
	@return		交差判定
*//***************************************************************************/
bool MapQuadtree::IntersectVsRay(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayDirection, float rayDist, HitResult& result)
{
	DirectX::XMFLOAT3 minPos, maxPos;
	if (rayDirection.x > 0.0f)
	{
		minPos.x = rayStart.x;
		maxPos.x = rayStart.x + rayDirection.x * rayDist;
	}
	else
	{
		minPos.x = rayStart.x + rayDirection.x * rayDist;
		maxPos.x = rayStart.x;
	}
	if (rayDirection.z > 0.0f)
	{
		minPos.z = rayStart.z;
		maxPos.z = rayStart.z + rayDirection.z * rayDist;
	}
	else
	{
		minPos.z = rayStart.z + rayDirection.z * rayDist;
		maxPos.z = rayStart.z;
	}

	bool hit = false;

	// カプセルの最小点が四分木の最小点を含むノードから数えて何個目のノードに居るか算出（０個目スタート）
	const uint32_t minX = static_cast<uint32_t>((minPos.x - m_quadtreeNodes[0].m_minPos.x) / m_quadtreeNodes[GetLevelStart(m_level)].m_size.x);
	const uint32_t minZ = static_cast<uint32_t>((minPos.z - m_quadtreeNodes[0].m_minPos.z) / m_quadtreeNodes[GetLevelStart(m_level)].m_size.z);
	
	// カプセルの最大点が四分木の最小点を含むノードから数えて何個目のノードに居るか算出（０個目スタート）
	const uint32_t maxX = static_cast<uint32_t>((maxPos.x - m_quadtreeNodes[0].m_minPos.x) / m_quadtreeNodes[GetLevelStart(m_level)].m_size.x);
	const uint32_t maxZ = static_cast<uint32_t>((maxPos.z - m_quadtreeNodes[0].m_minPos.z) / m_quadtreeNodes[GetLevelStart(m_level)].m_size.z);

	// 探索用変数の準備
	uint32_t nowX = minX;
	uint32_t nowZ = minZ;
	uint32_t level = m_level;	// 探索する階層（最下層から）
	int32_t  depth = m_level - level;
	uint32_t mortonCode = 0;	// 探索するモートンコード
	uint32_t index = 0;			// 探索する空間配列番号
	uint32_t mortonArea = 0;	// その空間内で探索するエリア	

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
				mortonCode = bitSeparete(nowX) | (bitSeparete(nowZ) << 1);
				index = GetLevelStart(level) + mortonCode;
				
				IntersectVsRayInNode(index, mortonArea, rayStart, rayDirection, rayDist, result, hit);
	
				// 隣の空間へ
				nowX++;
				//mortonCode = bitSeparete(nowX) | (bitSeparete(nowZ) << 1);
				//index = GetLevelStart(level) + mortonCode;
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
			//mortonCode = bitSeparete(nowX) | (bitSeparete(nowZ) << 1);
			//index = GetLevelStart(level) + mortonCode;
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
	uint32_t mortonArea,
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& direction,
	float dist,
	HitResult& result,
	bool& hit)
{
	OFT* oft = m_quadtreeNodes[index].m_pLatest;
	while (oft)
	{
		if (oft->m_mortonArea & mortonArea)
		{
			if (Collision::IntersectRayVsTriangle(start, direction, dist, oft->m_pMesh->position, result))
			{
				hit = true;
			}
		}
		oft = oft->m_pNext;
	}

	return hit;
}

/**************************************************************************//**
	@brief		カプセルの押し戻し
	@param[in]	capsule		: カプセルオブジェクト
				wallCheck	: 壁判定だけするか（押し戻しはしない）
	@return		衝突判定
*//***************************************************************************/
bool MapQuadtree::IntersectVsCapsule(Capsule& capsule, bool wallCheck)
{
	DirectX::XMFLOAT3 minPos, maxPos;
	capsule.GetBoundPoints(&minPos, &maxPos);
	DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&capsule.position);
	DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&capsule.direction);
	bool hit = false;

	// カプセルの最小点が四分木の最小点を含むノードから数えて何個目のノードに居るか算出（０個目スタート）
	const uint32_t minX = static_cast<uint32_t>((minPos.x - m_quadtreeNodes[0].m_minPos.x) / m_quadtreeNodes[GetLevelStart(m_level)].m_size.x);
	const uint32_t minZ = static_cast<uint32_t>((minPos.z - m_quadtreeNodes[0].m_minPos.z) / m_quadtreeNodes[GetLevelStart(m_level)].m_size.z);
	
	// カプセルの最大点が四分木の最小点を含むノードから数えて何個目のノードに居るか算出（０個目スタート）
	const uint32_t maxX = static_cast<uint32_t>((maxPos.x - m_quadtreeNodes[0].m_minPos.x) / m_quadtreeNodes[GetLevelStart(m_level)].m_size.x);
	const uint32_t maxZ = static_cast<uint32_t>((maxPos.z - m_quadtreeNodes[0].m_minPos.z) / m_quadtreeNodes[GetLevelStart(m_level)].m_size.z);

	// 探索用変数の準備
	uint32_t nowX = minX;
	uint32_t nowZ = minZ;
	uint32_t level = m_level;	// 探索する階層（最下層から）
	int32_t  depth = m_level - level;
	uint32_t mortonCode = 0;	// 探索するモートンコード
	uint32_t index = 0;			// 探索する空間配列番号
	uint32_t mortonArea = 0;	// その空間内で探索するエリア	

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
				mortonCode = bitSeparete(nowX) | (bitSeparete(nowZ) << 1);
				index = GetLevelStart(level) + mortonCode;
				
				IntersectVsCapsuleInNode(index, mortonArea, position, direction, capsule.radius, capsule.length, wallCheck, hit);
	
				// 隣の空間へ
				nowX++;
				//mortonCode = bitSeparete(nowX) | (bitSeparete(nowZ) << 1);
				//index = GetLevelStart(level) + mortonCode;
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
			//mortonCode = bitSeparete(nowX) | (bitSeparete(nowZ) << 1);
			//index = GetLevelStart(level) + mortonCode;
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
	
	if (!wallCheck)
	{
		DirectX::XMStoreFloat3(&capsule.position, position);
	}

	return hit;
}

/**************************************************************************//**
	@brief		ある空間内でのカプセルの押し戻し
	@param[in]	index		: 探索する空間
				mortonArea	: その空間内で探索するエリア
				capsulePos	: 位置
				direction	: 向き
				radius		: 半径
				length		: 長さ
				wallCheck	: 壁判定だけするか（押し戻しはしない）
				hit			: 全体の返り値となるフラグ
	@return		衝突判定
*//***************************************************************************/
bool MapQuadtree::IntersectVsCapsuleInNode(
	uint32_t index,
	uint32_t mortonArea,
	DirectX::XMVECTOR& capsulePos,
	const DirectX::XMVECTOR& direction,
	float radius,
	float length,
	bool wallCheck,
	bool& hit)
{
	OFT* oft = m_quadtreeNodes[index].m_pLatest;
	IntersectionResult result;
	while (oft)
	{
		if (oft->m_mortonArea & mortonArea)
		{
			DirectX::XMVECTOR triPos[3] =
			{
				DirectX::XMLoadFloat3(&oft->m_pMesh->position[0]),
				DirectX::XMLoadFloat3(&oft->m_pMesh->position[1]),
				DirectX::XMLoadFloat3(&oft->m_pMesh->position[2])
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
		}
		oft = oft->m_pNext;
	}

	return hit;
}
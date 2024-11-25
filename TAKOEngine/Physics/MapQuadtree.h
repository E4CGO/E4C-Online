//! @file MapQuadtree.h
//! @note マップ用四分木空間

#ifndef __MAP_QUADTREE_H__
#define __MAP_QUADTREE_H__

#include "TAKOEngine/Physics/Collision.h"

class MapQuadtree
{
public:
	MapQuadtree()
	{
		m_quadtreeNodes = nullptr;
		m_level = 0;
		m_linerLength = 0;
	}
	~MapQuadtree() {}

public:
	// 四分木空間の生成
	void CreateQuadtree(DirectX::XMFLOAT3 minPos, DirectX::XMFLOAT3 maxPos, uint32_t level);

	// 四分木空間のクリア
	void ClearQuadtree();

	// マップメッシュ登録
	bool Regist(Triangle* mesh);

	// 全てのノードで三角形と垂直レイの交差判定を行う（最初に交差する三角形のみを返す方式）
	bool IntersectVerticalRayVsTriangle(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayDirection, float rayDist, HitResult& result);

	// 全てのノードで三角形とレイの交差判定を行う（最初に交差する三角形のみを返す方式）
	bool IntersectVsRay(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayDirection, float rayDist, HitResult& result);

	// 全てのノードで三角形とスフィアキャストの交差判定を行う（最初に交差する三角形のみを返す方式）
	bool IntersectVsSphereCast(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayDirection, float rayDist, float radius, HitResult& result);

	// 球の衝突判定
	bool IntersectVsSphere(Sphere& sphere);

	// カプセルの衝突判定
	bool IntersectVsCapsule(Capsule& capsule, HitResult* result = nullptr);
	
private:
	struct OFT	// OBJECT_FOR_TREE構造体
	{
		uint32_t	m_index = 0;		// 登録空間の配列番号
		uint8_t		m_mortonArea = 0;	// 登録空間を分割した時のモートンコード（０～３）の、どの場所に存在するか。右からその数分シフトさせた位置にフラグを立てる（例：1と3→0b1010）
		Triangle*	m_pMesh = nullptr;	// 判定対象オブジェクト
		OFT*		m_pPre = nullptr;	// 前のOFTオブジェクト
		OFT*		m_pNext = nullptr;	// 次のOFTオブジェクト
	};

	struct Node
	{
		uint32_t			m_mortonCode = 0;	// この空間が属する階層でのモートンコード
		DirectX::XMFLOAT3	m_minPos = {};		// 四分木空間の最小点
		DirectX::XMFLOAT3	m_size = {};		// 四分木空間の3辺長（y軸は0）
		OFT*				m_pLatest = nullptr;	// 最新OFTオブジェクトのアドレス
	};

private:
	// 線形四分木の階層のスタートインデックスの取得
	const uint32_t GetLevelStart(uint32_t level)
	{
		if (level > MAX_LEVEL)	return 349525;

		return LevelStart[level];
	}

	// ある空間の親空間の配列番号を取得
	const int32_t GetParentIndex(uint32_t index)
	{
		// ルート空間なら-1でreturn
		if (index == 0)	return -1;

		return (index - 1) >> 2;
	}

	// モートンコードの生成
	uint32_t GetMortonCode(const DirectX::XMFLOAT3& point, DirectX::XMFLOAT3 nodeSize)
	{
		uint32_t x = static_cast<uint32_t>((point.x - m_quadtreeNodes[0].m_minPos.x) / nodeSize.x);
		uint32_t z = static_cast<uint32_t>((point.z - m_quadtreeNodes[0].m_minPos.z) / nodeSize.z);
		return bitSeparete(x) | (bitSeparete(z) << 1);
	}
	
	// ビット分割関数
	uint32_t bitSeparete(uint32_t n)
	{
		n = (n ^ (n << 8)) & 0x00ff00ff;
		n = (n ^ (n << 4)) & 0x0f0f0f0f;
		n = (n ^ (n << 2)) & 0x33333333;
		n = (n ^ (n << 1)) & 0x55555555;
		return n;
	}

	// ある空間内でのレイの交差判定
	bool IntersectVsRayInNode(
		uint32_t index,
		uint8_t mortonArea,
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& direction,
		float dist,
		HitResult& result,
		bool& hit);

	// ある空間内での球の衝突判定
	bool IntersectVsSphereInNode(
		uint32_t index,
		uint8_t mortonArea,
		DirectX::XMVECTOR& spherePos,
		float radius,
		bool& hit);

	// ある空間内でのカプセルの衝突判定
	bool IntersectVsCapsuleInNode(
		uint32_t index,
		uint8_t mortonArea,
		DirectX::XMVECTOR& capsulePos,
		const DirectX::XMVECTOR& direction,
		float radius,
		float length,
		bool& hit,
		HitResult* result = nullptr);

private:
	Node*		m_quadtreeNodes = nullptr;	// 四分木ノード配列ポインタ
	uint32_t	m_level = 0;		// 分割数
	uint32_t	m_linerLength = 0;	// 空間配列サイズ

	static const uint32_t	MAX_LEVEL = 9u;	// 最大分割数
	static constexpr uint32_t LevelStart[MAX_LEVEL + 1] =	// 線形四分木の各階層の開始インデックス。等比数列の和の公式を利用することで求まるが定数なのであらかじめ計算しておく
	{
		0, 1, 5, 21, 85, 341, 1365, 5461, 21845, 87381
	};
};

#endif // !__MAP_QUADTREE_H__
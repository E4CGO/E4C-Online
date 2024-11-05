//! @file OctreeNode.h
//! @note 八分木空間

#ifndef __Octree_Node_H__
#define __Octree_Node_H__

#include "TAKOEngine/Physics/Collision.h"

/**************************************************************************//**
	@class		OctreeNode
	@brief		八分木空間の一つのノード
	@par		この空間に存在するオブジェクトを登録する
*//***************************************************************************/
class OctreeNode
{
public:
	OctreeNode() {}
	OctreeNode(DirectX::XMFLOAT3 _center, float _halfSize) { this->m_center = _center; this->m_halfSize = _halfSize; }
	~OctreeNode() {}

public:
	DirectX::XMFLOAT3 GetCenter() const { return m_center; }
	void SetCenter(DirectX::XMFLOAT3 _center) { this->m_center = _center; }
	float GetHalfSize() const { return m_halfSize; }
	void SetHalfSize(float _halfSize) { this->m_halfSize = _halfSize; }

	std::vector<Triangle> GetTriangles() const { return m_triangles; }
	std::vector<Sphere*> GetSpheres() const { return m_spheres; }
	std::vector<AABB*> GetAABBs() const { return m_aabbs; }
	std::vector<Capsule*> GetCapsules() const { return m_capsules; }

	// オブジェクトを登録
	void InsertTriangleObject(Triangle& triangle) { m_triangles.push_back(triangle); }
	void InsertSphereObject(Sphere& sphere) { m_spheres.emplace_back(&sphere); }
	void InsertAABBObject(AABB& aabb) { m_aabbs.emplace_back(&aabb); }
	void InsertCapsuleObject(Capsule& capsule) { m_capsules.emplace_back(&capsule); }
	// オブジェクトを削除
	void ClearTriangleObject() { m_triangles.clear(); }
	void ClearSphereObject() { m_spheres.clear(); }
	void ClearAABBObject() { m_aabbs.clear(); }
	void ClearCapsuleObject() { m_capsules.clear(); }

private:
	DirectX::XMFLOAT3		m_center = {};		// 四分木空間の中心
	float					m_halfSize = {};	// 四分木空間の半辺長
	// 登録するオブジェクトを管理するマネージャー
	std::vector<Triangle>		m_triangles = {};		// 三角形登録用
	std::vector<Sphere*>		m_spheres = {};			// 球登録用
	std::vector<AABB*>			m_aabbs = {};			// AABB登録用
	std::vector<Capsule*>		m_capsules = {};		// カプセル登録用
};


/**************************************************************************//**
	@class		OctreeNodeManager
	@brief		八分木空間マネージャー
	@par		八分木空間ノードを線形上に並べ、様々な処理を執り行う
*//***************************************************************************/
class OctreeNodeManager
{
public:
	OctreeNodeManager()
	{
		m_depth = 0;
	}
	~OctreeNodeManager() {}

public:
	struct CrossedNode	// 交差したノードの記録用構造体
	{
		int			m_linerIndex = 0;	// 配列番号
		int			m_level = 0;		// 階層番号
	};

	// 線形八分木の階層のスタートインデックスの取得
	const uint32_t GetLevelStart(uint32_t level)
	{
		return LevelStart[level];
	}
	// 線形八分木のインデックスから階層を取得
	const uint32_t GetLevelFromIndex(uint32_t idx)
	{
		uint32_t ret = MAX_DEPTH;
		while (ret >= 0)
		{
			if (LevelStart[--ret] <= idx) break;
		}
		return ret;
	}

	// 八分木空間の生成
	void CreateOctree(DirectX::XMFLOAT3 center, float halfSize, uint32_t depth);

	// モートンコードの生成
	uint32_t GetMortonCode(const DirectX::XMFLOAT3& point, const OctreeNode& route, float halfSize);
	uint32_t bitSeparete(uint32_t n);

	// 同階層の隣のモートンコードを取得
	int GetNextMortonCode(uint32_t before, uint32_t shiftXYZ, bool minus);

	// 八分木空間へオブジェクトを登録する際の線形インデックスを取得
	uint32_t GetLinerIndexInsertObject(DirectX::XMFLOAT3 minPoint, DirectX::XMFLOAT3 maxPoint);

	// 八分木空間へ三角形オブジェクトを登録
	bool InsertTriangleObject(Triangle& triangle);
	// 八分木空間へ球体オブジェクトを登録
	bool InsertSphereObject(Sphere& sphere);
	// 八分木空間へAABBオブジェクトを登録
	bool InsertAABBObject(AABB& aabb);
	// 八分木空間へカプセルオブジェクトを登録
	bool InsertCapsuleObject(Capsule& capsule);

	// オブジェクトを八分木空間から削除
	void ClearAllTriangleObject();
	void ClearAllSphereObject();
	void ClearAllAABBObject();
	void ClearAllCapsuleObject();

	// 全てのノードで三角形と垂直レイの交差判定を行う（最初に交差する三角形のみを返す方式）
	bool IntersectVerticalRayVsTriangle(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd, HitResultVector& result);
	// 全てのノードで三角形とレイの交差判定を行う（最初に交差する三角形のみを返す方式）
	bool IntersectRayVsTriangle(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd, HitResultVector& result);

	// 全てのノードで三角形とスフィアキャストの交差判定を行う（最初に交差する三角形のみを返す方式）
	bool IntersectSphereCastVsTriangle(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd, float radius, HitResultVector& result);
	bool IntersectSphereCastVsTriangle(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd, float radius, HitResultVector& result, std::vector<CrossedNode>* nodeList);

	// 全てのオブジェクトの押し戻し処理を実行
	void CollisionAllObjects(
		uint32_t target,
		bool singleNode,
		std::vector<Sphere*>* upperListSphere = {},
		std::vector<Capsule*>* upperListCapsule = {},
		std::vector<AABB*>* upperListAABB = {},
		std::vector<Triangle>* upperListTriangle = {});

	// 指定したノード以下全てのノードで同じオブジェクト同士の押し戻し処理を実行
	void CollisionSphereVsSphere(uint32_t target, bool singleNode, std::vector<Sphere*>* upperListSphere);
	void CollisionCapsuleVsCapsule(uint32_t target, bool singleNode, std::vector<Capsule*>* upperListCapsule);
	void CollisionAABBVsAABB(uint32_t target, bool singleNode, std::vector<AABB*>* upperListAABB);

	// 指定したノード以下全てのノードでオブジェクトと三角形の押し戻し処理を実行　※三角形側は動かさない
	void CollisionSphereVsTriangle(uint32_t target, bool singleNode, std::vector<Sphere*>* upperListSphere, std::vector<Triangle>* upperListTriangle);
	void CollisionCapsuleVsTriangle(uint32_t target, bool singleNode, std::vector<Capsule*>* upperListCapsule, std::vector<Triangle>* upperListTriangle);
	void CollisionAABBVsTriangle(uint32_t target, bool singleNode, std::vector<AABB*>* upperListAABB, std::vector<Triangle>* upperListTriangle);

	// 指定したノード以下全てのノードでオブジェクトとオブジェクトの押し戻し処理を実行
	void CollisionSphereVsCapsule(uint32_t target, bool singleNode, std::vector<Sphere*>* upperListSphere, std::vector<Capsule*>* upperListCapsule);
	void CollisionSphereVsAABB(uint32_t target, bool singleNode, std::vector<Sphere*>* upperListSphere, std::vector<AABB*>* upperListAABB);
	void CollisionCapsuleVsAABB(uint32_t target, bool singleNode, std::vector<Capsule*>* upperListCapsule, std::vector<AABB*>* upperListAABB);

private:
	std::vector<OctreeNode>	m_octreeNodes = {};	// 八分木ノード配列
	uint32_t	m_depth = 0;	// 分割数

	static const uint32_t	MAX_DEPTH = 8u;	// 最大分割数
	static constexpr uint32_t LevelStart[MAX_DEPTH] =	// 線形八分木の各階層の開始インデックス。等比数列の和の公式を利用することで求まるが定数なのであらかじめ計算しておく
	{
		0, 1, 9, 73, 585, 4681, 37449, 299593
	};
};
#endif // !__Octotree_Node_H__
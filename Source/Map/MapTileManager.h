﻿#pragma once

#include "TAKOEngine/Tool/Singleton.h"

#include "GameObject/ObjectManager.h"
#include "GameObject/ModelObject.h"
#include "TAKOEngine/Physics/QuadtreeNode.h"
#include "TAKOEngine/Physics/OctreeNode.h"
#include "TAKOEngine/Physics/Liner8TreeManager.h"

class MapTileManager : public ObjectManager<ModelObject>, public Singleton<MapTileManager>
{
	friend class Singleton<MapTileManager>;
protected:
	MapTileManager() = default;
	~MapTileManager() { tree.Finalize(); }
public:
	void Clear() override;

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit, bool camera = false);
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& direction, float dist, HitResult& hit, bool camera = false);
	// 垂直レイキャスト
	bool VerticalRayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	// オブジェクトのコリダーを押し戻す
	//bool IntersectColliderVsMap(Collider* collider);

	// 球の押し戻し
	bool IntersectSphereVsMap(Sphere& sphere, bool wallCheck = false);

	// カプセルの押し戻し
	bool IntersectCapsuleVsMap(Capsule& capsule, bool wallCheck = false);

	// 空間生成
	void CreateSpatialIndex(uint32_t quadDepth = 1, uint32_t octDepth = 1, DirectX::XMFLOAT3* minPos = nullptr, DirectX::XMFLOAT3* maxPos = nullptr);

	// 四分木空間へ三角形オブジェクトを登録
	bool InsertTriangleObject(Triangle& triangle) { quadtree.InsertTriangleObject(triangle); }
	// 四分木空間へ球体オブジェクトを登録
	bool InsertSphereObject(Sphere& sphere) { quadtree.InsertSphereObject(sphere); }
	// 四分木空間へAABBオブジェクトを登録
	bool InsertAABBObject(AABB& aabb) { quadtree.InsertAABBObject(aabb); }
	// 四分木空間へカプセルオブジェクトを登録
	bool InsertCapsuleObject(Capsule& capsule) { quadtree.InsertCapsuleObject(capsule); }

protected:
	// マップサイズ計算
	void CalcMapArea(DirectX::XMFLOAT3& minPos, DirectX::XMFLOAT3& maxPos);

	// マップのメッシュを登録(返り値はメッシュの三角形の数)
	int InsertMapMesh();

	// 子空間探索
	bool SearchChildren(	// レイ探索
		int Elem,
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& direction,
		float dist,
		HitResult& result,
		bool& hit);
	bool SearchChildren(	// 球探索
		int Elem,
		DirectX::XMVECTOR& spherePos,
		float radius,
		bool wallCheck,
		bool& hit);
	bool SearchChildren(	// カプセル探索
		int Elem,
		DirectX::XMVECTOR& capsulePos,
		const DirectX::XMVECTOR& direction,
		float radius,
		float length,
		bool wallCheck,
		bool& hit);

	// 親空間探索
	bool SearchParent(	// レイ探索
		int Elem,
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& direction,
		float dist,
		HitResult& result,
		bool& hit);
	bool SearchParent(	// 球探索
		int Elem,
		DirectX::XMVECTOR& spherePos,
		float radius,
		bool wallCheck,
		bool& hit);
	bool SearchParent(	// カプセル探索
		int Elem,
		DirectX::XMVECTOR& capsulePos,
		const DirectX::XMVECTOR& direction,
		float radius,
		float length,
		bool wallCheck,
		bool& hit);

private:
	QuadtreeNodeManager quadtree;
	OctreeNodeManager octree;
	Liner8TreeManager<Triangle> tree;
};

#define MAPTILES MapTileManager::Instance()
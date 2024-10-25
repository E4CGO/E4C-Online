#pragma once

#include "TAKOEngine/Tool/Singleton.h"

#include "GameObject/ObjectManager.h"
#include "GameObject/ModelObject.h"
#include "TAKOEngine/Physics/QuadtreeNode.h"

class MapTileManager : public ObjectManager<ModelObject>, public Singleton<MapTileManager>
{
	friend class Singleton<MapTileManager>;
protected:
	MapTileManager() = default;
	~MapTileManager() = default;
public:
	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit, bool camera = false);
	// 垂直レイキャスト
	bool VerticalRayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	// 四分木空間生成
	void CreateQuadtree(DirectX::XMFLOAT3 center, float halfSize, uint32_t depth);
	void CreateQuadtree(uint32_t depth = 1);

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

private:
	QuadtreeNodeManager quadtree;
};

#define MAPTILES MapTileManager::Instance()
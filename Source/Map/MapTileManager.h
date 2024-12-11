#pragma once

#include "TAKOEngine/Tool/Singleton.h"

#include "GameObject/ObjectManager.h"
#include "GameObject/ModelObject.h"
#include "TAKOEngine/Physics/MapQuadtree.h"

class MapTileManager : public ObjectManager<ModelObject>, public Singleton<MapTileManager>
{
	friend class Singleton<MapTileManager>;
protected:
	MapTileManager() = default;
	~MapTileManager() { mapQuadtree.ClearQuadtree(); }
public:
	void Clear() override;

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit, bool camera = false);
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& direction, float dist, HitResult& hit, bool camera = false);

	// スフィアキャスト
	bool SphereCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& direction, float dist, float radius, HitResult& hit);

	// 球の押し戻し
	bool IntersectSphereVsMap(Sphere& sphere);

	// カプセルの押し戻し
	bool IntersectCapsuleVsMap(Capsule& capsule);

	// 空間生成
	void CreateSpatialIndex(uint32_t quadDepth = 1, uint32_t octDepth = 1, DirectX::XMFLOAT3* minPos = nullptr, DirectX::XMFLOAT3* maxPos = nullptr);

protected:
	// マップサイズ計算
	void CalcMapArea(DirectX::XMFLOAT3& minPos, DirectX::XMFLOAT3& maxPos);

	// マップのメッシュを登録(返り値はメッシュの三角形の数)
	int InsertMapMesh();

private:
	MapQuadtree mapQuadtree;
};

#define MAPTILES MapTileManager::Instance()
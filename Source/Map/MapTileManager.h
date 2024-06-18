#pragma once

#include "TAKOEngine/Tool/Singleton.h"

#include "GameObject/ObjectManager.h"
#include "GameObject/ModelObject.h"

class MapTileManager : public ObjectManager<ModelObject>, public Singleton<MapTileManager>
{
	friend class Singleton<MapTileManager>;
protected:
	MapTileManager() = default;
	~MapTileManager() = default;
public:
	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit, bool camera = false);
};

#define MAPTILES MapTileManager::Instance()
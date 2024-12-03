#pragma once

#include <vector>

#include "TAKOEngine/Tool/Singleton.h"
#include "TAKOEngine/Physics/Collider/Collider.h"

#include "Manager.h"

class MapCollisionManager : public Manager<Collider>, public Singleton<MapCollisionManager>
{
	friend class Singleton<MapCollisionManager>;
protected:
	MapCollisionManager() {}
	~MapCollisionManager();
public:
	// デバッグ情報の表示
	void DrawDebugGUI();

	// デバッグプリミティブの表示
	void DrawDebugPrimitive();
};

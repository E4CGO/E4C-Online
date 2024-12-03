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
	// �f�o�b�O���̕\��
	void DrawDebugGUI();

	// �f�o�b�O�v���~�e�B�u�̕\��
	void DrawDebugPrimitive();
};

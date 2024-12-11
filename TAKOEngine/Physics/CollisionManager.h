#pragma once

#include "TAKOEngine/Tool/Singleton.h"
#include "TAKOEngine/Physics/Collider.h"
#include "Source/Manager.h"

class CollisionManager : public Manager<Collider>, public Singleton<CollisionManager>
{
	friend class Singleton<CollisionManager>;
private:
	CollisionManager() {};
	~CollisionManager() = default;
public:
	//void Clear() override { items.clear(); }
	void Contacts();
};

#define COLLISIONS CollisionManager::Instance()
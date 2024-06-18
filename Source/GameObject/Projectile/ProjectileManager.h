#pragma once

#include "TAKOEngine/Tool/Singleton.h"

#include "GameObject/ObjectManager.h"
#include "GameObject/Projectile/Projectile.h"

class ProjectileManager : public ObjectManager<Projectile>, public Singleton<ProjectileManager>
{
	friend class Singleton<ProjectileManager>;
protected:
	ProjectileManager() = default;
	~ProjectileManager() = default;
};

#define PROJECTILES ProjectileManager::Instance()
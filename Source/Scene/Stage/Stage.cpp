#include "Map/MapTileManager.h"
#include "Scene/Stage/Stage.h"
#include "GameObject/Character/Enemy/EnemyManager.h"
#include "TAKOEngine/Rendering/LightManager.h"
#include "GameObject/Props/SpawnerManager.h"
#include "TAKOEngine/Physics/CollisionManager.h"

void Stage::Initialize()
{
	m_timer = 0.0f;

	MAPTILES.Clear();
	ENEMIES.Clear();
	SpawnerManager::Instance().Clear();
	
}

void Stage::Finalize()
{
	MAPTILES.Clear();
	ENEMIES.Clear();
	SpawnerManager::Instance().Clear();
	LightManager::Instance().Clear();
}

void Stage::Update(float elapsedTime)
{
	m_timer += elapsedTime;
}
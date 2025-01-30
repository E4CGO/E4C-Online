#include "Projectile.h"

#include "TAKOEngine/Physics/CollisionDataManager.h"

#include "GameObject/Projectile/ProjectileManager.h"
#include "GameObject/Character/Enemy/EnemyManager.h"
#include "GameObject/Character/Player/PlayerCharacterManager.h"

#include "Map/MapTileManager.h"

#define THRESHOLD 0.001f

void Projectile::Update(float elapsedTime)
{
	ModelObject::Update(elapsedTime);
	m_currentTimer -= elapsedTime;
}

void Projectile::Render(const RenderContext& rc)
{
	ModelObject::Render(rc);
#ifdef _DEBUG
	if (m_pMoveCollider) m_pMoveCollider->DrawDebugPrimitive({ 1, 1, 1, 1 });
#endif // _DEBUG
}

void Projectile::RenderDX12(const RenderContextDX12& rc)
{
	ModelObject::RenderDX12(rc);
}

void Projectile::Destory()
{
	OnDestory();
	PROJECTILES.Remove(this);
}
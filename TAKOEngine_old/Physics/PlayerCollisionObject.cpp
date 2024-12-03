#include "TAKOEngine/Physics/PlayerCollisionObject.h"

#include "GameObject/Character/Player/PlayerManager.h"

#include "GameData.h"

PlayerCollisionObject::PlayerCollisionObject(float radius)
{
	collider = std::make_unique<SphereCollider>(Collider::COLLIDER_OBJ::PLAYER, &transform);
	Sphere sphere({}, radius);
	collider->SetParam(sphere);
}

void PlayerCollisionObject::Update(float elapsedTime)
{
	Player* player = PLAYERS.GetPlayerById(GAME_DATA.GetClientId());

	if (!player) return;

	HitResult hit;
	Collider* col = collider.get();
	if (player->GetCollider()->Collision(col, {}, hit))
	{
		if (!isHit) OnHit(player);
		isHit = true;
	}
	else
	{
		if (isHit) OnLeave(player);
		isHit = false;
	}
}

void PlayerCollisionObject::Render(const RenderContext& rc)
{
#ifdef _DEBUG
	collider->DrawDebugPrimitive({ 1, 1, 1, 1 });
#endif
}

void PlayerCollisionObject::RenderDX12(const RenderContextDX12& rc)
{
}

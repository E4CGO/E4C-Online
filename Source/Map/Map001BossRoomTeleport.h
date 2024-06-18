#pragma once

#include "TAKOEngine/Physics/PlayerCollisionObject.h"

class Map001BossRoomTeleport : public PlayerCollisionObject
{
public:
	Map001BossRoomTeleport();
	~Map001BossRoomTeleport() {};

	// XVˆ—
	virtual void Update(float elapsedTime) override;
	// •`‰æˆ—
	void Render(const RenderContext& rc) override;

protected:
	void OnHit(Player* player);
};

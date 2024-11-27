#pragma once

#include "TAKOEngine/Physics/PlayerCollisionObject.h"

class Map001BossRoomTeleport : public PlayerCollisionObject
{
public:
	Map001BossRoomTeleport();
	~Map001BossRoomTeleport() {};

	// 更新処理
	virtual void Update(float elapsedTime) override;
	// 描画処理
	void Render(const RenderContext& rc) override;

protected:
	void OnHit(Player* player);
};

#pragma once

#include <memory>

#include "GameObject/GameObject.h"
#include "Scene\Stage\Stage.h"
#include "Source/GameObject/Props/Spawner.h"
#include "Source/GameObject/Character/Enemy/SkeletonMinion.h"

class TestingStage : public Stage
{
public:
	TestingStage() = default;
	void Initialize() override;
	void Update(float elapsedTime) override;
	void Render() override;

	void RenderDX12() override;
protected:
	void OnPhase() override;
public:
	enum PHASE
	{
		NORMAL,
		GET_KEY,
		BOSS,
		VICKORY,
	};
private:
	GameObject* key;
	GameObject* bossRoomTeleport;
	GameObject* spawners;
};

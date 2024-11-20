#pragma once

#include <memory>

#include "GameObject/GameObject.h"
#include "Scene\Stage\Stage.h"
#include "Source/GameObject/Props/Spawner.h"

class TestingStage : public Stage
{
public:
	TestingStage() = default;

	void Initialize() override;

	void Finalize();

	void Update(float elapsedTime) override;

	void Render() override;
protected:
	void OnPhase() override;
public:
	enum PHASE
	{
		NORMAL,
		GET_KEY,
		BOSS,
		VICKORY,
		MONSTERRUSH,
	};
private:
	GameObject* key;
	GameObject* bossRoomTeleport;
	GameObject* spawner;

	float spawntime = 5.f;
	float spawntimer = 0.f;

	DirectX::XMFLOAT3 spawnPos{};

	Spawner*spawners=nullptr;


	const int maxEnemies = 20;             // 全体で生成可能なエネミーの数
	const int maxEnemiesInRange = 3;       // レンジ内に同時に存在可能なエネミーの数
	int spawnedEnemyCount = 0;             // 現在までに生成されたエネミーの数
	int enemiesInRangeCount = 0;           // レンジ内に存在するエネミーの数
};

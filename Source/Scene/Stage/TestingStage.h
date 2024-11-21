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

	void SpawnerEnemy(float elapsedTime, int phase, float time, int enemyType);
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
	Spawner* spawners = nullptr;

	DirectX::XMFLOAT3 spawnPos{};

	int spawnedEnemyCount = 0;             // ���݂܂łɐ������ꂽ�G�l�~�[�̐�
	float spawntimer = 0.f;
	float spawntime = 5.f;
	int maxEnemies = 20;
	
};

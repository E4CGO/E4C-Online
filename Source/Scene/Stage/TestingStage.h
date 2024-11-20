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


	const int maxEnemies = 20;             // �S�̂Ő����\�ȃG�l�~�[�̐�
	const int maxEnemiesInRange = 3;       // �����W���ɓ����ɑ��݉\�ȃG�l�~�[�̐�
	int spawnedEnemyCount = 0;             // ���݂܂łɐ������ꂽ�G�l�~�[�̐�
	int enemiesInRangeCount = 0;           // �����W���ɑ��݂���G�l�~�[�̐�
};

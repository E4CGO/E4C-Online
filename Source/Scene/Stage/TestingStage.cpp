#include "TestingStage.h"

#include "GameObject\ModelObject.h"

#include "GameObject/Character/Enemy/EnemyManager.h"
#include "GameObject/Character/Enemy/Enemy.h"
#include "Scene/Stage/StageManager.h"

#include "Map/MapTileManager.h"
#include "Map/Map001.h"
#include "Map/Map001Key.h"
#include "Map/Map001BossRoomTeleport.h"
#include "TAKOEngine/Tool/Mathf.h"


void TestingStage::Initialize()
{
	Stage::Initialize(); // デフォルト

	STAGES.SetStage(this);

	// 床
	MAPTILES.Register(new Map001);

	key = STAGES.Register(new Map001Key);
	bossRoomTeleport = STAGES.Register(new Map001BossRoomTeleport);
	spawners =new Spawner;
	spawners->SetPosition({ -38.0f, 2.0f, -48.0f });
	
	enemyList.push_back({ PHASE::NORMAL, 0.0, ENEMY_TYPE::CHEST_EMPTY_SKELETON_MINION, { -40.0f, 0.0f, -15.0f }, { 0, DirectX::XMConvertToRadians(180), 0} });

	enemyList.push_back({ PHASE::NORMAL, 0.0, ENEMY_TYPE::SKELETON_MINION, { 0.0f, 20.0f, -12.0f } });
	enemyList.push_back({ PHASE::NORMAL, 0.0, ENEMY_TYPE::SKELETON_MINION, { -1.0f, 20.0f, -18.0f } });
	enemyList.push_back({ PHASE::NORMAL, 0.0, ENEMY_TYPE::SKELETON_MINION, { +1.0f, 20.0f, -22.0f } });
	enemyList.push_back({ PHASE::NORMAL, 0.0, ENEMY_TYPE::SKELETON_MINION, { +1.0f, 20.0f, -22.0f } });

	// KEYの回り
	DirectX::XMFLOAT3 pos = key->GetPosition();
	float d = 5.0f;
	for (int i = 0; i < 10; i++)
	{
		/*enemyList.push_back({ PHASE::GET_KEY, 0.0, ENEMY_TYPE::SKELETON_MINION, {
			pos.x + (cosf(DirectX::XMConvertToRadians(90.0f * i)) * d),
			20.0f,
			pos.z + (sinf(DirectX::XMConvertToRadians(90.0f * i)) * d),
			}
			}
		);*/
	}

	float sec = 0.0f;
	for (int i = 0; i < 1; i++)
	{
		enemyList.push_back({ PHASE::BOSS, sec, ENEMY_TYPE::SKELETON_MINION, { 26.0f, 10.0f, -86.0f } });
		enemyList.push_back({ PHASE::BOSS, sec, ENEMY_TYPE::SKELETON_MINION, { 14.0f, 10.0f, -86.0f } });
		enemyList.push_back({ PHASE::BOSS, sec, ENEMY_TYPE::SKELETON_MINION, { 26.0f, 10.0f, -74.0f } });
		enemyList.push_back({ PHASE::BOSS, sec, ENEMY_TYPE::SKELETON_MINION, { 14.0f, 10.0f, -74.0f } });
		sec += 5.0f;
	}
	// BOSS
	enemyList.push_back({ PHASE::BOSS, 0.0, ENEMY_TYPE::SKELETON_MINION_BOSS, { 20.0f, 40.0f, -80.0f } });
	// 宝箱
	enemyList.push_back({ PHASE::VICKORY, 0.0f, ENEMY_TYPE::CHEST_COIN, { 20.0f, 20.0f, -80.0f } });

	//enemyList.push_back({ PHASE::MONSTERRUSH,0.f,ENEMY_TYPE::SKELETON_MINION_BOSS,{key->GetPosition()} });

	
}
void TestingStage::Finalize()
{
	if (spawners != nullptr)
	{
		delete spawners;
		spawners = nullptr;
	}
}

void TestingStage::Update(float elapsedTime)
{
	Stage::Update(elapsedTime);
	spawners->Update(elapsedTime);
	if (spawners->SearchPlayer())
	{
		SpawnEnemy(elapsedTime, TestingStage::PHASE::MONSTERRUSH, 0.f, ENEMY_TYPE::SKELETON_MINION);
	}
	if (phase == PHASE::BOSS && ENEMIES.Count() == 0) // ボス部屋敵全滅
	{
		SetPhase(PHASE::VICKORY);
	}
}
void TestingStage::Render()
{
}
void TestingStage::SpawnEnemy(float elapsedTime, int phase, float time, int enemyType)
{
	float theta = Mathf::RandomRange(0, DirectX::XM_2PI);
	float range = Mathf::RandomRange(0, spawners->GetTerrytoryRange());

	spawnPos.x = spawners->GetPosition().x + spawners->GetTerrtoryOrigin().x + range * sinf(theta);
	spawnPos.y = 2.0f;
	spawnPos.z = spawners->GetPosition().z + spawners->GetTerrtoryOrigin().z + range * cosf(theta);


	spawntimer += elapsedTime;
	if (spawntimer > spawntime && spawnedEnemyCount < maxEnemies)
	{
		// 一度に3匹のモンスターをスポーン（ただし、生成可能なエネミー数を確認）
		int spawnCount = min(3, maxEnemies - spawnedEnemyCount); // 残り生成可能数を計算
		for (int i = 0; i < spawnCount; ++i)
		{
			float offsetX = static_cast<float>(i) * 2.0f - 2.0f; // スポーン位置を調整
			float offsetZ = static_cast<float>(i) * 4.0f - 2.0f;
			enemyList.push_back({
				phase,
				time,
				enemyType,
				{ spawnPos.x + offsetX,spawnPos.y,spawnPos.z + offsetZ}

				});

			spawnedEnemyCount++; // 生成したエネミー数をカウント
		}
		spawntimer = 0; // タイマーをリセット
	}

}
void TestingStage::OnPhase()
{
	if (phase == PHASE::BOSS) // ボス出現時・現存敵を削除
	{
		ENEMIES.Clear();
	}
}
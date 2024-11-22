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
#include "Source/GameObject/Character/Enemy/SkeletonMinion.h"

void TestingStage::Initialize()
{
	Stage::Initialize(); // デフォルト
	STAGES.SetStage(this);
	// 床
	MAPTILES.Register(new Map001);

	key = STAGES.Register(new Map001Key);
	bossRoomTeleport = STAGES.Register(new Map001BossRoomTeleport);
	spawners = STAGES.Register(new Spawner);
	spawners->SetPosition({ -38.0f, 0.0f, -48.0f });
	
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
}
void TestingStage::Update(float elapsedTime)
{
	Stage::Update(elapsedTime);
	if (phase == PHASE::BOSS && ENEMIES.Count() == 0) // ボス部屋敵全滅
	{
		SetPhase(PHASE::VICKORY);
	}
}
void TestingStage::Render()
{
}

void TestingStage::OnPhase()
{
	if (phase == PHASE::BOSS) // ボス出現時・現存敵を削除
	{
		ENEMIES.Clear();
	}
}
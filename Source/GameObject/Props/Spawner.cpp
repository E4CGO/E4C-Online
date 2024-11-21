#include "Spawner.h"
#include "GameObject/Character/Player/PlayerManager.h"
#include "Scene/Stage/StageManager.h"
#include "TAKOEngine/Tool/Mathf.h"
#include "Source/Scene/Stage/TestingStage.h"
#include <GameObject/Character/Enemy/EnemyManager.h>
/**************************************************************************//**
     @brief  コンストラクタ  
*//***************************************************************************/
Spawner::Spawner()
{
	
}
/**************************************************************************//**
     @brief    更新
    @param[in]    elapsedTime
*//***************************************************************************/
void Spawner::Update(float elapsedTime)
{
	if (SearchPlayer())
	{
		SpawnEnemy(elapsedTime);
	}
}
/**************************************************************************//**
     @brief    テリトリー配置
    @param[in]    origin
    @param[in]    range
*//***************************************************************************/
void Spawner::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;
	territoryRange = range;
}
/**************************************************************************//**
     @brief    プレイヤー発見
    @return    
*//***************************************************************************/
bool Spawner::SearchPlayer()
{
	for (Player*& player : PLAYERS.GetAll())
	{
		// プレイヤーの位置を取得
		playerPosition = player->GetPosition();

		// 自身とプレイヤーの間の距離を計算
		float vx = playerPosition.x - position.x;
		float vy = playerPosition.y - position.y;
		float vz = playerPosition.z - position.z;
		float dist = sqrtf(vx * vx + vy * vy + vz * vz);

		// 検知範囲内か確認
		if (dist < serchRange)
		{
			return true;
			
		}
		return false;
	}
}
void Spawner::SpawnEnemy(float elapsedTime)
{
	float theta = Mathf::RandomRange(0, DirectX::XM_2PI);
	float range = Mathf::RandomRange(0, territoryRange);

	spawnPos.x =position.x + territoryOrigin.x + range * sinf(theta);
	spawnPos.y = 2.0f;
	spawnPos.z = position.z +territoryOrigin.z + range * cosf(theta);


	spawntimer += elapsedTime;
	if (spawntimer > spawntime && spawnedEnemyCount < maxEnemies)
	{
		// 一度に3匹のモンスターをスポーン（ただし、生成可能なエネミー数を確認）
		int spawnCount = min(3, maxEnemies - spawnedEnemyCount); // 残り生成可能数を計算
		for (int i = 0; i < spawnCount; ++i)
		{
			float offsetX = static_cast<float>(i) * 2.0f - 2.0f; // スポーン位置を調整
			float offsetZ = static_cast<float>(i) * 1.0f - 1.0f;
			SkeletonMinion* enemy = new SkeletonMinion();
			enemy->SetPosition({ spawnPos.x + offsetX,spawnPos.y,spawnPos.z + offsetZ });
			ENEMIES.Register(enemy);

			spawnedEnemyCount++; // 生成したエネミー数をカウント
		}
		spawntimer = 0; // タイマーをリセット
	}

}
/**************************************************************************//**
     @brief    描画
    @param[in]    rc
*//***************************************************************************/
void Spawner::Render(const RenderContext& rc)
{

}

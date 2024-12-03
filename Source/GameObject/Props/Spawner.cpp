#include "Spawner.h"
#include "Scene/Stage/StageManager.h"
#include "TAKOEngine/Tool/Mathf.h"
#include <GameObject/Character/Enemy/EnemyManager.h>
#include "GameObject/Character/Player/PlayerCharacterManager.h"
#include "TAKOEngine/Tool/XMFLOAT.h"
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
	for (auto& player : PlayerCharacterManager::Instance().GetAll())
	{
		// プレイヤーの位置を取得
		playerPosition = player->GetPosition();

		// 自身とプレイヤーの間の距離を計算
		DirectX::XMFLOAT3 v = playerPosition - position;
		float dist = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);

		// 検知範囲内か確認
		if (dist < serchRange)
		{
			return true;
			
		}
		return false;
	}
}
/**************************************************************************//**
     @brief    エネミー生成
    @param[in]    elapsedTime
    @param[in]    enemyType
    @param[in]    maxEnemies
    @param[in]    activeEnemies
*//***************************************************************************/
void Spawner::SpawnEnemy(float elapsedTime, const std::string& enemyType, int maxEnemies, int activeEnemies)
{
spawntimer += elapsedTime;

// 不要な敵を整理
CleanupEnemies();

// 現在の生存している敵の数を確認
currentAliveCount = spawnedEnemies.size();

// 敵が3匹未満かつ合計生成数がmaxEnemiesを超えない場合にのみスポーン処理を実行
if (spawntimer > spawntime && currentAliveCount < activeEnemies && spawnedEnemyCount < maxEnemies) {
	// 生成可能な数を計算
	int spawnCount = min( activeEnemies - currentAliveCount, maxEnemies - spawnedEnemyCount );

	for (int i = 0; i < spawnCount; ++i) {
		// スポーン位置の計算
		float theta = Mathf::RandomRange(0, DirectX::XM_2PI);
		float range = Mathf::RandomRange(0, territoryRange);
		float offsetX = static_cast<float>(i) * 2.0f - 2.0f; // スポーン位置を調整
		float offsetZ = static_cast<float>(i) * 1.0f - 1.0f;

		DirectX::XMFLOAT3 spawnPos;
		spawnPos.x = position.x + territoryOrigin.x + range * sinf(theta) + offsetX;
		spawnPos.y = 10.0f;
		spawnPos.z = position.z + territoryOrigin.z + range * cosf(theta) + offsetZ;

		// 敵を生成
		Enemy* enemy = SetEnemy(enemyType);
		if (enemy) {
			enemy->SetPosition(spawnPos);

			// 管理リストに追加
			spawnedEnemies.push_back(enemy);
			ENEMIES.Register(enemy);

			// カウンターを更新
			spawnedEnemyCount++;
            
			currentAliveCount++;
		}
	}
	spawntimer = 0; // タイマーをリセット
}
}
/**************************************************************************//**
     @brief   リスト削除
*//***************************************************************************/
void Spawner::CleanupEnemies() {
	spawnedEnemies.erase(
		std::remove_if(spawnedEnemies.begin(), spawnedEnemies.end(),
			[this](Enemy* enemy) {
				if (!enemy->IsAlive()) {
					currentAliveCount--; // カウンターを更新
					return true;  // リストから削除
				}
				return false;
			}),
		spawnedEnemies.end());
}
/**************************************************************************//**
     @brief    エネミーの種類をセット
    @param[in]    enemyType
    @return    
*//***************************************************************************/
Enemy* Spawner::SetEnemy(const std::string& enemyType)
{
	if (enemyType == "Skeleton") {
		return new SkeletonMinion();
	}
	else if (enemyType == "SkeletonBoss")
	{
		return new SkeletonMinionBoss();
	}
}

/**************************************************************************//**
     @brief    描画
    @param[in]    rc
*//***************************************************************************/
void Spawner::Render(const RenderContext& rc)
{
#ifdef DEBUG
	T_GRAPHICS.GetDebugRenderer()->DrawCylinder(position, territoryRange, 1.5f, { 1,0,0,1 });
	T_GRAPHICS.GetDebugRenderer()->DrawCylinder(position, serchRange, 1.5f, { 1,0,1,1 });
#endif // DEBUG

	
}

void Spawner::RenderDX12(const RenderContextDX12& rc)
{

#ifdef DEBUG
	T_GRAPHICS.GetDebugRenderer()->DrawCylinder(position, territoryRange, 1.5f, { 1,0,0,1 });
	T_GRAPHICS.GetDebugRenderer()->DrawCylinder(position, serchRange, 1.5f, { 1,0,1,1 });
#endif // DEBUG
}

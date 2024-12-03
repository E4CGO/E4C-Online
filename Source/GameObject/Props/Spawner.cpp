//! @file Spawner.cpp
//! @note 

#include "Spawner.h"
#include "GameObject/Character/Enemy/EnemyManager.h"
#include "GameObject/Character/Player/PlayerCharacterManager.h"
#include "TAKOEngine/Tool/XMFLOAT.h"
#include "Source/GameObject/Character/Enemy/SkeletonMinion.h"
/**************************************************************************//**
     @brief  コンストラクタ  
*//***************************************************************************/
Spawner::Spawner(uint8_t enemyType, int maxExistedEnemiesNum, int maxSpawnEnemiesNum) : m_enemyType(enemyType), m_maxExistedEnemiesNum(maxExistedEnemiesNum), m_maxSpawnedEnemiesNum(maxSpawnEnemiesNum), GameObject()
{
	
}

/**************************************************************************//**
     @brief    更新
    @param[in]    elapsedTime
*//***************************************************************************/
void Spawner::Update(float elapsedTime)
{
	if (m_maxSpawnedEnemiesNum > 0 && m_maxSpawnedEnemiesNum <= m_spawnedCountTotal) return; // 生成終了
	if (SearchPlayer())
	{
		m_spawnTimer += elapsedTime;
		while (m_spawnTimer > m_spawnTime)
		{
			m_spawnTimer -= m_spawnTime;
			Spawn();
		}
	}
	else
	{
		m_spawnTimer = 0.0f;
	}
}

/**************************************************************************//**
    @brief		プレイヤー検索
	@return		判定
*//***************************************************************************/
bool Spawner::SearchPlayer()
{
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
	if (player == nullptr) return false;

	DirectX::XMFLOAT3 d = player->GetPosition() - position;
	d.y = 0.0f; // 高さ無視

	return (XMFLOAT3LengthSq(d) < m_searchRadius * m_searchRadius);
}

/**************************************************************************//**
 	@brief	エネミー生成
*//***************************************************************************/
void Spawner::Spawn()
{
	// TODO NETWORK
	m_spawnedCountTotal++;

	m_pSpawnedEnemies.insert(ENEMIES.Register(Enemy::EnemyFactory(m_enemyType)));
}

/**************************************************************************//**
 	@brief		生成した敵が消滅時コールバック
	@param[in]	enemy エネミー参照ポインタ
*//***************************************************************************/
void Spawner::EnemyDestoryCallBack(Enemy* enemy)
{
	if (m_pSpawnedEnemies.find(enemy) != m_pSpawnedEnemies.end())
	{
		m_pSpawnedEnemies.erase(enemy);
	}
}

/**************************************************************************//**
     @brief    描画
    @param[in]    rc
*//***************************************************************************/
void Spawner::Render(const RenderContext& rc)
{
#ifdef _DEBUG
	T_GRAPHICS.GetDebugRenderer()->DrawCylinder(position, m_spawnRadius, 1.5f, { 1,0,0,1 });
	T_GRAPHICS.GetDebugRenderer()->DrawCylinder(position, m_searchRadius, 1.5f, { 1,0,1,1 });
#endif // DEBUG

	
}

void Spawner::RenderDX12(const RenderContextDX12& rc)
{

#ifdef _DEBUG
	T_GRAPHICS.GetDebugRenderer()->DrawCylinder(position, m_spawnRadius, 1.5f, { 1,0,0,1 });
	T_GRAPHICS.GetDebugRenderer()->DrawCylinder(position, m_searchRadius, 1.5f, { 1,0,1,1 });
#endif // DEBUG
}

//! @file Spawner.cpp
//! @note 

#include "Spawner.h"
#include "GameObject/Character/Enemy/EnemyManager.h"
#include "GameObject/Character/Player/PlayerCharacterManager.h"
#include "TAKOEngine/Tool/XMFLOAT.h"
#include "TAKOEngine/Tool/Mathf.h"
#include "Source/GameObject/Character/Enemy/SkeletonMinion.h"
/**************************************************************************//**
     @brief  コンストラクタ  
*//***************************************************************************/
Spawner::Spawner(uint8_t enemyType, int maxExistedEnemiesNum, int maxSpawnEnemiesNum) : m_enemyType(enemyType), m_maxExistedEnemiesNum(maxExistedEnemiesNum), m_maxSpawnedEnemiesNum(maxSpawnEnemiesNum), GameObject()
{
	m_cylinderRenderer = std::make_unique<CylinderRenderer>(T_GRAPHICS.GetDeviceDX12(), 2);
}

/**************************************************************************//**
     @brief    更新
    @param[in]    elapsedTime
*//***************************************************************************/
void Spawner::Update(float elapsedTime)
{
	if (m_maxSpawnedEnemiesNum > 0 && m_maxSpawnedEnemiesNum <= m_spawnedCountTotal) return; // 生成終了
	if (m_maxExistedEnemiesNum <= m_pSpawnedEnemies.size()) return;
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

	Enemy* enemy = Enemy::EnemyFactory(m_enemyType);

	// ランダムポイント
	float angle = Mathf::RandomRange(0.0f, DirectX::XM_2PI);
	float distance = Mathf::RandomRange(0.0f, m_spawnRadius);
	DirectX::XMFLOAT3 offset = { cosf(angle) * distance, 0.0f, sinf(angle) * distance };

	enemy->SetPosition(position + offset);
	enemy->SetSpawnPosition(position + offset);
	enemy->SetSpawner(this);
	m_pSpawnedEnemies.insert(ENEMIES.Register(enemy));
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

	m_cylinderRenderer->SetCylinder(position, m_spawnRadius, 1.0f, { 1, 0, 0, 0 });
	m_cylinderRenderer->Render(rc);

	m_cylinderRenderer->SetCylinder(position, m_searchRadius, 1.0f, { 1, 0, 0, 0 });
	m_cylinderRenderer->Render(rc);

#endif // DEBUG
}
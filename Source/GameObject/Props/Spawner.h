//! @file Spawner.h
//! @note 
//! 
#ifndef __INCLUDED_SPAWNER_H__
#define __INCLUDED_SPAWNER_H__

#include "Source/GameObject/GameObject.h"
#include "Source/GameObject/Character/Enemy/Enemy.h"

#include "TAKOEngine/Rendering/DebugRenderer/CylinderRenderer.h"

/**************************************************************************//**
	@class  Spawner
	@brief  敵を生成する
	@par    [説明]
*//***************************************************************************/
class Spawner : public GameObject
{
public:
	Spawner(uint8_t enemyType, int maxExistedEnemiesNum, int maxSpawnEnemiesNum);
	~Spawner() {}

	// 更新処理
	void Update(float elapsedTime) override;
	// 描画実行
	void Render(const RenderContext& rc) override;
	void RenderDX12(const RenderContextDX12& rc) override;

	// プレイヤー検索範囲更新
	void SetSearchRadius(const float& radius) { m_searchRadius = radius; }
	// 生成半径を更新
	void SetSpawnRadius(const float& radius) { m_spawnRadius = radius; }
	// 生成した敵が消滅時コールバック
	void EnemyDestoryCallBack(Enemy* enemy);

	void SetId(const uint8_t id) { m_id = id; }
	uint8_t GetId() { return m_id; };
public:
	// エネミー生成
	Enemy* Spawn();
private:
	// プレイヤー検索
	bool SearchPlayer();

private:
	uint8_t m_id = 0;
	uint8_t m_enemyType;					// 生成するエネミー種類

	float m_searchRadius = 5.0f;			// プレイヤー検索半径
	float m_spawnRadius = 3.0f;				// 生成半径

	unsigned int m_spawnedCountTotal = 0;	// 現在までに生成されたエネミーの数
	int m_maxExistedEnemiesNum;				// 最大同時存在数
	int m_maxSpawnedEnemiesNum;					// 最大生成数 (-1の時は無限)

	float m_spawnTimer = 0.0f;				// 生成タイマー
	float m_spawnTime = 2.0f;				// エネミー生成するまでの秒数

	std::set<Enemy*> m_pSpawnedEnemies;		// スポナーが管理するリスト
	std::unique_ptr<CylinderRenderer> m_cylinderRenderer;	// 円柱デバッグ表示用
};
#endif //! __INCLUDED_SPAWNER_H__





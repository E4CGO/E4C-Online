//! @file EnemyManager.cpp
//! @note 
#include "EnemyManager.h"
#include "Network/OnlineController.h"

/**************************************************************************//**
	@brief		エネミーの更新処理
	@param[in]	elapsedTime 経過時間
	@return		なし
*//***************************************************************************/
void EnemyManager::Update(float elapsedTime)
{
	std::lock_guard<std::mutex> lock(m_mut);
	ObjectManager<Enemy>::Update(elapsedTime);

	// エネミー同士の衝突処理
	size_t size = this->items.size();
	HitResult hit;
	for (size_t i = 0; i < size; i++)
	{
		Collider* col1 = items.at(i)->GetMoveCollider();
		if (!col1) continue;

		for (size_t j = i + 1; j < size; j++)
		{
			Collider* col2 = items.at(j)->GetMoveCollider();
			if (!col2) continue;

			if (col1->Collision(col2, {}, hit))
			{
				items.at(i)->AddImpulse({ hit.normal.x * hit.distance, 0, hit.normal.z * hit.distance });
				items.at(j)->AddImpulse({ -hit.normal.x * hit.distance, 0, -hit.normal.z * hit.distance });
			}
		}
	}

	// 一気同期
	if (m_syncEnemies.size() > 0)
	{
		std::vector<Enemy::SYNC_DATA> enemiesSyncData;
		for (const uint32_t& enemy_id : m_syncEnemies)
		{
			enemiesSyncData.push_back(GetEnemyById(enemy_id)->SyncData());
		}
		ONLINE_CONTROLLER->SyncEnemy(enemiesSyncData);

		m_syncEnemies.clear(); // 削除
	}
}
/**************************************************************************//**
	@brief		エネミーの描画処理
	@param[in]	rc レンダーコンテキスト
	@return		なし
*//***************************************************************************/
void EnemyManager::Render(const RenderContext& rc)
{
	std::lock_guard<std::mutex> lock(m_mut);
	ObjectManager<Enemy>::Render(rc);
}
/**************************************************************************//**
	@brief		エネミーの描画処理
	@param[in]	rc レンダーコンテキスト
	@return		なし
*//***************************************************************************/
void EnemyManager::RenderDX12(const RenderContextDX12& rc)
{
	std::lock_guard<std::mutex> lock(m_mut);
	ObjectManager<Enemy>::RenderDX12(rc);
}

/**************************************************************************//**
 	@brief		IDでエネミーを取得
	@param[in]	id エネミー
	@return		エネミー参照ポインタ
*//***************************************************************************/
Enemy* EnemyManager::GetEnemyById(const uint32_t& id)
{
	for (Enemy*& enemy : items)
	{
		if (enemy->GetEnemyId() == id) return enemy;
	}

	return nullptr;
}

/**************************************************************************//**
 	@brief		全ての敵にレイキャスト
	@param[in]	start	レイの開始座標
	@param[in]	end		レイの終了座標
	@param[in]	hit		当たり結果の参照
	@param[in]	camera	カメラフラグ
	@return		判断結果
*//***************************************************************************/
bool EnemyManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit, bool camera)
{
	std::lock_guard<std::mutex> lock(m_mut);
	HitResult temp;
	hit.distance = FLT_MAX;
	for (Enemy*& item : items)
	{
		for (std::pair<int, Collider*>collider : item->GetColliders())
		{
			if (collider.second->RayCast(start, end, temp) && (temp.distance < hit.distance))
			{
				hit = temp; // 最短
			}
		}
	}
	return hit.distance < FLT_MAX;
}

/**************************************************************************//**
 	@brief		同期エネミーID登録
	@param[in]	enemy_id エネミーID
*//***************************************************************************/
void EnemyManager::RegisterSync(const uint32_t& enemy_id)
{
	m_syncEnemies.insert(enemy_id);
}





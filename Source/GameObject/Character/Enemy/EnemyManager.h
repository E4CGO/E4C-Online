//! @file EnemyManager.h
//! @note 

#ifndef __INCLUDE_ENEMY_MANAGER_H__
#define __INCLUDE_ENEMY_MANAGER_H__


#include "TAKOEngine/Tool/Singleton.h"

#include "GameObject/ObjectManager.h"
#include "GameObject/Character/Enemy/Enemy.h"

class EnemyManager : public ObjectManager<Enemy>, public Singleton<EnemyManager>
{
	friend class Singleton<EnemyManager>;
protected:
	EnemyManager() = default;
	~EnemyManager() = default;
public:
	// オブジェクトとして更新処理
	void Update(float elapsedTime) override;
	// オブジェクトとして描画処理
	void Render(const RenderContext& rc) override;
	void RenderDX12(const RenderContextDX12& rc) override;

	Enemy* GetEnemyById(const uint32_t& id);

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit, bool camera = false);
	// 同期エネミーID登録
	void RegisterSync(const uint32_t& enemy_id);
private:
	std::mutex m_mut;

	//std::vector<Enemy::SYNC_DATA> m_enemySyncData; // 同期リストk
	std::set<uint32_t> m_syncEnemies;
};

#define ENEMIES EnemyManager::Instance()

#endif // !__INCLUDE_ENEMY_MANAGER_H__


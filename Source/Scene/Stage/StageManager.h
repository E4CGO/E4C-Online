//! @file StageManager.h
//! @note 

#ifndef __INCLUDE_STAGE_MANAGER_H__
#define __INCLUDE_STAGE_MANAGER_H__

#include "TAKOEngine/Tool\Singleton.h"
#include "GameObject/ObjectManager.h"
#include "GameObject/GameObject.h"
#include "Scene/Stage/Stage.h"

/**************************************************************************//**
	@class	StageManager
	@brief	ステージ(ルーム)管理用クラス
	@par	[説明]
			オープンフィールド、ダンジョン
*//***************************************************************************/
class StageManager : public ObjectManager<GameObject>, public Singleton<StageManager>
{
	friend class Singleton<StageManager>;
protected:
	StageManager() = default;
	~StageManager() = default;
public:
	// 更新処理
	void Update(float elapsedTime);
	// 描画処理
	void Render();
	void RenderDX12();
	// 全てのステージをクリア
	void Clear();
	// ステージ切り替え
	void ChangeStage(Stage* stage);

	// 現在ステージ参照ポインタを取得
	Stage* GetStage() { return this->m_pCurrentStage; }
private:
	Stage* m_pCurrentStage = nullptr;	// 現在ステージ
	Stage* m_pNextStage = nullptr;		// 次ステージ
};

#define STAGES StageManager::Instance()

#endif // !__INCLUDE_STAGE_MANAGER_H__
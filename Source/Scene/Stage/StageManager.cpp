//! @file StageManager.cpp
//! @note 
#include "StageManager.h"

// 更新処理
void StageManager::Update(float elapsedTime)
{
	if (m_pNextStage != nullptr)
	{
		// 古いシーンを終了処理
		Clear();
		// 新しいシーンを設定
		m_pCurrentStage = m_pNextStage;
		m_pNextStage = nullptr;

		// シーン初期化処理
		if (!m_pCurrentStage->IsReady())
		{
			m_pCurrentStage->Initialize();
			m_pCurrentStage->SetReady();
		}
	}
	if (m_pCurrentStage != nullptr)
	{
		m_pCurrentStage->Update(elapsedTime);
	}
}
// 描画処理
void StageManager::Render()
{
	if (m_pCurrentStage != nullptr)
	{
		m_pCurrentStage->Render();
	}
}
void StageManager::RenderDX12()
{
	if (m_pCurrentStage != nullptr)
	{
		m_pCurrentStage->Render();
	}
}
// シーンクリア
void StageManager::Clear()
{
	if (m_pCurrentStage != nullptr)
	{
		m_pCurrentStage->Finalize();
		delete m_pCurrentStage;
		m_pCurrentStage = nullptr;
	}
}
// シーン切り替え
void StageManager::ChangeStage(Stage* stage)
{
	// 新しいシーンを設定
	m_pNextStage = stage;
}
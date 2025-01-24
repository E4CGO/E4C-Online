//! @file SceneTitle_E4C.h
//! @note　シーンタイトル

#ifndef __INCLUDED_SCENE_TITLE_E4C_H__
#define __INCLUDED_SCENE_TITLE_E4C_H__

#include <unordered_set>

#include "TAKOEngine/Rendering/Model/ModelDX12.h"
#include "TAKOEngine/Rendering/SpriteDX12.h"
#include "TAKOEngine/Tool/GLTFImporter.h"
#include "TAKOEngine/Editor/Camera/FreeCameraController.h"
#include "TAKOEngine/AI/StateMachine.h"

#include "Scene/Scene.h"

/**************************************************************************//**
	@class	SceneTitle_E4C
	@brief	タイトル画面
	@par    [説明]	タイトル画面管理
*//***************************************************************************/
class SceneTitle_E4C : public Scene
{
public:
	SceneTitle_E4C() = default;
	~SceneTitle_E4C() = default;

	// 初期化
	void Initialize() override;
	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;
	// 描画処理
	void Render() override;

	void RenderDX12() override;

	StateMachine<SceneTitle_E4C>* GetStateMachine() { return stateMachine.get(); }
private:
	// シーンGUI描画
	void DrawSceneGUI();
public:
	enum STATE
	{
		INIT,
		START,
		OPTION,
		CREDITS,
		EXIT
	};
private:
	std::unique_ptr<StateMachine<SceneTitle_E4C>> stateMachine;

	// フレームバッファマネージャー
	FrameBufferManager* m_frameBuffer;

	// Sprite Preload
	std::unordered_set<const char*> spriteList = {
		"",											// マスク
		// Setting UI
		"Data/Sprites/UI/Title/background.png",
		"Data/Sprites/UI/Title/title_logo.png",
		"Data/Sprites/UI/Title/cont_d.png",
		"Data/Sprites/UI/Title/cont_g.png",
		"Data/Sprites/UI/Title/cont_h.png",
		"Data/Sprites/UI/Title/credits_d.png",
		"Data/Sprites/UI/Title/credits_h.png",
		"Data/Sprites/UI/Title/exit_d.png",
		"Data/Sprites/UI/Title/exit_h.png",
		"Data/Sprites/UI/Title/new_d.png",
		"Data/Sprites/UI/Title/new_h.png",
		"Data/Sprites/UI/Title/option_d.png",
		"Data/Sprites/UI/Title/option_h.png"
	};

	std::unordered_set<std::shared_ptr<Sprite>> spritePreLoad;
};

#endif // __INCLUDED_SCENE_TITLE_E4C_H__
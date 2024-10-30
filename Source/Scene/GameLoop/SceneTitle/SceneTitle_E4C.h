#pragma once

#include "TAKOEngine/Rendering/Model/ModelDX12.h"
#include "TAKOEngine/Rendering/SpriteDX12.h"
#include "TAKOEngine/Tool/GLTFImporter.h"
#include "TAKOEngine/Editor/Camera/FreeCameraController.h"
#include "TAKOEngine/AI/StateMachine.h"

#include <unordered_set>

// テスト用
#include "GameObject/ModelObject.h"
#include "Scene/Scene.h"

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
		EXIT
	};
private:
	std::unique_ptr<StateMachine<SceneTitle_E4C>> stateMachine;

	std::unique_ptr<myRenderer::shadow::ShadowMapRender> shadowMapRenderer = std::make_unique<myRenderer::shadow::ShadowMapRender>();

	std::unique_ptr<FreeCameraController> cameraController;


	// Sprite Preload
	std::unordered_set<const char*> spriteList = {
		"",											// マスク
		// Setting UI
		"Data/Sprites/UI/start.png",
		"Data/Sprites/UI/exit.png"
	};

	std::unordered_set<std::shared_ptr<Sprite>> spritePreLoad;
	std::unique_ptr<SpriteDX12>			m_sprites[8];

	static float time;
};

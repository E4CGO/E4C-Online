#pragma once

#include "TAKOEngine/Rendering/Model/ModelDX12.h"
#include "TAKOEngine/Rendering/SpriteDX12.h"
#include "TAKOEngine/Tool/GLTFImporter.h"
#include "TAKOEngine/Editor/Camera/FreeCameraController.h"
#include "TAKOEngine/AI/StateMachine.h"
#include "PlayerCharacterData.h"
#include "GameObject/Character/Player/NonPlayerCharacter.h"
#include "GameObject/Character/Player/PlayerCharacter.h"

#include <unordered_set>

// テスト用
#include "GameObject/ModelObject.h"
#include "Scene/Scene.h"

class SceneCharacter_E4C : public Scene
{
public:
	SceneCharacter_E4C() = default;
	~SceneCharacter_E4C() = default;

	// 初期化
	void Initialize() override;
	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;
	// 描画処理
	void Render() override;

	void RenderDX12() override;

	StateMachine<SceneCharacter_E4C>* GetStateMachine() { return stateMachine.get(); }

	void UpdateCurrentModel(int characterNumber, int modelType, int value);
private:
	// シーンGUI描画
	void DrawSceneGUI();
public:
	enum STATE
	{
		INIT,
		CHARACTERSELECTION,
		CHARACTERCREATIONLEFT,
		CHARACTERCREATIONCENTER,
		CHARACTERCREATIONRIGHT,
		START
	};

	std::unique_ptr<FreeCameraController> cameraController;

	std::vector<std::unique_ptr<NonPlayerCharacter>> m_previewCharacters;

private:
	std::unique_ptr<StateMachine<SceneCharacter_E4C>> stateMachine;

	std::unique_ptr<myRenderer::shadow::ShadowMapRender> shadowMapRenderer = std::make_unique<myRenderer::shadow::ShadowMapRender>();

	// フレームバッファマネージャー
	FrameBufferManager* m_frameBuffer;

	// Sprite Preload
	std::unordered_set<const char*> spriteList = {
		"",											// マスク
		// Setting UI
	};

	std::unordered_set<std::shared_ptr<Sprite>> spritePreLoad;
	std::unique_ptr<SpriteDX12>			m_sprites[8];

	static const int m_maxCharacters;

	static float m_time;

	float transitiontime = 0.f;
};

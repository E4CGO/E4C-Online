//! @file SceneCharacter_E4C.h
//! @note　シーンキャラクリ

#ifndef __SCENE_CHARACTER_E4C_H__
#define __SCENE_CHARACTER_E4C_H__

#include <unordered_set>

#include "TAKOEngine/Rendering/Model/ModelDX12.h"
#include "TAKOEngine/Rendering/SpriteDX12.h"
#include "TAKOEngine/Tool/GLTFImporter.h"
#include "TAKOEngine/Editor/Camera/FreeCameraController.h"
#include "TAKOEngine/AI/StateMachine.h"
#include "PlayerCharacterData.h"
#include "GameObject/Character/Player/NonPlayerCharacter.h"
#include "GameObject/Character/Player/PlayerCharacter.h"
#include "Scene/Scene.h"

/**************************************************************************//**
	@class	SceneCharacter_E4C
	@brief	キャラクリを管理するシーン
	@par    [説明]
*//***************************************************************************/
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
		CHARACTERCREATION,
		START
	};

	std::unique_ptr<FreeCameraController> cameraController;

	std::vector<std::unique_ptr<NonPlayerCharacter>> m_previewCharacters;

private:
	std::unique_ptr<StateMachine<SceneCharacter_E4C>> stateMachine;

	std::unique_ptr<myRenderer::shadow::ShadowMapRender> shadowMapRenderer = std::make_unique<myRenderer::shadow::ShadowMapRender>();

	// フレームバッファマネージャー
	FrameBufferManager* m_frameBuffer;

	// ポストエフェクト
	std::unique_ptr<PostprocessingRendererDX12>	postprocessingRenderer = std::make_unique<PostprocessingRendererDX12>();

	// Sprite Preload
	std::unordered_set<const char*> spriteList = {
		"",											// マスク
		"Data/Sprites/UI/start.png",
		"Data/Sprites/UI/back.png",
		"Data/Sprites/UI/keybinds/key_Left.png",
		"Data/Sprites/UI/keybinds/key_Right.png",
		"Data/Sprites/big_background.t.png"
		// Setting UI
	};
	std::unordered_set<std::shared_ptr<Sprite>> spritePreLoad;

	// Model Preload
	std::unordered_set<const char*> modelList = {
		"Data/Model/Character/PlayerModels/MDL_PLAYER_BODY_ANIMATION.glb",
		"Data/Model/Character/PlayerModels/MDL_PLAYER_BOTTOM_ANIMATION.glb",
		"Data/Model/Character/PlayerModels/MDL_PLAYER_TOP_ANIMATION.glb",
		"Data/Model/Character/PlayerModels/MDL_PLAYER_HAIR_ANIMATION.glb",
		"Data/Model/Character/PlayerModels/MDL_PLAYER_SWORD_ANIMATION.glb",
		"Data/Model/Character/PlayerModels/MDL_PLAYER_SHIELD1_ANIMATION.glb",
		"Data/Model/Character/PlayerModels/MDL_PLAYER_SHIELD2_ANIMATION.glb",
	};
	std::unordered_set<std::shared_ptr<ModelResource>> modelPreLoad;

	static const int m_maxCharacters;

	static float m_time;

	float transitiontime = 0.f;
};

#endif // __SCENE_CHARACTER_E4C_H__
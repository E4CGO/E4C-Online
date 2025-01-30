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
public:
	enum STATE
	{
		INIT,
		CHARACTER_SELECTION,
		CHARACTER_CREATION,
		START
	};

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

	int GetSelectedCharacterIdx() { return m_selectedCharacter; }
	PlayerCharacter* GetSelectedCharacter() { return m_previewCharacters[m_selectedCharacter]; }
	void SetSelectedCharacter(PlayerCharacter* character)
	{
		for (int i = 0; i < m_previewCharacters.size(); i++)
		{
			if (m_previewCharacters[i] == character) {
				SetSelectedCharacter(i);
				return;
			}
		}
	}
	void SetSelectedCharacter(int idx)
	{
		m_selectedCharacter = idx;
	}

	PlayerCharacter* RegisterCharacter(PlayerCharacter* chara)
	{
		m_previewCharacters.push_back(chara);
		return chara;
	}
	const std::vector<PlayerCharacter*>& GetCharacters() { return m_previewCharacters; }
private:
	// シーンGUI描画
	void DrawSceneGUI();
private:
	std::unique_ptr<StateMachine<SceneCharacter_E4C>> stateMachine;

	std::unique_ptr<myRenderer::shadow::ShadowMapRender> shadowMapRenderer = std::make_unique<myRenderer::shadow::ShadowMapRender>();

	std::unique_ptr<FreeCameraController> cameraController;

	std::unique_ptr<ModelObject> m_background;

	std::vector<PlayerCharacter*> m_previewCharacters;
	int m_selectedCharacter = 0; // 選択したキャラ

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
		//男性よう
		"Data/Model/Character/PlayerModels/PlayerMale/MDLANM_PLAYER_BODY.glb",

		"Data/Model/Character/PlayerModels/PlayerMale/Hair/MDLANM_PLAYER_HAIR.glb",
		"Data/Model/Character/PlayerModels/PlayerMale/Hair/MDLANM_PLAYER_HAIR2.glb",
		"Data/Model/Character/PlayerModels/PlayerMale/Hair/MDLANM_PLAYER_HAIR3.glb",

		"Data/Model/Character/PlayerModels/PlayerMale/Costume1/MDLANM_PLAYER_TOP.glb",
		"Data/Model/Character/PlayerModels/PlayerMale/Costume2/MDLANM_PLAYER_TOP.glb",
		"Data/Model/Character/PlayerModels/PlayerMale/Costume3/MDLANM_PLAYER_TOP.glb",

		"Data/Model/Character/PlayerModels/PlayerMale/Costume1/MDLANM_PLAYER_ARM.glb",
		"Data/Model/Character/PlayerModels/PlayerMale/Costume2/MDLANM_PLAYER_ARM.glb",
		"Data/Model/Character/PlayerModels/PlayerMale/Costume3/MDLANM_PLAYER_ARM.glb",

		"Data/Model/Character/PlayerModels/PlayerMale/Costume1/MDLANM_PLAYER_BOTTOM.glb",
		"Data/Model/Character/PlayerModels/PlayerMale/Costume2/MDLANM_PLAYER_BOTTOM.glb",
		"Data/Model/Character/PlayerModels/PlayerMale/Costume3/MDLANM_PLAYER_BOTTOM.glb",

		"Data/Model/Character/PlayerModels/PlayerMale/Costume1/MDLANM_PLAYER_SHOES.glb",
		"Data/Model/Character/PlayerModels/PlayerMale/Costume2/MDLANM_PLAYER_SHOES.glb",
		"Data/Model/Character/PlayerModels/PlayerMale/Costume3/MDLANM_PLAYER_SHOES.glb",

		"Data/Model/Character/PlayerModels/PlayerMale/Weapons/MDLANM_PLAYER_SWORD.glb",
		"Data/Model/Character/PlayerModels/PlayerMale/Weapons/MDLANM_PLAYER_SWORD2.glb",
		"Data/Model/Character/PlayerModels/PlayerMale/Weapons/MDLANM_PLAYER_SWORD3.glb",
		"Data/Model/Character/PlayerModels/PlayerMale/Weapons/MDLANM_PLAYER_ROD.glb",
		"Data/Model/Character/PlayerModels/PlayerMale/Weapons/MDLANM_PLAYER_ROD2.glb",

		//女性よう
		"Data/Model/Character/PlayerModels/PlayerFemale/MDLANM_PLAYER_F_BODY.glb",

		"Data/Model/Character/PlayerModels/PlayerFemale/Hair/MDLANM_PLAYER_F_HAIR.glb",
		"Data/Model/Character/PlayerModels/PlayerFemale/Hair/MDLANM_PLAYER_F_HAIR2.glb",
		"Data/Model/Character/PlayerModels/PlayerFemale/Hair/MDLANM_PLAYER_F_HAIR3.glb",
		"Data/Model/Character/PlayerModels/PlayerFemale/Hair/MDLANM_PLAYER_F_HAIR4.glb",

		"Data/Model/Character/PlayerModels/PlayerFemale/Costume1/MDLANM_PLAYER_F_TOP.glb",
		"Data/Model/Character/PlayerModels/PlayerFemale/Costume2/MDLANM_PLAYER_F_TOP.glb",
		"Data/Model/Character/PlayerModels/PlayerFemale/Costume3/MDLANM_PLAYER_F_TOP.glb",

		"Data/Model/Character/PlayerModels/PlayerFemale/MDLANM_PLAYER_F_ARM.glb",

		"Data/Model/Character/PlayerModels/PlayerFemale/Costume1/MDLANM_PLAYER_F_BOTTOM.glb",
		"Data/Model/Character/PlayerModels/PlayerFemale/Costume2/MDLANM_PLAYER_F_BOTTOM.glb",
		"Data/Model/Character/PlayerModels/PlayerFemale/Costume3/MDLANM_PLAYER_F_BOTTOM.glb",

		"Data/Model/Character/PlayerModels/PlayerFemale/Costume1/MDLANM_PLAYER_F_SHOES.glb",
		"Data/Model/Character/PlayerModels/PlayerFemale/Costume2/MDLANM_PLAYER_F_SHOES.glb",

		"Data/Model/Character/PlayerModels/PlayerFemale/Weapons/MDLANM_PLAYER_F_SWORD.glb",
		"Data/Model/Character/PlayerModels/PlayerFemale/Weapons/MDLANM_PLAYER_F_SWORD2.glb",
		"Data/Model/Character/PlayerModels/PlayerFemale/Weapons/MDLANM_PLAYER_F_SWORD3.glb",
		"Data/Model/Character/PlayerModels/PlayerFemale/Weapons/MDLANM_PLAYER_F_ROD.glb",
		"Data/Model/Character/PlayerModels/PlayerFemale/Weapons/MDLANM_PLAYER_F_ROD2.glb",
	};
	std::unordered_set<std::shared_ptr<ModelResource>> modelPreLoad;

	static float m_time;

	float transitiontime = 0.f;
};

#endif // __SCENE_CHARACTER_E4C_H__
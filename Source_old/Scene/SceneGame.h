#pragma once

#include "TAKOEngine/Network/NetworkController.h"
#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"
#include "TAKOEngine/Editor/Camera/Camera.h"

#include <string>
#include <unordered_set>

#include "UI/Widget/WidgetMenu.h"

#include "Scene/Scene.h"
#include "Scene/Stage/Stage.h"
#include "TAKOEngine/Rendering/DeferredRendering.h"

#define START_TIME 3.0f

class SceneGame : public Scene
{
public:
	SceneGame(const char* name, const char* host, const char* port, NetworkController* networkController = nullptr);
	~SceneGame() override {};

	// 初期化
	void Initialize() override;
	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;
	// 描画処理
	void Render() override;

	void UpdateConnection();
	bool UpdateGame(float elapsedTime);

	//カメラの移動値や回転角を保存する
	void SaveSettings();
	//保存したカメラの移動値や回転角を出力する
	void LoadSettings();
	//カメラのパラメータを触れるようにする
	void CameraGUI();

	StateMachine<SceneGame>* GetStateMachine() { return stateMachine.get(); }
	NetworkController* GetNetworkController() { return networkController; }

private:
	char address[256];
	std::string name;
	std::string host;
	std::string port;

	float startTimer = START_TIME;

	NetworkController* networkController = nullptr;

	
	std::unique_ptr<ThridPersonCameraController> cameraController;

	std::unique_ptr<PostprocessingRenderer>	postprocessingRenderer = std::make_unique<PostprocessingRenderer>();

	std::unique_ptr<myRenderer::shadow::ShadowMapRender> shadowMapRenderer = std::make_unique<myRenderer::shadow::ShadowMapRender>();

	std::unique_ptr<Stage> stage;

	std::unique_ptr<StateMachine<SceneGame>> stateMachine;

	//DeferredRendering
	std::unique_ptr<DeferredRendering> deferredRendering = std::make_unique<DeferredRendering>();

	WidgetMenu* menu;

	// Sprite Preload
	std::unordered_set<const char*> spriteList = {
		"",											// マスク
		"Data/Sprites/crosshair122.png",
		"Data/Sprites/skill_icon.png",
		"Data/Sprites/big_background.png",
		"Data/Sprites/Gauge/Hp_line_white.png",
		"Data/Sprites/Gauge/Hp_frame.png",

		// Setting UI
		"Data/Sprites/bar_ready.png",
		"Data/Sprites/button_frame.png",
		"Data/Sprites/button_ready_on.png",
		"Data/Sprites/button2_ready_on.png",
		"Data/Sprites/button_agree.png",
		"Data/Sprites/button3_ready.png",
		"Data/Sprites/button_cancel.png",
		"Data/Sprites/gear.png",
	};
	std::unordered_set<std::shared_ptr<Sprite>> spritePreLoad;
	// Model Preload
	std::unordered_set<const char*> modelList = {
		// Chara
		"Data/Model/Character/Barbarian.glb",
		"Data/Model/Character/Knight.glb",
		//"Data/Model/Character/Mage.glb",
		"Data/Model/Character/Rogue_Hooded.glb",
		// Object
		"Data/Model/Object/arrow.gltf",
		"Data/Model/Object/axe_2handed.gltf",
		"Data/Model/Object/smokebomb.gltf",
		//"Data/Model/Object/spellbook_open.gltf",
		// Enemy
		//"Data/Model/Enemy/character_skeleton_archer.gltf",
		//"Data/Model/Enemy/character_skeleton_mage.gltf",
		"Data/Model/Enemy/character_skeleton_minion.gltf",
		//"Data/Model/Enemy/character_skeleton_warrior.gltf",
	};
	std::unordered_set<std::shared_ptr<ModelResource>> modelPreLoad;

	enum CameraMovePar
	{
		MoveEyeX,
		MoveEyeY,
		MoveEyeZ,
		MoveDuration,
		MoveMax
	};
	enum CameraRotatePar
	{
		Angle,
		Radius,
		Speed,
		RotateMax
	};
	float settingValue;
	float MovePar[CameraMovePar::MoveMax] = { settingValue,settingValue,settingValue,settingValue };
	float RotatePar[CameraRotatePar::RotateMax] = { settingValue,settingValue,settingValue };

	float transitionTime = 0.0f;
	float transitionDuration = 2.f;  // 5秒かけて移動
	int currentSegment = 0;
	std::vector<DirectX::XMFLOAT3> cameraPositions = {
	{0,0,0},
	{5.0f, -1.0f, 2.0f},
	{2.0f, 3.0f, 4.0f},
	{3.f,2.f,2.f},
	{0.f,0.f,1.f}
	};
	Camera* mainCamera;
};

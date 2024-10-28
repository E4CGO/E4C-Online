#pragma once
#include "Scene/Scene.h"
#include "GameObject/ModelObject.h"
#include "GameObject/Character/Player/Knight.h"
#include "GameObject/Character/Player/Barbarian.h"
#include "Map/MapTile.h"
#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"

class SceneTest : public Scene
{
public:
	SceneTest() = default;
	~SceneTest() = default;

	// 初期化
	void Initialize() override;
	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;
	// 描画処理
	void Render() override;
private:
	Camera* maincamera;
	//std::unique_ptr<FreeCameraController> cameraController;
	std::unique_ptr<ThridPersonCameraController> cameraController;

	MapTile* stage;
	//std::unique_ptr<ModelObject> knight;

	std::unique_ptr<Player> player;
};

class EmptyScene : public Scene
{
public:
	EmptyScene() {}
	~EmptyScene() {}

	// 初期化
	void Initialize() override {
		player = new Barbarian;
	};
	// 終了化
	void Finalize() override {
		delete player;
	};

	// 更新処理
	void Update(float elapsedTime) override {
		player->Update(elapsedTime);
	};
	// 描画処理
	void Render() override {};
private:
	Player* player;
};
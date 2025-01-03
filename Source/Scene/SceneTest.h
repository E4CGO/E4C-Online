#pragma once

#include "Scene/Scene.h"
#include "GameObject/Character/Player/PlayerCharacter.h"
#include "Map/MapTile.h"
#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"
#include "TAKOEngine/Editor/Camera/CameraManager.h"

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
	
	std::unique_ptr<ThridPersonCameraController> cameraController;

	MapTile* stage;

	std::unique_ptr<PlayerCharacter> newPlayer;
};

class EmptyScene : public Scene
{
public:
	EmptyScene() {}
	~EmptyScene() {}

	// 初期化
	void Initialize() override {
	};
	// 終了化
	void Finalize() override {
	};

	// 更新処理
	void Update(float elapsedTime) override {
	};
	// 描画処理
	void Render() override {};
private:
};
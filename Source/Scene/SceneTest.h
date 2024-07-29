#pragma once
#include "Scene/Scene.h"
#include "GameObject/ModelObject.h"
#include "GameObject/Character/Player/Knight.h"
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
	Camera& camera = Camera::Instance();
	//std::unique_ptr<FreeCameraController> cameraController;
	std::unique_ptr<ThridPersonCameraController> cameraController;

	std::unique_ptr<MapTile> stage;
	//std::unique_ptr<ModelObject> knight;

	std::unique_ptr<Player> player;
};
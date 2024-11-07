#pragma once

#include <memory>
#include <array>

#include "GameObject/ModelObject.h"
#include "GameObject/GameObject.h"
#include "GameObject/Character/Player/PlayerCharacter.h"
#include "GameObject/Props/Teleporter.h"
#include "TAKOEngine/Rendering/Shaders/PlaneShader.h"

#include "Scene/Scene.h"
#include "Scene/Stage/Stage.h"

#include "Map/MapTile.h"

#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"
#include "TAKOEngine/Editor/Camera/CameraManager.h"
#include "TAKOEngine/Tool/GLTFImporter.h"

class SceneGame_E4C;

class StageOpenWorld_E4C : public Stage
{
public:
	StageOpenWorld_E4C(SceneGame_E4C* scene) : m_pScene(scene), Stage() {};

	void Initialize() override;

	void Update(float elapsedTime) override;

	void Render() override;
protected:
	void OnPhase() override;
public:
	enum PHASE
	{
		NORMAL,
	};
private:
	SceneGame_E4C* m_pScene;

	std::unique_ptr<ThridPersonCameraController> cameraController;

	MapTile* stage_collision = nullptr;

	std::unique_ptr<ModelObject> testModel;

	std::unique_ptr <Teleporter> teleporter;
	std::unique_ptr <Plane> plane;
	std::unique_ptr <Plane> portal;

	std::unique_ptr<gltf_model> map;

	std::unique_ptr<ModelObject> Locator;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffers[8];

	float transitionTime = 0.0f;
	float transitionDuration = 2.f;  // 5秒かけて移動
	int currentSegment = 0;
};

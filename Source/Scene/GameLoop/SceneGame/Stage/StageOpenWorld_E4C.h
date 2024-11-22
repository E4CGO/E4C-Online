#pragma once

#include <memory>
#include <array>

#include "GameObject/ModelObject.h"
#include "GameObject/GameObject.h"
#include "GameObject/Character/Player/PlayerCharacter.h"
#include "GameObject/Props/Teleporter.h"
#include "GameObject/Props/Spawner.h"
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
	StageOpenWorld_E4C(SceneGame_E4C* scene) : m_scene(scene), Stage() {};

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
	SceneGame_E4C* m_scene;

	std::unique_ptr<ThridPersonCameraController> cameraController;
	MapTile* stage_collision = nullptr;

	GameObject* spawners;

	std::unique_ptr <Teleporter> teleporter;
	std::unique_ptr <Plane> plane;
	std::unique_ptr <Plane> portal;

	std::unique_ptr<gltf_model> map;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffers[8];


	float transitionTime = 0.0f;
	float transitionDuration = 2.f;  // 5•b‚©‚¯‚ÄˆÚ“®
	int currentSegment = 0;
	
	std::vector<DirectX::XMFLOAT3> cameraPositions = {
		{0,3,0},
		{10,3,0},
		{5,3,4},
		{8,3,8}
	};

	bool debugCameraMode = false;
};

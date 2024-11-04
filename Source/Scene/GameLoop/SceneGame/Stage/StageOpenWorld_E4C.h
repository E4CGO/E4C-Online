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

class StageOpenWorld_E4C : public Stage
{
public:
	StageOpenWorld_E4C() = default;

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
	std::unique_ptr<FreeCameraController> DebugcameraController;

	std::unique_ptr<ThridPersonCameraController> cameraController;

	MapTile* stage_collision;

	std::unique_ptr<PlayerCharacter> player;

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


	bool DebugCameraMode = false;
};

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

	Camera& camera = Camera::Instance();
	std::unique_ptr<ThridPersonCameraController> cameraController;

	MapTile* stage_collision;

	std::unique_ptr<PlayerCharacter> player;

	std::unique_ptr <Teleporter> teleporter;
	std::unique_ptr <Plane> plane;
	std::unique_ptr <Plane> portal;

	std::unique_ptr<gltf_model> map;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffers[8];
};

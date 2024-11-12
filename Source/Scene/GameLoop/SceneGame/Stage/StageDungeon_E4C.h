#pragma once

#include <memory>
#include <array>

#include "GameObject/ModelObject.h"
#include "GameObject/GameObject.h"
#include "GameObject/Character/Player/PlayerCharacter.h"
#include "TAKOEngine/Rendering/Shaders/PlaneShader.h"

#include "Scene/Scene.h"
#include "Scene/Stage/Stage.h"

#include "Map/MapTile.h"
#include "Map/RoomBase.h"
#include "Map/SimpleRoom1.h"
#include "Map/EndRoom1.h"
#include "Map/CrossRoom1.h"
#include "Map/Passage1.h"

#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"
#include "TAKOEngine/Editor/Camera/CameraManager.h"
#include "TAKOEngine/Tool/GLTFImporter.h"

class SceneGame_E4C;

class StageDungeon_E4C : public Stage
{
public:
	// コンストラクタ（部屋配列なし）
	StageDungeon_E4C(SceneGame_E4C* scene);
	// コンストラクタ（部屋配列あり）
	StageDungeon_E4C(SceneGame_E4C* scene, std::vector<int> roomTree);

	
	enum PHASE
	{
		NORMAL,
	};

	enum STATE
	{

	};

	void Initialize() override;

	void Update(float elapsedTime) override;

	void Render() override;
protected:
	void OnPhase() override;
protected:
	SceneGame_E4C* m_pScene;

	std::unique_ptr<ThridPersonCameraController> cameraController;

	std::unique_ptr<RoomBase> rootRoom;

	std::unique_ptr<ModelObject> testModel;

	std::unique_ptr <Plane> plane;
	std::unique_ptr <Plane> portal;

	std::unique_ptr<ModelObject> Locator;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffers[8];

	float transitionTime = 0.0f;
	float transitionDuration = 2.f;  // 5秒かけて移動
	int currentSegment = 0;

	
};
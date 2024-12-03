#pragma once

#include <unordered_set>
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

class SceneGame_E4C;

class StageOpenWorld_E4C : public Stage
{
public:
	StageOpenWorld_E4C(SceneGame_E4C* scene) : m_pScene(scene), Stage() {};
	~StageOpenWorld_E4C() {};

	void Initialize() override;

	void Update(float elapsedTime) override;

	void Render() override;

	void RenderDX12() override;
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

	std::unique_ptr <Teleporter> teleporter;

	std::unordered_map<std::string, std::unique_ptr<ModelObject>> models;

	std::unique_ptr<ModelObject> sky;
	DirectX::XMFLOAT4X4 test_transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	DirectX::XMFLOAT3 test_position = { 0, 0, 0 };
	DirectX::XMFLOAT4 test_rotation = { 0, 0, 0, 0 };
	DirectX::XMFLOAT3 test_scale = { 1, 1, 1 };

	std::unique_ptr<SpriteDX12>			m_sprites[8];

	// Sprite Preload
	std::unordered_set<const char*> spriteList = {
		"",											// マスク
		// Setting UI
	};

	std::unordered_set<std::shared_ptr<Sprite>> spritePreLoad;

	float transitionTime = 0.0f;
	float transitionDuration = 2.f;  // 5秒かけて移動
	int currentSegment = 0;

	std::vector<DirectX::XMFLOAT3> cameraPositions = {
		{0,3,0},
		{10,3,0},
		{5,3,4},
		{8,3,8}
	};

	// フレームバッファマネージャー
	FrameBufferManager* m_frameBuffer = nullptr;

	// ポストエフェクト
	std::unique_ptr<PostprocessingRendererDX12>	postprocessingRenderer = std::make_unique<PostprocessingRendererDX12>();
};

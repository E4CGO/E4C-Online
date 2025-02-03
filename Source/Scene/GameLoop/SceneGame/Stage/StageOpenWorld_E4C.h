//! @file StageOpenWorld_E4C.h
//! @note

#ifndef __INCLUDED_STAGE_OPEN_WORLD_E4C_H__
#define __INCLUDED_STAGE_OPEN_WORLD_E4C_H__

#include <unordered_set>
#include <memory>
#include <array>
#include <deque>

#include "GameObject/ModelObject.h"
#include "GameObject/GameObject.h"
#include "GameObject/Character/Player/PlayerCharacter.h"
#include "TAKOEngine/Rendering/Plane.h"
#include "GameObject/Props/Teleporter.h"
#include "GameObject/Props/Spawner.h"
#include "GameObject/Character/Enemy/MouseMob.h"
#include "Source/UI/Widget/WidgetPlayerHP.h"
#include "Source/UI/Widget/WidgetPauseMenu.h"

#include "TAKOEngine/Rendering/Shaders/PlaneShader.h"

#include "Scene/Scene.h"
#include "Scene/Stage/Stage.h"

#include "Map/MapTile.h"

#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"
#include "TAKOEngine/Editor/Camera/CameraManager.h"
#include "Source/GameObject/Props/Spawner.h"

#include "TAKOEngine/Rendering/ParticleRenderer/HitParticleRenderer.h"

using namespace DirectX;

class SceneGame_E4C;

class StageOpenWorld_E4C : public Stage
{
public:
	StageOpenWorld_E4C(SceneGame_E4C* scene) : m_pScene(scene), Stage() {};
	~StageOpenWorld_E4C() {};

	void Initialize() override;

	void Finalize() override;

	void Update(float elapsedTime) override;

	void Render() override;

	void RenderDX12() override;
public:
	enum PHASE
	{
		NORMAL,
	};

private:
	// シーンGUI描画
	void DrawSceneGUI();

private:

	SceneGame_E4C* m_pScene;

	std::unique_ptr<ThridPersonCameraController> cameraController;

	MapTile* stage_collision = nullptr;
	MapTile* village_collision = nullptr;
	MapTile* border_collision = nullptr;

	std::unique_ptr <Teleporter> teleporter;

	std::unordered_map<std::string, std::unique_ptr<ModelObject>> models;
	std::unordered_map<std::string, std::unique_ptr<ModelObject>> modelsInit;

	std::unique_ptr<ModelObject> sky;

	std::unique_ptr<SpriteDX12> m_sprites[8];

	std::unique_ptr<RunningDustDX12> runningDust1;
	std::deque<RunningDustDX12> runningDust;
	bool running = true;
	float f_timer;
	int f_count = 0;// 何個目のモデルか数える
	float f_INTERVAL = 0.1f;// push の間隔
	float f_popINTERVAL = 0.3f;// pop の間隔
	float f_alpha = 0.2f;// 透明度

	// Sprite Preload
	std::unordered_set<const char*> spriteList = {
		"",											// マスク
		// Setting UI
	};

	std::unordered_set<std::shared_ptr<Sprite>> spritePreLoad;

	WidgetPlayerHP* m_pCharacterGauge;
	WidgetPauseMenu* m_pPauseMenu;

	float transitionTime = 0.0f;
	float transitionDuration = 2.f;  // 5秒かけて移動
	int currentSegment = 0;

	std::vector<DirectX::XMFLOAT3> cameraPositions = {
		{0,3,0},
		{10,3,0},
		{5,3,4},
		{8,3,8}
	};

	std::unique_ptr<HitParticleRenderer> m_particle[2];

	// フレームバッファマネージャー
	FrameBufferManager* m_frameBuffer = nullptr;

	// ポストエフェクト
	std::unique_ptr<PostprocessingRendererDX12>	postprocessingRenderer = std::make_unique<PostprocessingRendererDX12>();

	float m_sceneGlobalTimer = 0;
	float m_sceneTickTimer = 0;
	float shakeTimer = 0;
	float shakeTime = 0.2f;
};

#endif // !__INCLUDED_STAGE_OPEN_WORLD_E4C_H__
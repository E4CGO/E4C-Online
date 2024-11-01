#include <profiler.h>
#include "Map/MapTileManager.h"
#include "TAKOEngine/Tool/GLTFImporter.h"


#include "Source/Scene/Stage/StageManager.h"

#include "SceneGame_E4C.h"
#include "SceneGame_E4CState.h"
#include "Scene/GameLoop/SceneGame/Stage/StageOpenWorld_E4C.h"
#include "Scene/Stage/TestingStage.h"

void SceneGame_E4C::Initialize()
{
	stateMachine = std::make_unique<StateMachine<SceneGame_E4C>>();
	stateMachine->RegisterState(GAME_STATE::OPENWORLD, new SceneGame_E4CState::WaitingState(this));
	stateMachine->RegisterState(GAME_STATE::DUNGEON, new SceneGame_E4CState::WaitingState(this));
	stateMachine->SetState(GAME_STATE::OPENWORLD);

	//stageDungeon = std::make_unique<TestingStage>();
	//stageDungeon->Initialize();

	CameraManager& cameraManager = CameraManager::Instance();
	Camera* mainCamera = new Camera();
	cameraManager.Register(mainCamera);
	cameraManager.SetCamera(0);

	stageOpenWorld = std::make_unique<StageOpenWorld_E4C>();
	stageOpenWorld->Initialize();

	
}

void SceneGame_E4C::Finalize()
{
	LightManager::Instance().Clear();
	CameraManager::Instance().Clear();
}

// 更新処理
void SceneGame_E4C::Update(float elapsedTime)
{
	stateMachine->Update(elapsedTime);

	stageNumber = STAGES.stageNumber;

	if (stageNumber == 0)
	{
		stageOpenWorld->Update(elapsedTime);
	}
	if (stageNumber == 1)
	{
		stageDungeon->Update(elapsedTime);
	}
}

// 描画処理
void SceneGame_E4C::Render()
{
	RenderContext rc;
	rc.deviceContext = T_GRAPHICS.GetDeviceContext();
	rc.renderState = T_GRAPHICS.GetRenderState();


	if (stageNumber == 0)
	{
		stageOpenWorld->Render();
	}
	if (stageNumber == 1)
	{
		STAGES.Render(rc);
	}

	//ProfileDrawUI();
}
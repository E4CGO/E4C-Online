//! @file SceneGame_E4C.cpp
//! @note 

#include "SceneGame_E4C.h"

#include <iostream>
#include <profiler.h>
#include "Map/MapTileManager.h"
#include "TAKOEngine/Tool/GLTFImporter.h"


#include "Source/Scene/Stage/StageManager.h"

#include "SceneGame_E4CState.h"
#include "Scene/GameLoop/SceneGame/Stage/StageOpenWorld_E4C.h"
#include "Scene/Stage/TestingStage.h"

#include "Scene/Stage/StageManager.h"
#include "GameObject/Character/Player/PlayerCharacterManager.h"
#include "TAKOEngine/Tool/Console.h"

#include "UI/Widget/WidgetCrosshair.h"
#include "TAKOEngine/GUI/UIManager.h"

void SceneGame_E4C::Initialize()
{
	stateMachine = std::make_unique<StateMachine<SceneGame_E4C>>();
	stateMachine->RegisterState(GAME_STATE::OPENWORLD, new SceneGame_E4CState::WaitingState(this));
	stateMachine->RegisterState(GAME_STATE::DUNGEON, new SceneGame_E4CState::WaitingState(this));
	stateMachine->SetState(GAME_STATE::OPENWORLD);

	CameraManager& cameraManager = CameraManager::Instance();
	Camera* mainCamera = new Camera();
	cameraManager.Register(mainCamera);
	cameraManager.SetCamera(0);

	//Console::Instance().Open();

	// 選択した自機
	const PlayerCharacterData::CharacterInfo info = PlayerCharacterData::Instance().GetCurrentCharacter();
	PlayerCharacter* player = PlayerCharacterManager::Instance().UpdatePlayerData(0, "", info.Character.pattern);
	player->Show();
	player->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::IDLE));

	STAGES.ChangeStage(new StageOpenWorld_E4C(this));

	m_ponlineController = new Online::OnlineController;
	if (m_ponlineController->Initialize())
	{
		m_ponlineController->Login();
	}

	CURSOR_OFF;

	UI.Register(new WidgetCrosshair);
}

void SceneGame_E4C::Finalize()
{
	delete m_ponlineController;
	Console::Instance().Close();
	LightManager::Instance().Clear();
	CameraManager::Instance().Clear();
	STAGES.Clear();
	MAPTILES.Clear();
	PlayerCharacterManager::Instance().Clear();
	UI.Clear();
}

// 更新処理
void SceneGame_E4C::Update(float elapsedTime)
{
	STAGES.Update(elapsedTime);

	stateMachine->Update(elapsedTime);
}

// 描画処理
void SceneGame_E4C::Render()
{
	RenderContext rc;
	rc.deviceContext = T_GRAPHICS.GetDeviceContext();
	rc.renderState = T_GRAPHICS.GetRenderState();

	STAGES.Render();

	//ProfileDrawUI();
}
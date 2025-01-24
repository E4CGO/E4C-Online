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
#include "Scene/GameLoop/SceneGame/Stage/StageDungeon_E4C.h"

#include "GameObject/Character/Player/PlayerCharacterManager.h"
#include "GameObject/Character/Enemy/EnemyManager.h"
#include "TAKOEngine/Tool/Console.h"

#include "UI/Widget/WidgetCrosshair.h"
#include "TAKOEngine/GUI/UIManager.h"
#include "Source\PlayerCharacterData.h"
#include "GameObject/Props/SpawnerManager.h"

#include "TAKOEngine/Physics/CollisionManager.h"
#include "UI/Widget/WidgetCharacterName.h"

void SceneGame_E4C::Initialize()
{
	stateMachine = std::make_unique<StateMachine<SceneGame_E4C>>();
	stateMachine->RegisterState(GAME_STATE::INIT, new SceneGame_E4CState::InitState(this));
	stateMachine->RegisterState(GAME_STATE::EXIT, new SceneGame_E4CState::ExitState(this));
	stateMachine->SetState(GAME_STATE::INIT);

	CameraManager& cameraManager = CameraManager::Instance();
	Camera* mainCamera = new Camera();
	cameraManager.Register(mainCamera);
	cameraManager.SetCamera(0);

	//Console::Instance().Open();

	// 選択した自機
	const PlayerCharacterData::CharacterInfo info = PlayerCharacterData::Instance().GetCurrentCharacter();
	PlayerCharacter* player = PlayerCharacterManager::Instance().UpdatePlayerData(0, info.name.c_str(), info.pattern);
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
	UI.Register(new WidgetCharacterName);

	// STAGE初期化
	STAGES.Update(0);
}

void SceneGame_E4C::Finalize()
{
	delete m_ponlineController;
	Console::Instance().Close();
	LightManager::Instance().Clear();
	CameraManager::Instance().Clear();
	STAGES.Clear();
	PlayerCharacterManager::Instance().Clear();
	ENEMIES.Clear();
	MAPTILES.Clear();
	PlayerCharacterManager::Instance().Clear();
	EnemyManager::Instance().Clear();
	SpawnerManager::Instance().Clear();
	COLLISIONS.Clear();
	UI.Clear();
}

// 更新処理
void SceneGame_E4C::Update(float elapsedTime)
{
	STAGES.Update(elapsedTime);
	UI.Update(elapsedTime);
	COLLISIONS.Contacts();
	stateMachine->Update(elapsedTime);
}

// 描画処理
void SceneGame_E4C::Render()
{
	T_TEXT.Begin();
	RenderContext rc;
	rc.deviceContext = T_GRAPHICS.GetDeviceContext();
	rc.renderState = T_GRAPHICS.GetRenderState();

	STAGES.Render();

	UI.Render(rc);

	T_TEXT.End();
	// デバッグレンダラ描画実行
	T_GRAPHICS.GetDebugRenderer()->Render(T_GRAPHICS.GetDeviceContext(), CameraManager::Instance().GetCamera()->GetView(), CameraManager::Instance().GetCamera()->GetProjection());
}

void SceneGame_E4C::RenderDX12()
{
	STAGES.RenderDX12();
}
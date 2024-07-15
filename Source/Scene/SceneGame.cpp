#include "SceneGame.h"

#include <locale.h>
#include <profiler.h>
#include <algorithm>

#include "TAKOEngine/Runtime/tentacle_lib.h"

#include "TAKOEngine/Network/Network.h"
#include "TAKOEngine/GUI/UIManager.h"
#include "TAKOEngine/GUI/Dialog.h"
#include "TAKOEngine/Physics/Collision.h"
#include "TAKOEngine/Physics/CollisionDataManager.h"
#include "TAKOEngine/Effects/EffectManager.h"
#include "TAKOEngine/Tool/Encode.h"

#include "Scene/SceneManager.h"
#include "Scene/SceneGameState.h"
#include "Scene/SceneTitle.h"
#include "Scene/Stage/StageManager.h"
#include "Scene/Stage/TestingStage.h"

#include "GameObject/Character/Player/PlayerManager.h"
#include "GameObject/Projectile/ProjectileManager.h"

#include "Map/MapTileManager.h"
#include "Map/MapTile.h"

#include "UI/Widget/WidgetText.h"
#include "UI/Widget/WidgetCrosshair.h"

#include "GameData.h"

SceneGame::SceneGame(const char* name, const char* host, const char* port, NetworkController* networkController)
{
	this->name = Encode::utf8_to_string((char8_t*)name);

	this->host = host;
	this->port = port;
	this->networkController = networkController;
};

void SceneGame::Initialize()
{
	ID3D11Device* device = T_GRAPHICS.GetDevice();
	float screenWidth = T_GRAPHICS.GetScreenWidth();
	float screenHeight = T_GRAPHICS.GetScreenHeight();

	// Sprite Resource Preload
	for (auto& filename : spriteList)
	{
		spritePreLoad.insert(RESOURCE.LoadSpriteResource(filename));
	}
	// Model Resource Preload
	for (auto& filename : modelList)
	{
		modelPreLoad.insert(RESOURCE.LoadModelResource(filename));
	}

	//シャドウマップレンダラ
	shadowMapRenderer->Initialize();

	//ライト情報
	LightManager::Instance().SetAmbientColor({ 0, 0, 0, 0 });
	Light* dl = new Light(LightType::Directional);
	dl->SetDirection({ 0.0f, -0.503f, -0.864f });
	LightManager::Instance().Register(dl);
	shadowMapRenderer->SetShadowLight(dl);

	// カメラ設定
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),							// 画角
		T_GRAPHICS.GetScreenWidth() / T_GRAPHICS.GetScreenHeight(),	// 画面アスペクト比
		0.1f,														// ニアクリップ
		10000.0f);													// ファークリップ
	
	camera.SetLookAt(
		{ 0, 5.0f, 10.0f },	     // 視点
		{ 0, 0, 0 },	         // 注視点
		{ 0, 0.969f, -0.248f }); // 上ベクトル
	
	cameraController = std::make_unique<ThridPersonCameraController>();
	cameraController->SyncCameraToController(camera);
	cameraController->SetEnable(false);

	// Network
	if (networkController != nullptr)
	{
		if (!networkController->Initialize(host.c_str(), port.c_str()))
		{
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
			return;
		}
		else
		{
			networkController->Reset();
			GAME_DATA.SetName(name.c_str());
		}
	}

	//ステージ
	stage = std::make_unique<TestingStage>();
	stage->Initialize();

	//	モデルをレンダラーに登録
	Model* list[] =
	{
		MAPTILES.get(0)->GetModel().get(),
	};
	for (Model* model : list)
	{
		if (!model) continue;
		shadowMapRenderer->ModelRegister(model);
	}

	stateMachine = std::make_unique<StateMachine<SceneGame>>();
	stateMachine->RegisterState(GAME_STATE::WAITING, new SceneGameState::WaitingState(this));
	stateMachine->RegisterState(GAME_STATE::READY,   new SceneGameState::ReadyState(this));
	stateMachine->RegisterState(GAME_STATE::GAME,    new SceneGameState::GameState(this));
	stateMachine->RegisterState(GAME_STATE::GAME_OVER, new SceneGameState::GameOverState(this));
	stateMachine->RegisterState(GAME_STATE::WIN,     new SceneGameState::WinState(this));
	stateMachine->SetState(GAME_STATE::WAITING);

	WidgetText* ip = new WidgetText(host.c_str(), 0.8f);
	ip->SetBorder(1);
	UI.Register(ip);

	menu = new WidgetMenu;
	UI.Register(menu);

#ifdef _DEBUG
	//T_CONSOLE.Open();
#endif
}

void SceneGame::Finalize()
{
	modelPreLoad.clear();
	spritePreLoad.clear();
	shadowMapRenderer->Clear();
	CURSOR_ON;
	PLAYERS.Clear();
	MAPTILES.Clear();
	ENEMIES.Clear();
	PROJECTILES.Clear();
	ENEMYCOLLISIONS.Clear();
	STAGES.Clear();
	UI.Clear();
	T_CONSOLE.Close();

	// 中断接続
	switch (stateMachine->GetStateIndex())
	{
	case GAME_STATE::GAME:
	case GAME_STATE::GAME_OVER:
	case GAME_STATE::WIN:
		networkController->SendDisconnection();
		break;
	}

	networkController->Finalize();
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	if (stateMachine->GetStateIndex() != networkController->GetGameState())
	{
		stateMachine->ChangeState(networkController->GetGameState());
	}

	stateMachine->Update(elapsedTime);

	{
		ProfileScopedSection_2("Map", ImGuiControl::Profiler::Dark);
		// マップ更新
		MAPTILES.Update(elapsedTime);
	}

	{
		ProfileScopedSection_2("Player", ImGuiControl::Profiler::Yellow);
		// プレイヤー更新
		PLAYERS.Update(elapsedTime);
	}

	{
		ProfileScopedSection_2("Enemy", ImGuiControl::Profiler::Red);
		// エネミー更新
		ENEMIES.Update(elapsedTime);
	}

	{
		ProfileScopedSection_2("Projectiles", ImGuiControl::Profiler::Purple);
		// 発射物更新
		PROJECTILES.Update(elapsedTime);
	}

	{
		ProfileScopedSection_2("Collision", ImGuiControl::Profiler::Yellow);
		// オブジェクト間衝突判定
		COLLISION.Update(elapsedTime);
	}

	{
		ProfileScopedSection_2("Stages", ImGuiControl::Profiler::Green);
		// ステージオブジェクト更新
		STAGES.Update(elapsedTime);
	}

	{
		ProfileScopedSection_2("Effect", ImGuiControl::Profiler::Dark);
		// エフェクト更新
		EFFECTS.Update(elapsedTime);
	}

	// カメラ更新
	cameraController->Update(elapsedTime);
	cameraController->SyncContrllerToCamera(camera);

	{
		ProfileScopedSection_2("Effect", ImGuiControl::Profiler::Purple);

		// メニュー
		if (T_INPUT.KeyDown(VK_ESCAPE) || T_INPUT.GamePadKeyDown(GAME_PAD_BTN::BACK))
		{
			if (menu->IsShow())
			{
				menu->Hide();
			}
			else
			{
				menu->Show();
			}
		}

		UI.MoveToEnd(menu);
		// インターフェース更新
		UI.Update(elapsedTime);
	}
}

void SceneGame::UpdateConnection()
{
	ProfileScopedSection_2("Network", ImGuiControl::Profiler::Blue);
	if (T_INPUT.KeyDown(VK_SPACE) || T_INPUT.GamePadKeyDown(GAME_PAD_BTN::START))
	{
		networkController->ReadyTrigger();
	}
	if (!networkController->IsReady())
	{
		int classType = networkController->GetClassType();
		if (T_INPUT.KeyDown(VK_UP) || T_INPUT.GamePadKeyDown(GAME_PAD_BTN::DPAD_UP))
		{
			classType--;
		}
		if (T_INPUT.KeyDown(VK_DOWN) || T_INPUT.GamePadKeyDown(GAME_PAD_BTN::DPAD_DOWN))
		{
			classType++;
		}
		if (classType < 0) classType = PLAYER_CLASS::End - 1;
		if (classType >= PLAYER_CLASS::End) classType = 0;
		if (classType != networkController->GetClassType()) networkController->SetClassType(classType);

		int colorIdx = networkController->GetColorIndex();
		if (T_INPUT.KeyDown(VK_LEFT) || T_INPUT.GamePadKeyDown(GAME_PAD_BTN::DPAD_LEFT))
		{
			colorIdx--;
		}
		if (T_INPUT.KeyDown(VK_RIGHT) || T_INPUT.GamePadKeyDown(GAME_PAD_BTN::DPAD_RIGHT))
		{
			colorIdx++;
		}
		if (colorIdx < 0) colorIdx = Player::COLOR_PATTERN::END - 1;
		if (colorIdx >= Player::COLOR_PATTERN::END) colorIdx = Player::COLOR_PATTERN::DEFAULT;
		if (colorIdx != networkController->GetColorIndex()) networkController->SetColorIndex(colorIdx);
	}
	networkController->UpdateConnection();
	// 位置
	PLAYERS.InitializePosition();
}

bool SceneGame::UpdateGame(float elapsedTime)
{
	ProfileScopedSection_2("Network", ImGuiControl::Profiler::Blue);
	if (networkController->IsHost())
	{
		stage->Update(elapsedTime); // ホスト・敵生成

		if (stage->IsFinish())
		{
			networkController->SetGameState(GAME_STATE::WIN);
		}
		bool alive = false;
		for (Player*& player : PLAYERS.GetAll())
		{
			if (player->GetHp() > 0)
			{
				alive = true;
				break;
			}
		}
		if (!alive) networkController->SetGameState(GAME_STATE::GAME_OVER);
	}

	if (cameraController->isEnable())
	{
		T_INPUT.KeepCursorCenter();
	}

	networkController->Update();

	return true;
}

// 描画処理
void SceneGame::Render()
{
	T_TEXT.Begin();
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Scene)->Clear(T_GRAPHICS.GetDeviceContext(), 0.2f, 0.2f, 0.2f, 1);
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Scene)->SetRenderTarget(T_GRAPHICS.GetDeviceContext());

	// 描画コンテキスト設定
	RenderContext rc;
	rc.camera = &camera;
	rc.deviceContext = T_GRAPHICS.GetDeviceContext();
	rc.renderState   = T_GRAPHICS.GetRenderState();

	// 内容描画
	{
		// ライトの情報を詰め込む
		LightManager::Instance().PushRenderContext(rc);

		//シャドウマップ描画
		shadowMapRenderer->Render();
		rc.shadowMapData = shadowMapRenderer->GetShadowMapData();
		
		MAPTILES.Render(rc);			// マップ
		PLAYERS.Render(rc);				// プレイヤー
		ENEMIES.Render(rc);				// エネミー
		PROJECTILES.Render(rc);			// 発射物
		STAGES.Render(rc);				// ステージオブジェクト
		EFFECTS.Render(camera.GetView(), camera.GetProjection()); // エフェクト
	}
#ifdef _DEBUG
	{
		PROJECTILES.DrawDebugPrimitive();
	}
#endif // _DEBUG

	// ラインレンダラ描画実行
	T_GRAPHICS.GetLineRenderer()->Render(T_GRAPHICS.GetDeviceContext(), camera.GetView(), camera.GetProjection());
	// デバッグレンダラ描画実行
	T_GRAPHICS.GetDebugRenderer()->Render(T_GRAPHICS.GetDeviceContext(), camera.GetView(), camera.GetProjection());

	//	ポストプロセス処理を行う
	{
		T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->Clear(T_GRAPHICS.GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
		T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->SetRenderTarget(T_GRAPHICS.GetDeviceContext());
		postprocessingRenderer->Render(T_GRAPHICS.GetDeviceContext());
	}

	UI.Render(rc);						// インターフェース

	T_TEXT.End();

#ifdef _DEBUG
	ProfileDrawUI();
	shadowMapRenderer->DrawDebugGUI();
#endif // _DEBUG
}
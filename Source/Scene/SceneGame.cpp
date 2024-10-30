#include "SceneGame.h"

#include <locale.h>
#include <profiler.h>
#include <algorithm>
#include <fstream>

#include "TAKOEngine/Runtime/tentacle_lib.h"

#include "TAKOEngine/Network/Network.h"
#include "TAKOEngine/GUI/UIManager.h"
#include "TAKOEngine/GUI/Dialog.h"
#include "TAKOEngine/Physics/Collision.h"
#include "TAKOEngine/Physics/CollisionDataManager.h"
#include "TAKOEngine/Effects/EffectManager.h"
#include "TAKOEngine/Tool/Encode.h"
#include "TAKOEngine/Editor/Camera/CameraManager.h"


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

	LoadSettings();
};

void SceneGame::Initialize()
{
	ID3D11Device* device = T_GRAPHICS.GetDevice();
	float screenWidth = T_GRAPHICS.GetScreenWidth();
	float screenHeight = T_GRAPHICS.GetScreenHeight();
	CameraManager& cameraManger = CameraManager::Instance();
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

	//DeferredRendering
	deferredRendering->Initialize();

	//シャドウマップレンダラ
	shadowMapRenderer->Initialize();

	//ライト情報
	LightManager::Instance().SetAmbientColor({ 0, 0, 0, 0 });
	Light* dl = new Light(LightType::Directional);
	dl->SetDirection({ 0.0f, -0.503f, -0.864f });
	LightManager::Instance().Register(dl);
	shadowMapRenderer->SetShadowLight(dl);

	// カメラ設定
	Camera* mainCamera = new Camera();
	cameraManger.Register(mainCamera);
	cameraManger.SetCamera(0);
	

	CameraManager::Instance().GetCamera()->SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),							// 画角
		T_GRAPHICS.GetScreenWidth() / T_GRAPHICS.GetScreenHeight(),	// 画面アスペクト比
		0.1f,														// ニアクリップ
		10000.0f);													// ファークリップ

	CameraManager::Instance().GetCamera()->SetLookAt(
		{ 0, 5.0f, 10.0f },	     // 視点
		{ 0, 0, 0 },	         // 注視点
		{ 0, 0.969f, -0.248f }); // 上ベクトル

	cameraController = std::make_unique<ThridPersonCameraController>();
	cameraController->SyncCameraToController(CameraManager::Instance().GetCamera());
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
	iModel* list[] =
	{
		MAPTILES.get(0)->GetModel().get(),
	};
	for (iModel* model : list)
	{
		if (!model) continue;
		shadowMapRenderer->ModelRegister(model);
	}

	stateMachine = std::make_unique<StateMachine<SceneGame>>();
	stateMachine->RegisterState(GAME_STATE::WAITING, new SceneGameState::WaitingState(this));
	stateMachine->RegisterState(GAME_STATE::READY, new SceneGameState::ReadyState(this));
	stateMachine->RegisterState(GAME_STATE::GAME, new SceneGameState::GameState(this));
	stateMachine->RegisterState(GAME_STATE::GAME_OVER, new SceneGameState::GameOverState(this));
	stateMachine->RegisterState(GAME_STATE::WIN, new SceneGameState::WinState(this));
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
	CameraManager::Instance().Clear();
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




	CameraManager::Instance().Update();
	std::vector<DirectX::XMFLOAT3> cameraFocusPoints = {
		{CameraManager::Instance().GetCamera()->GetFocus().x, CameraManager::Instance().GetCamera()->GetFocus().y, CameraManager::Instance().GetCamera()->GetFocus().z},
		{0,3,4},
		{2,5,7},
		{ 5,1,2},
		{ 5,6,4}
	};
	// ゲームループ内で
	
	if (T_INPUT.KeyPress(VK_SHIFT))
	{
		CameraManager::Instance().GetCamera()->MovePointToCamera(cameraPositions, cameraFocusPoints,transitionTime,transitionDuration,elapsedTime);
	}
	else
	{
		cameraController->SyncContrllerToCamera(CameraManager::Instance().GetCamera());
		CameraManager::Instance().GetCamera()->GetSegment() = 0;
		cameraController->Update(elapsedTime);
		transitionTime = 0;
	}
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
	
	rc.deviceContext = T_GRAPHICS.GetDeviceContext();
	rc.renderState = T_GRAPHICS.GetRenderState();
	rc.camera = CameraManager::Instance().GetCamera();

	// ライトの情報を詰め込む
	LightManager::Instance().PushRenderContext(rc);

	//シャドウマップ描画
	shadowMapRenderer->Render();
	rc.shadowMapData = shadowMapRenderer->GetShadowMapData();
	// 内容描画
	{
		//Deferred Rendering
		deferredRendering->SetDeferredRTV();
		
		//オブジェクト描画
		MAPTILES.Render(rc);						// マップ
		PLAYERS.Render(rc);						// プレイヤー
		ENEMIES.Render(rc);						// エネミー
		PROJECTILES.Render(rc);						// 発射物
		STAGES.Render(rc);						// ステージオブジェクト
		EFFECTS.Render(CameraManager::Instance().GetCamera()->GetView(), CameraManager::Instance().GetCamera()->GetProjection()); 	// エフェクト
		CameraGUI();
	}
#ifdef _DEBUG
	{
		PROJECTILES.DrawDebugPrimitive();
		T_GRAPHICS.GetDebugRenderer()->DrawSphere2(cameraPositions,2,{1,0,0,1});
		
	}
#endif // _DEBUG

	// ラインレンダラ描画実行
	T_GRAPHICS.GetLineRenderer()->Render(T_GRAPHICS.GetDeviceContext(), CameraManager::Instance().GetCamera()->GetView(), CameraManager::Instance().GetCamera()->GetProjection());
	// デバッグレンダラ描画実行
	T_GRAPHICS.GetDebugRenderer()->Render(T_GRAPHICS.GetDeviceContext(), CameraManager::Instance().GetCamera()->GetView(), CameraManager::Instance().GetCamera()->GetProjection());

	//	ポストプロセス処理を行う
	{
		T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->Clear(T_GRAPHICS.GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
		T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->SetRenderTarget(T_GRAPHICS.GetDeviceContext());
		postprocessingRenderer->Render(T_GRAPHICS.GetDeviceContext());
	}

	//DeferredRendering
	deferredRendering->Render();

	UI.Render(rc);						// インターフェース

	T_TEXT.End();

#ifdef _DEBUG
	ProfileDrawUI();
	shadowMapRenderer->DrawDebugGUI();
	deferredRendering->DrawDebugGUI();
#endif // _DEBUG
}
void SceneGame::CameraGUI()
{

	if (ImGui::TreeNode("CameraMovePar"))
	{
		ImGui::DragFloat("MoveEyeX", &MovePar[CameraMovePar::MoveEyeX], 1.0f, 0.f,FLT_MAX);
		ImGui::DragFloat("MoveEyeY", &MovePar[CameraMovePar::MoveEyeY], 1.0f, 0.f, FLT_MAX);
		ImGui::DragFloat("MoveEyeZ", &MovePar[CameraMovePar::MoveEyeZ], 1.0f, 0.f, FLT_MAX);
		ImGui::DragFloat("MoveDuration", &MovePar[CameraMovePar::MoveDuration], 1.0f, 0.f, FLT_MAX);
		ImGui::TreePop();
	}
	else if (ImGui::TreeNode("CameraRotatePar"))
	{
		ImGui::DragFloat("Angle", &RotatePar[CameraRotatePar::Angle], 1.0f, 0.f, FLT_MAX);
		ImGui::DragFloat("Radius", &RotatePar[CameraRotatePar::Radius], 1.0f, 0.f, FLT_MAX);
		ImGui::DragFloat("Speed", &RotatePar[CameraRotatePar::Speed], 1.0f, 0.f, FLT_MAX);
		ImGui::TreePop();
	}
	if (ImGui::Button("Save Settings")) {
		SaveSettings();
	}
	ImGui::InputFloat("time", &transitionTime);

	if (ImGui::TreeNode("Camera Positions"))
	{
		for (size_t i = 0; i < cameraPositions.size(); ++i)
		{
			std::string label = "Position " + std::to_string(i);  // 各カメラポジションのラベル
			ImGui::DragFloat3(label.c_str(), &cameraPositions[i].x, 1.0f, -FLT_MAX, FLT_MAX);  // カメラポジションの設定
		}
		ImGui::InputInt("segment", &currentSegment);
		ImGui::TreePop();
	}
}
void SceneGame::SaveSettings()
{
	std::ofstream file("settings.txt");
	if (file.is_open()) {
		// カメラの動的パラメータを保存
		{
			file << "MoveEyeX " << MovePar[CameraMovePar::MoveEyeX] << "\n";
			file << "MoveEyeY " << MovePar[CameraMovePar::MoveEyeY] << "\n";
			file << "MoveEyeZ " << MovePar[CameraMovePar::MoveEyeZ] << "\n";
			file << "MoveDuration " << MovePar[CameraMovePar::MoveDuration] << "\n";
		}
		//カメラ回転パラメータを保存
		{
			file << "Angle " << RotatePar[CameraRotatePar::Angle] << "\n";
			file << "Radius " << RotatePar[CameraRotatePar::Radius] << "\n";
			file << "Speed " << RotatePar[CameraRotatePar::Speed] << "\n";
		}
		file.close();
	}
}
void SceneGame::LoadSettings() {
	std::ifstream file("settings.txt");
	if (file.is_open()) {
		std::string settingName;
		while (file >> settingName >> settingValue) {
			if (settingName == "MoveEyeX") {
				MovePar[CameraMovePar::MoveEyeX] = settingValue;
			}
			if (settingName == "MoveEyeY") {
				MovePar[CameraMovePar::MoveEyeY] = settingValue;
			}
			if (settingName == "MoveEyeZ") {
				MovePar[CameraMovePar::MoveEyeZ] = settingValue;
			}
			if (settingName == "MoveDuration") {
				MovePar[CameraMovePar::MoveDuration] = settingValue;
			}
			if (settingName == "Angle") {
				RotatePar[CameraRotatePar::Angle] = settingValue;
			}
			if (settingName == "Radius") {
				RotatePar[CameraRotatePar::Radius] = settingValue;
			}
			if (settingName == "Speed") {
				RotatePar[CameraRotatePar::Speed] = settingValue;
			}
		}
		file.close();
	}
}
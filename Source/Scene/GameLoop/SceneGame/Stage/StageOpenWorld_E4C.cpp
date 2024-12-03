#include "StageOpenWorld_E4C.h"

#include "TAKOEngine/GUI/UIManager.h"

#include "GameObject/ModelObject.h"
#include "GameObject/Props/Teleporter.h"
#include "Scene/Stage/StageManager.h"
#include "Scene/Stage/Stage.h"

#include "Map/MapTileManager.h"
#include "Map/MapTile.h"

#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"
#include "TAKOEngine/Tool/Timer.h"

#include "GameObject/Character/Player/PlayerCharacterManager.h"

#include "Scene/GameLoop/SceneGame/Stage/StageDungeon_E4C.h"

#include "Network/OnlineController.h"

#include "Scene/GameLoop/SceneGame/SceneGame_E4C.h"

static float timer = 0;

void StageOpenWorld_E4C::Initialize()
{
	Stage::Initialize(); // デフォルト

	// Sprite Resource Preload
	for (auto& filename : spriteList)
	{
		spritePreLoad.insert(RESOURCE.LoadSpriteResource(filename));
	}

	stage_collision = new MapTile("Data/Model/Stage/Terrain_Collision.glb", 0.01f);
	stage_collision->Update(0);
	MAPTILES.Register(stage_collision);
	MAPTILES.CreateSpatialIndex(5, 7);

	map = std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Map.glb", 1.0f, ModelObject::RENDER_MODE::DX11GLTF);
	tower = std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Tower.glb", 1.0f);

	teleporter = std::make_unique<Teleporter>(new StageDungeon_E4C(m_pScene), m_pScene->GetOnlineController());
	teleporter->SetPosition({ 16, 8.5, -46 });
	teleporter->SetScale({ 5.0f, 10.0f, 1.0f });

	portalSquare = std::make_unique<Plane>(T_GRAPHICS.GetDevice(), "", 1.0f, XMFLOAT3{ 10.0f,10.0f,5.0f }, 5.0f, 5.0f);
	portalSquare->SetShader(ModelShaderId::PortalSquare);
	
	// 光
	LightManager::Instance().SetAmbientColor({ 0, 0, 0, 0 });
	Light* dl = new Light(LightType::Directional);
	dl->SetDirection({ 0.0f, -0.503f, -0.864f });
	LightManager::Instance().Register(dl);

	// プレイヤー
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
	player->SetPosition({ 15.0f, 15.0f, 5.0f });
	// プレイヤーが走るときの土埃
	runningDust1 = std::make_unique<RunningDust>( T_GRAPHICS.GetDevice(), "Data/Sprites/smoke.png", 100.0f,
										player->GetPosition(),	// position
										1.0f,			// alpha
										f_count,	// model_id
										0 );		// age
	//runningDust.push_back(Fireball{ T_GRAPHICS.GetDevice(), "Data/Sprites/smoke.png", 100.0f,
	//									player->GetPosition(),	// position
	//									1.0f,			// alpha
	//									f_count,	// model_id
	//									0 });		// age

	// カメラ設定
	Camera* mainCamera = CameraManager::Instance().GetCamera();
	mainCamera->SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),							// 画角
		T_GRAPHICS.GetScreenWidth() / T_GRAPHICS.GetScreenHeight(),	// 画面アスペクト比
		0.1f,														// ニアクリップ
		10000.0f													// ファークリップ
	);
	mainCamera->SetLookAt(
		{ 0, 5.0f, 5.0f },		// 視点
		player->GetPosition(),			// 注視点
		{ 0, 0.969f, -0.248f }	// 上ベクトル
	);

	cameraController = std::make_unique<ThridPersonCameraController>();
	cameraController->SyncCameraToController(mainCamera);
	cameraController->SetEnable(true);
	cameraController->SetPlayer(player);
	CURSOR_OFF;
}

void StageOpenWorld_E4C::Update(float elapsedTime)
{
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();

	Camera* camera = CameraManager::Instance().GetCamera();
	Online::OnlineController* onlineController = m_pScene->GetOnlineController();
	if (onlineController->GetState() == Online::OnlineController::STATE::LOGINED)
	{
		onlineController->BeginSync();
	}

	// ゲームループ内で
	cameraController->SyncContrllerToCamera(camera);
	cameraController->Update(elapsedTime);

	if (T_INPUT.KeyDown(VK_MENU))
	{
		if (TentacleLib::isShowCursor())
		{
			cameraController->SetEnable(true);
			CURSOR_OFF;
		}
		else
		{
			cameraController->SetEnable(false);
			CURSOR_ON;
		}
	}
	if (!TentacleLib::isFocus())
	{
		cameraController->SetEnable(false);
		CURSOR_ON;
	}
	if (cameraController->isEnable())
	{
		T_INPUT.KeepCursorCenter();
	}
	PlayerCharacterManager::Instance().Update(elapsedTime);
	map->Update(elapsedTime);
	tower->Update(elapsedTime);

	teleporter->Update(elapsedTime);

	portalSquare->Update(elapsedTime);
	runningDust1->Update(elapsedTime);

	timer += elapsedTime;
}

void StageOpenWorld_E4C::Render()
{
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->Clear(T_GRAPHICS.GetDeviceContext(), 0.2f, 0.2f, 0.2f, 1);
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->SetRenderTarget(T_GRAPHICS.GetDeviceContext());

	// 描画コンテキスト設定
	RenderContext rc;
	rc.camera = CameraManager::Instance().GetCamera();
	rc.deviceContext = T_GRAPHICS.GetDeviceContext();
	rc.renderState = T_GRAPHICS.GetRenderState();

	rc.timerGlobal = timer;
	rc.timerTick = TentacleLib::Timer::Instance().Delta();

	// ライトの情報を詰め込む
	LightManager::Instance().PushRenderContext(rc);

	// 描画
	PlayerCharacterManager::Instance().Render(rc);

	map->Render(rc);
	tower->Render(rc);

	teleporter->Render(rc);

	portalSquare->Render(rc);
	runningDust1->Render(rc);

	UI.Render(rc);

	//MAPTILES.Render(rc);

	//if (ImGui::TreeNode("Camera Positions"))
	//{
	//	for (size_t i = 0; i < cameraPositions.size(); ++i)
	//	{
	//		std::string label = "Position " + std::to_string(i);  // 各カメラポジションのラベル
	//		ImGui::DragFloat3(label.c_str(), &cameraPositions[i].x, 1.0f, -FLT_MAX, FLT_MAX);  // カメラポジションの設定
	//	}
	//	ImGui::TreePop();
	//}
	// デバッグレンダラ描画実行

	T_GRAPHICS.GetDebugRenderer()->Render(T_GRAPHICS.GetDeviceContext(), CameraManager::Instance().GetCamera()->GetView(), CameraManager::Instance().GetCamera()->GetProjection());
}

void StageOpenWorld_E4C::OnPhase()
{
}
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

	// フレームバッファマネージャー
	m_frameBuffer = T_GRAPHICS.GetFrameBufferManager();

	// Sprite Resource Preload
	/*for (auto& filename : spriteList)
	{
		spritePreLoad.insert(RESOURCE.LoadSpriteResource(filename));
	}*/

	stage_collision = new MapTile("Data/Model/Stage/Terrain_Collision.glb", 0.01f);
	stage_collision->Update(0);
	MAPTILES.Register(stage_collision);
	MAPTILES.CreateSpatialIndex(5, 7);

	map = std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Map.glb", 1.0f, ModelObject::RENDER_MODE::DX12, 0);
	//tower = std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Tower.glb", 1.0f, ModelObject::RENDER_MODE::DX12, 0);

	sky = std::make_unique<ModelObject>("Data/Model/Cube/Cube.fbx", 100.0f, ModelObject::RENDER_MODE::DX12);
	m_sprites[1] = std::make_unique<SpriteDX12>(1, L"Data/Model/Stage/skybox.dds");

	teleporter = std::make_unique<Teleporter>(new StageDungeon_E4C(m_pScene), m_pScene->GetOnlineController());
	teleporter->SetPosition({ 16, 8.5, -46 });
	teleporter->SetScale({ 5.0f, 10.0f, 1.0f });

	{
		std::array<DirectX::XMFLOAT3, 4 > positions = {
			DirectX::XMFLOAT3{ 10.0f, 10.0f, 5.0f},
			DirectX::XMFLOAT3{ 10.0f, 20.0f, 5.0f },
			DirectX::XMFLOAT3{ 5.0f, 10.0f, 5.0f },
			DirectX::XMFLOAT3{ 5.0f, 20.0f, 5.0f }
		};

		plane = std::make_unique<Plane>(T_GRAPHICS.GetDevice(), "Data/Sprites/gem.png", 1.0f, positions);

		billboard = std::make_unique<Fireball>(T_GRAPHICS.GetDevice(), "Data/Sprites/fire.png", 1.0f, positions[0]);
	}

	map12 = std::make_unique<ModelObject>("Data/Model/Enemy/Goblin.glb", 1.0f, ModelObject::RENDER_MODE::DX12);

	// 光
	LightManager::Instance().SetAmbientColor({ 0.1f, 0.1f, 0.1f, 0.1f });
	Light* dl = new Light(LightType::Directional);
	dl->SetDirection({ 0.0f, -0.503f, -0.864f });
	LightManager::Instance().Register(dl);

	// プレイヤー
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
	player->SetPosition({ 15.0f, 15.0f, 5.0f });

	playerModels.resize(50);

	for (size_t i = 0; i < playerModels.size(); i++)
	{
		playerModels[i] = std::make_unique<ModelObject>("Data/Model/Character/PlayerModels/ANM_PCMsword_Prototype_3_ANIMATION.glb", 1.0f, ModelObject::RENDER_MODE::DX12);
		playerModels[i]->SetPosition({ 3.0f * ((i % 5)), 1.0f, 3.0f * (i / 5) });
		playerModels[i]->SetAnimation(0, true);
	}

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

	for (size_t i = 0; i < playerModels.size(); i++)
	{
		playerModels[i]->Update(elapsedTime + (elapsedTime / 100.0f) * i);
	}

	sky->Update(elapsedTime);

	teleporter->Update(elapsedTime);
	//plane->Update(elapsedTime);
	//billboard->Update(elapsedTime);

	//timer += elapsedTime;

	map12->Update(elapsedTime);
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

void StageOpenWorld_E4C::RenderDX12()
{
	T_GRAPHICS.BeginRender();

	// シーン用定数バッファ更新
	const Descriptor* scene_cbv_descriptor = T_GRAPHICS.UpdateSceneConstantBuffer(
		CameraManager::Instance().GetCamera());

	// レンダーコンテキスト設定
	RenderContextDX12 rc;
	rc.d3d_command_list = m_frameBuffer->GetCommandList();
	rc.scene_cbv_descriptor = scene_cbv_descriptor;

	// 3Dモデル描画
	{
		m_frameBuffer->WaitUntilToPossibleSetRenderTarget(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Scene));
		m_frameBuffer->SetRenderTarget(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Scene));
		m_frameBuffer->Clear(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Scene));

		PlayerCharacterManager::Instance().RenderDX12(rc);

		map->RenderDX12(rc);
		//tower->RenderDX12(rc);

		for (size_t i = 0; i < playerModels.size(); i++)
		{
			playerModels[i]->RenderDX12(rc);
		}

		// skyBox
		{
			rc.skydomeData.skyTexture = m_sprites[1]->GetDescriptor();
			ModelShaderDX12* shader = T_GRAPHICS.GetModelShaderDX12(ModelShaderDX12Id::Skydome);
			//shader->Render(rc, sky->GetModel().get());
		}

		// レンダーターゲットへの書き込み終了待ち
		m_frameBuffer->WaitUntilFinishDrawingToRenderTarget(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Scene));
	}

	// ポストエフェクト描画
	{
		postprocessingRenderer->Render(m_frameBuffer);
	}

	// 2D描画
	{

	}

	T_GRAPHICS.End();
}

void StageOpenWorld_E4C::OnPhase()
{
}
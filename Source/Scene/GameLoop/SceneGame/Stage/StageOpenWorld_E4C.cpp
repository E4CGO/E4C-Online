//! @file StageOpenWorld_E4C.cpp
//! @note

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
#include "TAKOEngine/Physics/CollisionManager.h"
#include "TAKOEngine/Sound/Sound.h"

#include "GameObject/Character/Player/PlayerCharacterManager.h"
#include "GameObject/Character/Enemy/EnemyManager.h"
#include "GameObject/Props/SpawnerManager.h"

#include "Map/DungeonData.h"

#include "Scene/GameLoop/SceneGame/Stage/StageDungeon_E4C.h"

#include "Network/OnlineController.h"

#include "Scene/GameLoop/SceneGame/SceneGame_E4C.h"

void StageOpenWorld_E4C::Initialize()
{
	Stage::Initialize(); // デフォルト

	// フレームバッファマネージャー
	m_frameBuffer = T_GRAPHICS.GetFrameBufferManager();

	// Sprite Resource Preload
	for (auto& filename : spriteList)
	{
		spritePreLoad.insert(RESOURCE.LoadSpriteResource(filename));
	}

	m_pCharacterGauge = new WidgetPlayerHP();
	m_pPauseMenu = new WidgetPauseMenu();
	UI.Register(m_pCharacterGauge);
	UI.Register(m_pPauseMenu);

	stage_collision = new MapTile("Data/Model/Stage/Terrain_Collision.glb", 0.01f);
	village_collision = new MapTile("Data/Model/Stage/Terrain_Village_Collision.glb", 1.f);
	stage_collision->Update(0);
	village_collision->Update(0);
	stage_collision->SetMoveCollider(Collider::COLLIDER_TYPE::MAP, Collider::COLLIDER_OBJ::OBSTRUCTION);
	village_collision->SetMoveCollider(Collider::COLLIDER_TYPE::MAP, Collider::COLLIDER_OBJ::OBSTRUCTION);

	MAPTILES.Register(stage_collision);
	MAPTILES.Register(village_collision);
	MAPTILES.CreateSpatialIndex(5, 7);

	teleporter = std::make_unique<Teleporter>(new StageDungeon_E4C(m_pScene), m_pScene->GetOnlineController());
	teleporter->SetPosition({ -34.0f, 6.0f, -43.5f });
	teleporter->SetScale({ 5.0f, 10.0f, 1.0f });
	teleporter->SetVisibility(true);

	Spawner* spawner = new Spawner(ENEMY_TYPE::MOUSE, 1, -1);
	spawner->SetPosition({ 15.7f, 4.7f, -42.0f });
	spawner->SetSearchRadius(10.0f);
	SpawnerManager::Instance().Register(spawner);

	if (T_GRAPHICS.isDX11Active)
	{
		models.emplace("map", std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Map.glb", 1.0f, ModelObject::RENDER_MODE::DX11, ModelObject::MODEL_TYPE::LHS_Phong));
		models.emplace("village", std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Village.glb", 1.0f, ModelObject::RENDER_MODE::DX11, ModelObject::MODEL_TYPE::LHS_Phong));

		models.emplace("bush", std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Bush.glb", 1.0f, ModelObject::RENDER_MODE::DX11, ModelObject::MODEL_TYPE::LHS_Phong));
		models.emplace("tree1t", std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Tree_1_Trunk.glb", 1.0f, ModelObject::RENDER_MODE::DX11, ModelObject::MODEL_TYPE::LHS_Phong));
		models.emplace("tree1l", std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Tree_1_Leaf.glb", 1.0f, ModelObject::RENDER_MODE::DX11, ModelObject::MODEL_TYPE::LHS_Phong));
		models.emplace("tree2t", std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Tree_2_Trunk.glb", 1.0f, ModelObject::RENDER_MODE::DX11, ModelObject::MODEL_TYPE::LHS_Phong));
		models.emplace("tree2l", std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Tree_2_Leaf.glb", 1.0f, ModelObject::RENDER_MODE::DX11, ModelObject::MODEL_TYPE::LHS_Phong));
		models.emplace("flower", std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Flower.glb", 1.0f, ModelObject::RENDER_MODE::DX11, ModelObject::MODEL_TYPE::LHS_Phong));

		models.emplace("target1", std::make_unique<ModelObject>("Data/Model/Object/BlockTarget.glb", 1.0f, ModelObject::RENDER_MODE::DX11, ModelObject::MODEL_TYPE::LHS_Phong));
		models["target1"]->SetPosition({ -34.9f, 1.8f, 20.4f });
		models["target1"]->SetAngle({ 0.0f, -0.84f, 0.0f });
		models.emplace("target2", std::make_unique<ModelObject>("Data/Model/Object/CloseTarget1.glb", 1.0f, ModelObject::RENDER_MODE::DX11, ModelObject::MODEL_TYPE::LHS_Phong));
		models["target2"]->SetPosition({ -38.1f, 1.80f, 16.2f });
		models["target2"]->SetAngle({ 0.0f, -1.0f, 0.0f });
		models.emplace("target3", std::make_unique<ModelObject>("Data/Model/Object/CloseTarget2.glb", 1.0f, ModelObject::RENDER_MODE::DX11, ModelObject::MODEL_TYPE::LHS_Phong));
		models["target3"]->SetPosition({ -32.0f, 1.80f, 23.4f });
		models["target3"]->SetAngle({ 0.0f, -1.0f, 0.0f });

		//// プレイヤーが走るときの土埃
		//runningDust1 = std::make_unique<RunningDust>(T_GRAPHICS.GetDevice(), "Data/Sprites/smoke.png", 100.0f,
		//	DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),	// position
		//	1.0f,			// alpha
		//	f_count,	// model_id
		//	0);		// age
	}

	if (T_GRAPHICS.isDX12Active)
	{
		models.emplace("map", std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Map.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR));
		models.emplace("village", std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Village.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR));

		models.emplace("bush", std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Bush.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::Foliage));
		models.emplace("tree1t", std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Tree_1_Trunk.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR));
		models.emplace("tree1l", std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Tree_1_Leaf.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::Foliage));
		models.emplace("tree2t", std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Tree_2_Trunk.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR));
		models.emplace("tree2l", std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Tree_2_Leaf.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::Foliage));
		models.emplace("flower", std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Flower.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::Foliage));

		models.emplace("portal", std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Portal.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR));
		models["portal"]->SetPosition({ -34.0f, 4.0f, -45.0f });
		models["portal"]->SetAngle({ 0.0f, 1.5f, 0.0f });

		models.emplace("target1", std::make_unique<ModelObject>("Data/Model/Object/BlockTarget.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR));
		models["target1"]->SetPosition({ -34.9f, 1.8f, 20.4f });
		models["target1"]->SetAngle({ 0.0f, -0.84f, 0.0f });
		models.emplace("target2", std::make_unique<ModelObject>("Data/Model/Object/CloseTarget1.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR));
		models["target2"]->SetPosition({ -38.1, 1.80f, 16.2f });
		models["target2"]->SetAngle({ 0.0f, -1.0f, 0.0f });
		models.emplace("target3", std::make_unique<ModelObject>("Data/Model/Object/CloseTarget2.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR));
		models["target3"]->SetPosition({ -32.0, 1.80f, 23.4f });
		models["target3"]->SetAngle({ 0.0f, -1.0f, 0.0f });

		sky = std::make_unique<ModelObject>("Data/Model/Cube/Cube.fbx", 250.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR);
		sky->SetShader("Cube", ModelShaderDX12Id::Skydome);
		m_sprites[1] = std::make_unique<SpriteDX12>(1, L"Data/Model/Stage/skybox.dds");

		// パーティクル
		//DirectX::XMFLOAT3 p_pos = { 0,3,0 };
		//m_particle[0] = std::make_unique<HitParticleRenderer>(p_pos);
		//p_pos = { 3,3,0 };
		//m_particle[1] = std::make_unique<HitParticleRenderer>(p_pos);

		runningDust1 = std::make_unique<RunningDustDX12>("Data/Sprites/smoke.png", 100.0f,
			DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),	// position
			1.0f,			// alpha
			f_count,	// model_id
			0);		// age
	}

	// 光
	LightManager::Instance().SetAmbientColor({ 0.3f, 0.3f, 0.3f, 0.0f });
	Light* dl = new Light(LightType::Directional);
	dl->SetDirection({ 0.0f, -0.503f, -0.864f });
	LightManager::Instance().Register(dl);

	// プレイヤー
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
	player->SetPosition({ 15.0f, 15.0f, 5.0f });

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
		player->GetPosition(),	// 注視点
		{ 0, 0.969f, -0.248f }	// 上ベクトル
	);

	cameraController = std::make_unique<ThridPersonCameraController>();
	cameraController->SyncCameraToController(mainCamera);
	cameraController->SetEnable(true);
	cameraController->SetPlayer(player);
	CURSOR_OFF;

	Sound::Instance().Finalize();
	Sound::Instance().InitAudio();
	Sound::Instance().LoadAudio("Data/Sound/3-Dreamland(Overworld).mp3");
	Sound::Instance().PlayAudio(0);

	// ダンジョンの生成設定の初期化
	// 現在の階などを先にリセットしておく
	DUNGEONDATA.InitDungeonGenerateSetting();

	// 影初期化
	T_GRAPHICS.GetShadowRenderer()->Init(T_GRAPHICS.GetDeviceDX12());
}

void StageOpenWorld_E4C::Finalize()
{
	T_GRAPHICS.GetShadowRenderer()->Finalize();
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

	sky->Update(elapsedTime);

	teleporter->Update(elapsedTime);

	ENEMIES.Update(elapsedTime);

	SpawnerManager::Instance().Update(elapsedTime);

	PlayerCharacterManager::Instance().Update(elapsedTime);

	for (auto& it : models)
	{
		it.second->Update(elapsedTime);
		if (it.first == "bush" || it.first == "flower") continue;
		T_GRAPHICS.GetShadowRenderer()->ModelRegister(it.second->GetModel().get());
	}

	for (auto& model : PlayerCharacterManager::Instance().GetPlayerCharacterById()->GetModels())
	{
		T_GRAPHICS.GetShadowRenderer()->ModelRegister(model.get());
	}

	m_sceneTickTimer = elapsedTime;
	m_sceneGlobalTimer += elapsedTime;
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

	rc.timerGlobal = m_sceneGlobalTimer;
	rc.timerTick = TentacleLib::Timer::Instance().Delta();

	// ライトの情報を詰め込む
	LightManager::Instance().PushRenderContext(rc);

	for (auto& it : models)
	{
		it.second->Render(rc);
	}

	SpawnerManager::Instance().Render(rc);

	teleporter->Render(rc);

	ENEMIES.Render(rc);

	UI.Render(rc);

	// 描画
	PlayerCharacterManager::Instance().Render(rc);
	// デバッグレンダラ描画実行

	T_GRAPHICS.GetDebugRenderer()->Render(T_GRAPHICS.GetDeviceContext(), CameraManager::Instance().GetCamera()->GetView(), CameraManager::Instance().GetCamera()->GetProjection());
}

void StageOpenWorld_E4C::RenderDX12()
{
	RenderContextDX12 rc;

	T_GRAPHICS.BeginRender();

	// 3Dモデル描画
	{
		m_frameBuffer->WaitUntilToPossibleSetRenderTarget(T_GRAPHICS.GetFrameBufferDX12(FrameBufferDX12Id::Scene));
		m_frameBuffer->SetRenderTarget(T_GRAPHICS.GetFrameBufferDX12(FrameBufferDX12Id::Scene));
		m_frameBuffer->Clear(T_GRAPHICS.GetFrameBufferDX12(FrameBufferDX12Id::Scene));

		// シャドウマップ
		{
			T_GRAPHICS.GetShadowRenderer()->Render(m_frameBuffer);
			rc.shadowMap.shadow_srv_descriptor = T_GRAPHICS.GetShadowRenderer()->GetShadowSRV();
			rc.shadowMap.shadow_sampler_descriptor = T_GRAPHICS.GetShadowRenderer()->GetShadowSampler();
		}
		// シーン用定数バッファ更新
		const Descriptor* scene_cbv_descriptor = T_GRAPHICS.UpdateSceneConstantBuffer(
			CameraManager::Instance().GetCamera(), m_sceneGlobalTimer, m_sceneTickTimer);

		// レンダーコンテキスト設定
		rc.d3d_command_list = m_frameBuffer->GetCommandList();
		rc.scene_cbv_descriptor = scene_cbv_descriptor;

		// skyBox
		{
			rc.skydomeData.skyTexture = m_sprites[1]->GetDescriptor();
			sky->RenderDX12(rc);
		}

		// ステージ
		for (auto& it : models)
		{
			it.second->RenderDX12(rc);
		}

		teleporter->RenderDX12(rc);

		ENEMIES.RenderDX12(rc);

		SpawnerManager::Instance().RenderDX12(rc);

		// プレイヤー
		PlayerCharacterManager::Instance().RenderDX12(rc);

		// パーティクル
		//m_particle[0]->Render(m_frameBuffer);
		//m_particle[1]->Render(m_frameBuffer);

		// レンダーターゲットへの書き込み終了待ち
		m_frameBuffer->WaitUntilFinishDrawingToRenderTarget(T_GRAPHICS.GetFrameBufferDX12(FrameBufferDX12Id::Scene));
	}

	// ポストエフェクト描画
	{
		postprocessingRenderer->Render(m_frameBuffer);
	}

	// 2D描画
	{
		T_TEXT.BeginDX12();

		UI.RenderDX12(rc);

		T_TEXT.EndDX12();
	}
	DrawSceneGUI();
	T_GRAPHICS.GetImGUIRenderer()->RenderDX12(m_frameBuffer->GetCommandList());

	T_GRAPHICS.End();
}

void StageOpenWorld_E4C::DrawSceneGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->Pos;
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
}
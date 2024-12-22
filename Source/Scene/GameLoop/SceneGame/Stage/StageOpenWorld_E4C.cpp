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

#include "GameObject/Character/Player/PlayerCharacterManager.h"
#include "GameObject/Character/Enemy/EnemyManager.h"

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
	for (auto& filename : spriteList)
	{
		spritePreLoad.insert(RESOURCE.LoadSpriteResource(filename));
	}

	stage_collision = new MapTile("Data/Model/Stage/Terrain_Collision.glb", 0.01f);
	stage_collision->Update(0);
	stage_collision->SetCollider(Collider::COLLIDER_TYPE::MAP, Collider::COLLIDER_OBJ::OBSTRUCTION);

	MAPTILES.Register(stage_collision);
	MAPTILES.CreateSpatialIndex(5, 7);

	if (T_GRAPHICS.isDX11Active)
	{
		models.emplace("map", std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Map.glb", 1.0f, ModelObject::RENDER_MODE::DX11, ModelObject::MODEL_TYPE::LHS_TOON));
		models.emplace("tower", std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Tower.glb", 1.0f, ModelObject::RENDER_MODE::DX11, ModelObject::MODEL_TYPE::LHS_TOON));
		models.emplace("boss", std::make_unique<ModelObject>("Data/Model/Enemy/MDLANM_ENMboss_1205.glb", 1.0f, ModelObject::RENDER_MODE::DX11, ModelObject::MODEL_TYPE::LHS_TOON));
		models["boss"]->SetPosition({ 10.0, 0.0f, 10.0f });
		models["boss"]->SetAnimation(0, true);

		sky = std::make_unique<ModelObject>("Data/Model/Cube/Cube.fbx", 250.0f, ModelObject::RENDER_MODE::DX11);
		m_sprites[1] = std::make_unique<SpriteDX12>(1, L"Data/Model/Stage/skybox.dds");
	}

	if (T_GRAPHICS.isDX12Active)
	{
		models.emplace("map", std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Map.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR));
		models.emplace("tower", std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Tower.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR));
		models.emplace("boss", std::make_unique<ModelObject>("Data/Model/Enemy/MDLANM_ENMboss_1205.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_TOON));
		models["boss"]->SetPosition({ 10.0, 0.0f, 10.0f });
		models["boss"]->SetAnimation(0, true);

		sky = std::make_unique<ModelObject>("Data/Model/Cube/Cube.fbx", 250.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR);
		sky->SetShader("Cube", ModelShaderDX12Id::Skydome);
		m_sprites[1] = std::make_unique<SpriteDX12>(1, L"Data/Model/Stage/skybox.dds");
	}

	teleporter = std::make_unique<Teleporter>(new StageDungeon_E4C(m_pScene), m_pScene->GetOnlineController());
	teleporter->SetPosition({ 16, 8.5, -46 });
	teleporter->SetScale({ 5.0f, 10.0f, 1.0f });

	portalSquare = std::make_unique<Plane>(T_GRAPHICS.GetDevice(), "", 1.0f, XMFLOAT3{ 10.0f,10.0f,5.0f }, 5.0f, 5.0f);
	portalSquare->SetShader(ModelShaderId::PortalSquare);

	// 光
	LightManager::Instance().SetAmbientColor({ 0.3f, 0.3f, 0.3f, 0.0f });
	Light* dl = new Light(LightType::Directional);
	dl->SetDirection({ 0.0f, -0.503f, -0.864f });
	//dl->SetPosition({ 0, 20, 0 });
	LightManager::Instance().Register(dl);

	// プレイヤー
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
	player->SetPosition({ 15.0f, 15.0f, 5.0f });
	// プレイヤーが走るときの土埃
	runningDust1 = std::make_unique<RunningDust>(T_GRAPHICS.GetDevice(), "Data/Sprites/smoke.png", 100.0f,
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),	// position
		1.0f,			// alpha
		f_count,	// model_id
		0);		// age
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
		player->GetPosition(),	// 注視点
		{ 0, 0.969f, -0.248f }	// 上ベクトル
	);
	spawner = std::make_unique<Spawner>(0, 2, -1);
	spawner->SetPosition({ 0.0f, 5.0f, -5.0f });
	spawner->SetSpawnRadius(0.0f);

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

	ENEMIES.Update(elapsedTime);

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

	COLLISIONS.Contacts();

	for (auto& it : models)
	{
		it.second->Update(elapsedTime);
	}

	sky->Update(elapsedTime);

	spawner->Update(elapsedTime);

	teleporter->Update(elapsedTime);

	portalSquare->Update(elapsedTime);
	runningDust1->Update(elapsedTime);

	timer += elapsedTime;

	for (auto& it : models)
	{
		T_GRAPHICS.GetShadowRenderer()->ModelRegister(it.second->GetModel().get());
	}

	for (auto& model : PlayerCharacterManager::Instance().GetPlayerCharacterById()->GetModels())
	{
		T_GRAPHICS.GetShadowRenderer()->ModelRegister(model.get());
	}
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

	for (auto& it : models)
	{
		it.second->Render(rc);
	}

	teleporter->Render(rc);

	ENEMIES.Render(rc);

	portalSquare->Render(rc);
	runningDust1->Render(rc);

	UI.Render(rc);

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

		// シャドウマップ
		{
			T_GRAPHICS.GetShadowRenderer()->Render(m_frameBuffer);
			rc.shadowMap.shadow_srv_descriptor = T_GRAPHICS.GetShadowRenderer()->GetShadowSRV();
			rc.shadowMap.shadow_sampler_descriptor = T_GRAPHICS.GetShadowRenderer()->GetShadowSampler();
		}

		// プレイヤー
		PlayerCharacterManager::Instance().RenderDX12(rc);

		ENEMIES.RenderDX12(rc);

		// ステージ
		for (auto& it : models)
		{
			it.second->RenderDX12(rc);
		}

		// スポナー
		spawner->RenderDX12(rc);

		// skyBox
		{
			rc.skydomeData.skyTexture = m_sprites[1]->GetDescriptor();
			sky->RenderDX12(rc);
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
		T_TEXT.BeginDX12();

		UI.RenderDX12(rc);

		T_TEXT.EndDX12();
	}

	T_GRAPHICS.End();
}

void StageOpenWorld_E4C::OnPhase()
{
}
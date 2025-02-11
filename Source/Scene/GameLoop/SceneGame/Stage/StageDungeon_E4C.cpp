//! @file StageDungeon_E4C.cpp
//! @note

#include "StageDungeon_E4C.h"
#include "StageOpenWorld_E4C.h"

#include "Map/MapTileManager.h"
#include "Map/MapTile.h"
#include "Map/DungeonData.h"
#include "Map/InstancingModelManager.h"

#include "TAKOEngine/GUI/UIManager.h"
#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"
#include "TAKOEngine/Tool/GLTFImporter.h"
#include "TAKOEngine/Tool/Timer.h"
#include "TAKOEngine/Sound/Sound.h"

#include "GameObject/GameObjectManager.h"
#include "GameObject/ModelObject.h"
#include "Scene/Stage/StageManager.h"
#include "Scene/Stage/Stage.h"

#include "GameObject/Character/Player/PlayerCharacterManager.h"
#include "GameObject/Character/Enemy/EnemyManager.h"
#include "GameObject/Projectile/ProjectileManager.h"
#include "GameObject/Props/Zone/ZoneManager.h"
#include "GameObject/Props/Spawner.h"
#include "GameObject/Props/SpawnerManager.h"
#include "GameObject/Props/Teleporter.h"

#include "Network/OnlineController.h"

#include "Scene/GameLoop/SceneGame/SceneGame_E4C.h"

#include "PreloadManager.h"

static float timer = 0;

void StageDungeon_E4C::GenerateDungeon()
{
	DungeonData& dungeonData = DungeonData::Instance();

	// 生成順番配列の中身がないなら自動生成を行う
	if (m_roomOrder.size() == 0)
	{
		// 自動生成はリーダーのみ行う
		isLeader = true;

		// ダンジョンの自動生成を行う
		// 生成する部屋タイプを算出
		//RoomType firstRoomType = RoomType::FIRST_START;
		RoomType firstRoomType = DUNGEONDATA.GetCurrentFloorGenSetting().startRoomType;

		std::vector<RoomType> placeableRooms;
		placeableRooms.emplace_back(firstRoomType);

		// 部屋候補が一つしかない場合
		if (placeableRooms.size() == 1) firstRoomType = placeableRooms.at(0);
		// 複数候補があるならランダムで一つ
		else
		{
			// 生成可能な部屋の重みの合計
			int totalWeight = 0;
			for (RoomType type : placeableRooms)
			{
				totalWeight += dungeonData.GetRoomGenSetting(type).weight;
			}

			int randomValue = std::rand() % totalWeight;
			for (RoomType type : placeableRooms)
			{
				randomValue -= dungeonData.GetRoomGenSetting(type).weight;

				if (randomValue < 0) firstRoomType = type;
			}
		}

		bool isLastRoomGenerated = false;

		// 部屋の生成を開始する（再帰）
		rootRoom = std::make_unique<RoomBase>(
			nullptr, -1,
			firstRoomType,
			m_roomAABBs,
			isLastRoomGenerated);

		// 生成順番に登録する
		for (RoomBase* room : rootRoom->GetAll())
		{
			m_roomOrder.emplace_back(room->GetRoomType());
		}
	}
	// 中身があるならそれに沿って生成を行う
	else
	{
		// リーダーではない
		isLeader = false;

		int orderIndex = 1;

		rootRoom = std::make_unique<RoomBase>(
			nullptr, -1,
			(RoomType)(m_roomOrder.at(0)),
			m_roomAABBs,
			m_roomOrder, orderIndex);
	}
}

void StageDungeon_E4C::Initialize()
{
	Stage::Initialize(); // デフォルト

	PRELOAD.Join("DungeonModels");
	PRELOAD.Lock();

	// フレームバッファマネージャー
	m_frameBuffer = T_GRAPHICS.GetFrameBufferManager();

	// 光
	LightManager::Instance().SetAmbientColor({ 0.3f, 0.3f, 0.3f, 0.0f });
	Light* dl = new Light(LightType::Directional);
	dl->SetDirection({ 0.0f, -0.503f, -0.864f });
	LightManager::Instance().Register(dl);

	// プレイヤー
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
	player->SetPosition({ 0.0f, 2.0f, 2.0f });
	player->SetAngle({ 0.0f, 0.0f, 0.0f });
	player->GetStateMachine()->ChangeState(PlayerCharacter::STATE::IDLE);
	player->UpdateTransform();

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

	// 現在の階取得
	currentFloor = DUNGEONDATA.GetCurrentFloor();

	// テキスト設定
	floorText = std::make_unique<WidgetText>();
	floorText->SetText(("現在の階：" + std::to_string(currentFloor) + "階").c_str());
	floorText->SetBorderColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	floorText->SetBorder(2);
	floorText->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	floorText->SetPosition({ 30.0f, 30.0f });

	GenerateDungeon();

	// 部屋のモデルを配置
	for (RoomBase* room : rootRoom->GetAll())
	{
		room->PlaceMapTile(isLeader);

		// currentFloorが最大階でない場合は階段の行先はStageDungeon
		if (currentFloor < DUNGEONDATA.GetDungeonGenSetting().maxFloor)
		{
			if (room->GetRoomType() == DUNGEONDATA.GetCurrentFloorGenSetting().endRoomType)
			{
				room->PlaceTeleporterTile(new StageDungeon_E4C(m_pScene), m_pScene->GetOnlineController());
			}
		}
		// 最大階（ボスフロア）はボス撃破関数で階段を配置する
	}

	// 部屋の当たり判定を設定
	MAPTILES.CreateSpatialIndex(5, 7);

	PRELOAD.Unlock();
	Sound::Instance().InitAudio();
	Sound::Instance().LoadAudio("Data/Sound/5-Miraculous_Maze(Dungeon).mp3");
	Sound::Instance().PlayAudio(0);

	m_pPauseMenu = new WidgetPauseMenu(cameraController.get());
	UI.Register(m_pPauseMenu);

	// 影初期化
	T_GRAPHICS.GetShadowRenderer()->Init(T_GRAPHICS.GetDeviceDX12());
}

void StageDungeon_E4C::Finalize()
{
	ENEMIES.Clear();
	MAPTILES.Clear();
	PROJECTILES.Clear();
	//UI.Clear();
	SpawnerManager::Instance().Clear();
	GameObjectManager::Instance().Clear();
	ZoneManager::Instance().Clear();
	Sound::Instance().StopAudio(0);
	Sound::Instance().Finalize();

	T_GRAPHICS.GetShadowRenderer()->Finalize();
}

void StageDungeon_E4C::Update(float elapsedTime)
{
	// プレイヤーのYが-30.0fより下なら初期位置付近に戻す
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
	if (player->GetPosition().y < -30.0f)
	{
		player->SetPosition({ 0.0f, 1.0f, 1.0f });
	}

	// ゲームループ内で
	cameraController->SyncContrllerToCamera(CameraManager::Instance().GetCamera());
	cameraController->Update(elapsedTime);
	Online::OnlineController* onlineController = m_pScene->GetOnlineController();
	if (onlineController->GetState() == Online::OnlineController::STATE::LOGINED)
	{
		onlineController->RoomIn();
		onlineController->BeginSync();
	}

	if (T_INPUT.KeyDown(VK_MENU) || T_INPUT.GamePadKeyDown(GAME_PAD_BTN::BACK))
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
	if (!TentacleLib::isFocus() || m_pPauseMenu->IsActive())
	{
		cameraController->SetEnable(false);
		CURSOR_ON;
	}
	if (cameraController->isEnable())
	{
		T_INPUT.KeepCursorCenter();
	}

	ENEMIES.Update(elapsedTime);
	GameObjectManager::Instance().Update(elapsedTime);
	SpawnerManager::Instance().Update(elapsedTime);

	// 部屋
	for (RoomBase* room : rootRoom->GetAll()) room->Update(elapsedTime);

	PROJECTILES.Update(elapsedTime);

	PlayerCharacterManager::Instance().Update(elapsedTime);
	ZoneManager::Instance().Update(elapsedTime);

	MAPTILES.Update(elapsedTime);

	// テキスト
	floorText->Update(elapsedTime);

	// なんかUIアップデートせんとあかんっぽい(01/27)
	//UI.Update(elapsedTime);

	// キャラクターの影登録
	/*for (auto& model : PlayerCharacterManager::Instance().GetPlayerCharacterById()->GetModels())
	{
		T_GRAPHICS.GetShadowRenderer()->ModelRegister(model.get());
	}

	for (auto& model : PlayerCharacterManager::Instance().GetPlayerCharacterById()->GetModels())
	{
		T_GRAPHICS.GetShadowRenderer()->ModelRegister(model.get());
	}*/

	m_timer += elapsedTime;

	m_sceneTickTimer = elapsedTime;
	m_sceneGlobalTimer += elapsedTime;
}

void StageDungeon_E4C::DefeatBoss()
{
	// 部屋のモデルを配置
	for (RoomBase* room : rootRoom->GetAll())
	{
		if (room->GetRoomType() == DUNGEONDATA.GetCurrentFloorGenSetting().endRoomType)
		{
			room->PlaceTeleporterTile(new StageOpenWorld_E4C(m_pScene), m_pScene->GetOnlineController());
		}
	}
}

void StageDungeon_E4C::Render()
{
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->Clear(T_GRAPHICS.GetDeviceContext(), 0.2f, 0.2f, 0.2f, 1);
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->SetRenderTarget(T_GRAPHICS.GetDeviceContext());

	// 描画コンテキスト設定
	RenderContext rc;
	rc.camera = CameraManager::Instance().GetCamera();
	rc.deviceContext = T_GRAPHICS.GetDeviceContext();
	rc.renderState = T_GRAPHICS.GetRenderState();

	rc.timerGlobal = m_timer;
	rc.timerTick = TentacleLib::Timer::Instance().Delta();

	// ライトの情報を詰め込む
	LightManager::Instance().PushRenderContext(rc);

	// 描画
	GameObjectManager::Instance().Render(rc);
	PlayerCharacterManager::Instance().Render(rc);
	ZoneManager::Instance().Render(rc);

	MAPTILES.Render(rc);

#ifdef _DEBUG
	// デバッグレンダラ描画実行
	T_GRAPHICS.GetDebugRenderer()->Render(T_GRAPHICS.GetDeviceContext(), CameraManager::Instance().GetCamera()->GetView(), CameraManager::Instance().GetCamera()->GetProjection());
	rootRoom->DrawDebugGUI();
#endif // _DEBUG
}

void StageDungeon_E4C::RenderDX12()
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
			CameraManager::Instance().GetCamera(), m_sceneGlobalTimer, m_sceneTickTimer, rc);

		// レンダーコンテキスト設定
		rc.d3d_command_list = m_frameBuffer->GetCommandList();
		rc.scene_cbv_descriptor = scene_cbv_descriptor;

		// MapTile
		MAPTILES.RenderDX12(rc);

		ENEMIES.RenderDX12(rc);

		SpawnerManager::Instance().RenderDX12(rc);

		PROJECTILES.RenderDX12(rc);

		GameObjectManager::Instance().RenderDX12(rc);
		// プレイヤー
		PlayerCharacterManager::Instance().RenderDX12(rc);
		ZoneManager::Instance().RenderDX12(rc);

		// 部屋（OneWayWall、DebugCube等）
		for (RoomBase* room : rootRoom->GetAll()) room->RenderDX12(rc);

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

		floorText->RenderDX12(rc);
		UI.RenderDX12(rc);

		T_TEXT.EndDX12();
	}

	// デバッグ
	{
#ifdef _DEBUG
		DrawSceneGUI();
#endif // _DEBUG
	}

	T_GRAPHICS.GetImGUIRenderer()->RenderDX12(m_frameBuffer->GetCommandList());

	T_GRAPHICS.End();
}

void StageDungeon_E4C::DrawSceneGUI()
{
	if (ImGui::Begin("StageDungeon_E4C"))
	{
		// 現在の階
		ImGui::Text(("現在の階：" + std::to_string(currentFloor)).c_str());

		// 生成配列のテキスト
		std::string orderText = "生成配列：";
		for (int i = 0; i < m_roomOrder.size(); i++)
		{
			orderText += std::to_string(m_roomOrder.at(i));
			if (i < m_roomOrder.size() - 1) orderText += ",";
		}
		ImGui::Text(orderText.c_str());

		ImGui::SeparatorText("ダンジョン全体の生成設定");
		ImGui::Text(("最大階数：" + std::to_string(DUNGEONDATA.GetDungeonGenSetting().maxFloor)).c_str());

		ImGui::SeparatorText("現在の階の生成設定");
		ImGui::Text(("最大深度：" + std::to_string(DUNGEONDATA.GetCurrentFloorGenSetting().maxDepth)).c_str());
	}
	ImGui::End();
}
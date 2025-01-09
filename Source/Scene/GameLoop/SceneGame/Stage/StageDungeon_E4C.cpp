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

#include "GameObject/GameObjectManager.h"
#include "GameObject/ModelObject.h"
#include "Scene/Stage/StageManager.h"
#include "Scene/Stage/Stage.h"

#include "GameObject/Character/Player/PlayerCharacterManager.h"
#include "GameObject/Character/Enemy/EnemyManager.h"
#include "GameObject/Props/Spawner.h"
#include "GameObject/Props/Teleporter.h"

#include "Network/OnlineController.h"

#include "Scene/GameLoop/SceneGame/SceneGame_E4C.h"

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
		RoomType firstRoomType = RoomType::FIRST_START;

		std::vector<RoomType> placeableRooms;
		placeableRooms.emplace_back(RoomType::FIRST_START);

		// 部屋候補が一つしかない場合
		if (placeableRooms.size() == 1) firstRoomType = placeableRooms.at(0);
		// 複数候補があるならランダムで一つ
		else
		{
			// 生成可能な部屋の重みの合計
			int totalWeight = 0;
			for (RoomType type : placeableRooms)
			{
				totalWeight += dungeonData.GetRoomGenerateSetting(type).weight;
			}

			int randomValue = std::rand() % totalWeight;
			for (RoomType type : placeableRooms)
			{
				randomValue -= dungeonData.GetRoomGenerateSetting(type).weight;

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

	// フレームバッファマネージャー
	m_frameBuffer = T_GRAPHICS.GetFrameBufferManager();

	// 光
	LightManager::Instance().SetAmbientColor({ 0.3f, 0.3f, 0.3f, 0.0f });
	Light* dl = new Light(LightType::Directional);
	dl->SetDirection({ 0.0f, -0.503f, -0.864f });
	dl->SetPosition({ 0, 20, 0 });
	LightManager::Instance().Register(dl);

	// プレイヤー
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
	player->SetPosition({ 0.0f, 5.0f, 2.0f });
	player->GetStateMachine()->ChangeState(PlayerCharacter::STATE::IDLE);

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

	//m_roomOrder.emplace_back(RoomType::TUTO_START);
	//m_roomOrder.emplace_back(RoomType::TUTO_NOTHINGROOM);
	//m_roomOrder.emplace_back(RoomType::TUTO_SPAWNERROOM);
	//m_roomOrder.emplace_back(RoomType::TUTO_NOTHINGROOM);
	//m_roomOrder.emplace_back(RoomType::TUTO_END);

	GenerateDungeon();

	// 一番遠い部屋のうち、ランダムな一つを抽選しテレポーターを設置する
	//RoomBase* lastRoom = rootRoom->GetFarthestChild().at(std::rand() % rootRoom->GetFarthestChild().size());

	//TeleportToOpenworld* teleporter = new TeleportToOpenworld();
	//teleporter->SetPosition(lastRoom->GetPosition() + DUNGEONDATA.GetRoomGenerateSetting(lastRoom->GetRoomType()).portalPosition);
	//teleporter->SetAngle({ 90.0f * RADIAN1, 0.0f, 0.0f });
	//teleporter->SetScale({ 10.0f, 10.0f, 1.0f });
	//GameObjectManager::Instance().Register(teleporter);

	// 部屋のモデルを配置
	for (RoomBase* room : rootRoom->GetAll())
	{
		room->PlaceMapTile(isLeader);
	}

	// 部屋の当たり判定を設定
	MAPTILES.CreateSpatialIndex(5, 7);
}

void StageDungeon_E4C::Finalize()
{
	ENEMIES.Clear();
	MAPTILES.Clear();

	GameObjectManager::Instance().Clear();
}

void StageDungeon_E4C::Update(float elapsedTime)
{
	Camera* camera = CameraManager::Instance().GetCamera();
	Online::OnlineController* onlineController = m_pScene->GetOnlineController();
	if (onlineController->GetState() == Online::OnlineController::STATE::LOGINED)
	{
		onlineController->RoomIn();
		onlineController->BeginSync();
	}

	// プレイヤーのYが-30.0fより下なら初期位置付近に戻す
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
	if (player->GetPosition().y < -30.0f)
	{
		player->SetPosition({ 0.0f, 1.0f, 1.0f });
	}

	// ゲームループ内で
	cameraController->SyncContrllerToCamera(camera);
	cameraController->Update(elapsedTime);

	// 部屋を全てアップデート
	rootRoom->Update(elapsedTime);

	PlayerCharacterManager::Instance().Update(elapsedTime);
	GameObjectManager::Instance().Update(elapsedTime);
	ENEMIES.Update(elapsedTime);
	MAPTILES.Update(elapsedTime);

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

	for (auto& model : PlayerCharacterManager::Instance().GetPlayerCharacterById()->GetModels())
	{
		T_GRAPHICS.GetShadowRenderer()->ModelRegister(model.get());
	}

	for (auto& model : PlayerCharacterManager::Instance().GetPlayerCharacterById()->GetModels())
	{
		T_GRAPHICS.GetShadowRenderer()->ModelRegister(model.get());
	}

	timer += elapsedTime;
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

	rc.timerGlobal = timer;
	rc.timerTick = TentacleLib::Timer::Instance().Delta();

	// ライトの情報を詰め込む
	LightManager::Instance().PushRenderContext(rc);

	// 描画
	PlayerCharacterManager::Instance().Render(rc);

	GameObjectManager::Instance().Render(rc);

	MAPTILES.Render(rc);

#ifdef _DEBUG
	// デバッグレンダラ描画実行
	T_GRAPHICS.GetDebugRenderer()->Render(T_GRAPHICS.GetDeviceContext(), CameraManager::Instance().GetCamera()->GetView(), CameraManager::Instance().GetCamera()->GetProjection());
	rootRoom->DrawDebugGUI();
#endif // _DEBUG
}

void StageDungeon_E4C::RenderDX12()
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
			// TODO: 影
			//T_GRAPHICS.GetShadowRenderer()->Render(m_frameBuffer);
			rc.shadowMap.shadow_srv_descriptor = T_GRAPHICS.GetShadowRenderer()->GetShadowSRV();
			rc.shadowMap.shadow_sampler_descriptor = T_GRAPHICS.GetShadowRenderer()->GetShadowSampler();
		}

		// モデル描画
		PlayerCharacterManager::Instance().RenderDX12(rc);
		GameObjectManager::Instance().RenderDX12(rc);
		ENEMIES.RenderDX12(rc);
		MAPTILES.RenderDX12(rc);

		for (RoomBase* room : rootRoom->GetAll())
		{
			room->Render(rc);

			DirectX::XMFLOAT3 p = { 0, 0, 0 };
			DirectX::XMFLOAT3 s = { 5, 5, 5 };

			//T_GRAPHICS.GetDebugRenderer()->DrawCube(p, s, {1.0f, 1.0f, 1.0f, 1.0f});
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

void StageDungeon_E4C::OnPhase()
{
}
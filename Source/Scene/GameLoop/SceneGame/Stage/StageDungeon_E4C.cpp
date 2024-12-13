#include "StageDungeon_E4C.h"
#include "StageOpenWorld_E4C.h"

#include "TAKOEngine/GUI/UIManager.h"

#include "GameObject/GameObjectManager.h"

#include "GameObject/ModelObject.h"
#include "Scene/Stage/StageManager.h"
#include "Scene/Stage/Stage.h"

#include "Map/MapTileManager.h"
#include "Map/MapTile.h"
#include "Map/DungeonData.h"
#include "Map/InstancingModelManager.h"

#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"
#include "TAKOEngine/Tool/GLTFImporter.h"
#include "TAKOEngine/Tool/Timer.h"

#include "GameObject/Character/Player/PlayerCharacterManager.h"

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
		std::vector<RoomType> placeableRooms;
		//placeableRooms.emplace_back(DungeonData::SIMPLE_ROOM_1);
		placeableRooms.emplace_back(RoomType::CROSS_ROOM_1);

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

			if (randomValue < 0)
			{
				int orderIndex = 0;

				switch (type)
				{
				case RoomType::SIMPLE_ROOM_1:
					rootRoom = std::make_unique<SimpleRoom1>(
						nullptr, -1,
						m_roomAABBs,
						true,
						m_roomOrder, orderIndex);
					break;

				case RoomType::END_ROOM:
					rootRoom = std::make_unique<EndRoom1>(
						nullptr, -1,
						m_roomAABBs,
						true,
						m_roomOrder, orderIndex);
					break;

				case RoomType::CROSS_ROOM_1:
					rootRoom = std::make_unique<CrossRoom1>(
						nullptr, -1,
						m_roomAABBs,
						true,
						m_roomOrder, orderIndex);
					break;

				case RoomType::CROSS_ROOM_2:
					rootRoom = std::make_unique<CrossRoom2>(
						nullptr, -1,
						m_roomAABBs,
						true,
						m_roomOrder, orderIndex);
					break;

				case RoomType::PASSAGE_1:
					rootRoom = std::make_unique<Passage1>(
						nullptr, -1,
						m_roomAABBs,
						true,
						m_roomOrder, orderIndex);
					break;

				case RoomType::DEAD_END:
					rootRoom = std::make_unique<DeadEndRoom>(
						nullptr, -1,
						m_roomAABBs,
						true,
						m_roomOrder, orderIndex);
					break;
				}
			}
		}
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

		switch (m_roomOrder.front())
		{
		case RoomType::SIMPLE_ROOM_1:
			rootRoom = std::make_unique<SimpleRoom1>(
				nullptr, -1,
				m_roomAABBs,
				false,
				m_roomOrder, orderIndex);
			break;

		case RoomType::END_ROOM:
			rootRoom = std::make_unique<EndRoom1>(
				nullptr, -1,
				m_roomAABBs,
				false,
				m_roomOrder, orderIndex);
			break;

		case RoomType::CROSS_ROOM_1:
			rootRoom = std::make_unique<CrossRoom1>(
				nullptr, -1,
				m_roomAABBs,
				false,
				m_roomOrder, orderIndex);
			break;

		case RoomType::CROSS_ROOM_2:
			rootRoom = std::make_unique<CrossRoom2>(
				nullptr, -1,
				m_roomAABBs,
				false,
				m_roomOrder, orderIndex);
			break;

		case RoomType::PASSAGE_1:
			rootRoom = std::make_unique<Passage1>(
				nullptr, -1,
				m_roomAABBs,
				false,
				m_roomOrder, orderIndex);
			break;

		case RoomType::DEAD_END:
			rootRoom = std::make_unique<DeadEndRoom>(
				nullptr, -1,
				m_roomAABBs,
				false,
				m_roomOrder, orderIndex);
			break;
		}
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
	player->SetPosition({ 0.0f, 5.0f, 0.0f });
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

	m_roomOrder.emplace_back(0);

	GenerateDungeon();

	// 一番遠い部屋のうち、ランダムな一つを抽選しテレポーターを設置する
	RoomBase* lastRoom = rootRoom->GetFarthestChild().at(std::rand() % rootRoom->GetFarthestChild().size());

	TeleportToOpenworld* teleporter = new TeleportToOpenworld();
	teleporter->SetPosition(lastRoom->GetCenterPos());
	teleporter->SetAngle({ 90.0f * RADIAN1, 0.0f, 0.0f });
	teleporter->SetScale({ 10.0f, 10.0f, 1.0f });
	GameObjectManager::Instance().Register(teleporter);

	// 部屋のモデルを配置
	for (RoomBase* room : rootRoom->GetAll())
	{
		room->PlaceMapTile(isLeader);
	}

	// インスタンシングモデルテスト
	{
		//FILE_DATA fileData = DUNGEONDATA.GetModelFileDatas(TileType::WALL_01A).at(0);

		//std::filesystem::path filePath = fileData.fileName;
		//std::string fileNameStr = filePath.stem().string();
		//const char* fileName = fileNameStr.c_str();

		//ModelObject* instancingModel = new ModelObject(
		//	fileData.fileName.c_str(),
		//	fileData.scale,
		//	ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_TOON);
		//instancingModel->SetShader(fileName, ModelShaderDX12Id::ToonInstancing);

		//for (int i = 0; i < 4; i++)
		//{
		//	// 使われていないIDを取得して利用
		//	int id = instancingModel->GetModel()->AllocateInstancingIndex();
		//	if (id < 0) continue;

		//	DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
		//	DirectX::XMFLOAT3 angle = { DirectX::XMConvertToRadians(90.0f * i), DirectX::XMConvertToRadians(90.0f * i), DirectX::XMConvertToRadians(90.0f * i) };
		//	DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };

		//	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		//	DirectX::XMMATRIX R = AnglesToMatrix({ angle.x, angle.z, angle.y });
		//	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(-position.x * 0.25f, position.y * 0.25f, position.z * 0.25f);

		//	DirectX::XMMATRIX LocalTransform = S * R * T;

		//	DirectX::XMFLOAT4X4 tm;
		//	DirectX::XMStoreFloat4x4(&tm, LocalTransform);

		//	instancingModel->GetModel()->UpdateTransform(id, tm);
		//}
		//MAPTILES.Register(instancingModel);
	}

	//MapTile* stage_collision = new MapTile("Data/Model/Stage/Terrain_Collision.glb", 0.01f);
	//stage_collision->SetPosition({ 0.0f, -1.0f, 0.0f });
	//stage_collision->Update(0);
	//stage_collision->SetCollider(Collider::COLLIDER_TYPE::MAP, Collider::COLLIDER_OBJ::OBSTRUCTION);
	//MAPTILES.Register(stage_collision);

	// 部屋の当たり判定を設定
	MAPTILES.CreateSpatialIndex(5, 7);

	Console::Instance().Open();
}

void StageDungeon_E4C::Finalize()
{
	Console::Instance().Close();
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

	// ゲームループ内で
	cameraController->SyncContrllerToCamera(camera);
	cameraController->Update(elapsedTime);

	// 部屋を全てアップデート
	rootRoom->Update(elapsedTime);

	PlayerCharacterManager::Instance().Update(elapsedTime);
	GameObjectManager::Instance().Update(elapsedTime);
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
	
	DirectX::XMFLOAT3 pos = PlayerCharacterManager::Instance().GetPlayerCharacterById()->GetPosition();

	Console::Instance().Log(std::string("Player Position: " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z)).c_str());

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

		// モデル描画
		PlayerCharacterManager::Instance().RenderDX12(rc);
		//PlayerCharacterManager::Instance().GetPlayerCharacterById()->SetKinematic(true);

		GameObjectManager::Instance().RenderDX12(rc);

		MAPTILES.RenderDX12(rc);

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
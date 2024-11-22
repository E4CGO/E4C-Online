#include "StageDungeon_E4C.h"
#include "StageOpenWorld_E4C.h"

#include "GameObject/GameObjectManager.h"

#include "GameObject/ModelObject.h"
#include "Scene/Stage/StageManager.h"
#include "Scene/Stage/Stage.h"

#include "Map/MapTileManager.h"
#include "Map/MapTile.h"
#include "Map/DungeonData.h"

#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"
#include "TAKOEngine/Tool/GLTFImporter.h"
#include "TAKOEngine/Tool/Timer.h"

#include "GameObject/Character/Player/PlayerCharacterManager.h"

#include "Scene/Stage/StageManager.h"

#include "Scene/Stage/TestingStage.h"
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
		std::vector<DungeonData::RoomType> placeableRooms;
		//placeableRooms.emplace_back(DungeonData::SIMPLE_ROOM_1);
		placeableRooms.emplace_back(DungeonData::CROSS_ROOM_1);

		// 生成可能な部屋の重みの合計
		int totalWeight = 0;
		for (DungeonData::RoomType type : placeableRooms)
		{
			totalWeight += dungeonData.GetRoomGenerateSetting(type).weight;
		}

		int randomValue = std::rand() % totalWeight;
		for (DungeonData::RoomType type : placeableRooms)
		{
			randomValue -= dungeonData.GetRoomGenerateSetting(type).weight;

			if (randomValue < 0)
			{
				int orderIndex = 0;

				switch (type)
				{
				case DungeonData::SIMPLE_ROOM_1:
					rootRoom = std::make_unique<SimpleRoom1>(
						nullptr, -1,
						m_roomAABBs,
						true,
						m_roomOrder, orderIndex);
					break;

				case DungeonData::END_ROOM:
					rootRoom = std::make_unique<EndRoom1>(
						nullptr, -1,
						m_roomAABBs,
						true,
						m_roomOrder, orderIndex);
					break;

				case DungeonData::CROSS_ROOM_1:
					rootRoom = std::make_unique<CrossRoom1>(
						nullptr, -1,
						m_roomAABBs,
						true,
						m_roomOrder, orderIndex);
					break;

				case DungeonData::CROSS_ROOM_2:
					rootRoom = std::make_unique<CrossRoom2>(
						nullptr, -1,
						m_roomAABBs,
						true,
						m_roomOrder, orderIndex);
					break;

				case DungeonData::PASSAGE_1:
					rootRoom = std::make_unique<Passage1>(
						nullptr, -1,
						m_roomAABBs,
						true,
						m_roomOrder, orderIndex);
					break;

				case DungeonData::DEAD_END:
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
		case DungeonData::SIMPLE_ROOM_1:
			rootRoom = std::make_unique<SimpleRoom1>(
				nullptr, -1,
				m_roomAABBs,
				false,
				m_roomOrder, orderIndex);
			break;

		case DungeonData::END_ROOM:
			rootRoom = std::make_unique<EndRoom1>(
				nullptr, -1,
				m_roomAABBs,
				false,
				m_roomOrder, orderIndex);
			break;

		case DungeonData::CROSS_ROOM_1:
			rootRoom = std::make_unique<CrossRoom1>(
				nullptr, -1,
				m_roomAABBs,
				false,
				m_roomOrder, orderIndex);
			break;

		case DungeonData::CROSS_ROOM_2:
			rootRoom = std::make_unique<CrossRoom2>(
				nullptr, -1,
				m_roomAABBs,
				false,
				m_roomOrder, orderIndex);
			break;

		case DungeonData::PASSAGE_1:
			rootRoom = std::make_unique<Passage1>(
				nullptr, -1,
				m_roomAABBs,
				false,
				m_roomOrder, orderIndex);
			break;

		case DungeonData::DEAD_END:
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

	// 光
	LightManager::Instance().SetAmbientColor({ 0, 0, 0, 0 });
	Light* dl = new Light(LightType::Directional);
	dl->SetDirection({ 0.0f, -0.503f, -0.864f });
	LightManager::Instance().Register(dl);

	// プレイヤー
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
	player->SetPosition({ 5.0f, 5.0f, 5.0f });
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

	{
		HRESULT hr;

		D3D11_BUFFER_DESC buffer_desc{};
		buffer_desc.ByteWidth = (sizeof(CbScene) + 15) / 16 * 16;
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		hr = T_GRAPHICS.GetDevice()->CreateBuffer(&buffer_desc, nullptr, constant_buffers[1].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	GenerateDungeon();

	// 一番遠い部屋のうち、ランダムな一つを抽選しテレポーターを設置する
	RoomBase* lastRoom = rootRoom->GetFarthestChild().at(std::rand() % rootRoom->GetFarthestChild().size());

	TeleportToOpenworld* teleporter = new TeleportToOpenworld();
	teleporter->SetPosition(lastRoom->GetCenterPos());
	teleporter->SetAngle({ 90.0f * RADIAN1, 0.0f, 0.0f});
	teleporter->SetScale({ 10.0f, 10.0f, 1.0f });
	GameObjectManager::Instance().Register(teleporter);

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
	PlayerCharacterManager::Instance().Update(elapsedTime);

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

void StageDungeon_E4C::OnPhase()
{
}
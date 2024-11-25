#include "StageDungeon_E4C.h"

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

#include "Network/OnlineController.h"

#include "Scene/GameLoop/SceneGame/SceneGame_E4C.h"

static float timer = 0;

StageDungeon_E4C::StageDungeon_E4C(SceneGame_E4C* scene)
	: m_pScene(scene), Stage()
{
	std::vector<AABB> roomAABBs;

	// 乱数による部屋の自動生成を行う
	rootRoom = std::make_unique<SimpleRoom1>(nullptr, -1, roomAABBs);
}

StageDungeon_E4C::StageDungeon_E4C(SceneGame_E4C* scene, std::vector<int> roomTree)
	: m_pScene(scene), Stage()
{
	int treeIndex = 0;

	// 部屋配列に従って部屋を生成する
	rootRoom = std::make_unique<SimpleRoom1>(nullptr, -1, roomTree, treeIndex);
}

void StageDungeon_E4C::Initialize()
{
	Stage::Initialize(); // デフォルト

	// フレームバッファマネージャー
	m_frameBuffer = T_GRAPHICS.GetFrameBufferManager();

	{
		std::array<DirectX::XMFLOAT3, 4 > positions = {
		DirectX::XMFLOAT3{ 10.0f, 10.0f, 5.0f},
		DirectX::XMFLOAT3{ 10.0f, 20.0f, 5.0f },
		DirectX::XMFLOAT3{ 5.0f, 10.0f, 5.0f },
		DirectX::XMFLOAT3{ 5.0f, 20.0f, 5.0f }
		};

		plane = std::make_unique<Plane>(T_GRAPHICS.GetDevice(), "Data/Sprites/gem.png", 1.0f, positions);
	}

	{
		std::array<DirectX::XMFLOAT3, 4 >positions = {
			DirectX::XMFLOAT3{ 15.0f, 15.0f, 5.0f},
			DirectX::XMFLOAT3{ 15.0f, 25.0f, 5.0f },
			DirectX::XMFLOAT3{ 25.0f, 15.0f, 5.0f },
			DirectX::XMFLOAT3{ 25.0f, 25.0f, 5.0f }
		};

		portal = std::make_unique<Plane>(T_GRAPHICS.GetDevice(), "", 1.0f, positions);
		portal.get()->SetShader(ModelShaderId::Portal);
	}

	// 光
	LightManager::Instance().SetAmbientColor({ 0, 0, 0, 0 });
	Light* dl = new Light(LightType::Directional);
	dl->SetDirection({ 0.0f, -0.503f, -0.864f });
	LightManager::Instance().Register(dl);

	// プレイヤー
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
	player->SetPosition({ 5.0f, 5.0f, 5.0f });

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

	// 部屋のモデルを配置
	for (RoomBase* room : rootRoom->GetAll())
	{
		room->PlaceMapTile();
	}

	//test = std::make_unique<ModelDX12>("Data/Model/Enemy/Goblin.glb");
	//test->PlayAnimation(0, true);

	MAPTILES.CreateSpatialIndex(5, 7);
}

void StageDungeon_E4C::Update(float elapsedTime)
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

	// 部屋を全てアップデート
	rootRoom->Update(elapsedTime);

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
	plane->Update(elapsedTime);
	portal->Update(elapsedTime);

	timer += elapsedTime;

	//test->UpdateAnimation(elapsedTime);
	//test->UpdateTransform();
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

	MAPTILES.Render(rc);

	plane->Render(rc);

	portal->Render(rc);

	// デバッグレンダラ描画実行
	T_GRAPHICS.GetDebugRenderer()->Render(T_GRAPHICS.GetDeviceContext(), CameraManager::Instance().GetCamera()->GetView(), CameraManager::Instance().GetCamera()->GetProjection());

	rootRoom->DrawDebugGUI();
}

void StageDungeon_E4C::RenderDX12()
{
	//T_GRAPHICS.BeginRender();

	//// シーン用定数バッファ更新
	//const Descriptor* scene_cbv_descriptor = T_GRAPHICS.UpdateSceneConstantBuffer(
	//	CameraManager::Instance().GetCamera());

	//// レンダーコンテキスト設定
	//RenderContextDX12 rc;
	//rc.d3d_command_list = m_frameBuffer->GetCommandList();
	//rc.scene_cbv_descriptor = scene_cbv_descriptor;

	//// 3Dモデル描画
	//{
	//	m_frameBuffer->WaitUntilToPossibleSetRenderTarget(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Scene));
	//	m_frameBuffer->SetRenderTarget(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Scene));
	//	m_frameBuffer->Clear(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Scene));

	//	// モデル描画
	//	ModelShaderDX12* shader = T_GRAPHICS.GetModelShaderDX12(ModelShaderDX12Id::Toon);
	//	if (test != nullptr)
	//	{
	//		shader->Render(rc, test.get());
	//	}

	//	// レンダーターゲットへの書き込み終了待ち
	//	m_frameBuffer->WaitUntilFinishDrawingToRenderTarget(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Scene));
	//}

	//// ポストエフェクト描画
	//{
	//	postprocessingRenderer->Render(m_frameBuffer);
	//}

	//// 2D描画
	//{

	//}

	//T_GRAPHICS.End();
}

void StageDungeon_E4C::OnPhase()
{
}
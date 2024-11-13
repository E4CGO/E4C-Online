#include "StageOpenWorld_E4C.h"

#include "GameObject/ModelObject.h"
#include "GameObject/Props/Teleporter.h"
#include "Scene/Stage/StageManager.h"
#include "Scene/Stage/Stage.h"

#include "Map/MapTileManager.h"
#include "Map/MapTile.h"

#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"
#include "TAKOEngine/Tool/Timer.h"

#include "GameObject/Character/Player/PlayerCharacterManager.h"

#include "Scene/Stage/StageManager.h"

#include "Scene/Stage/TestingStage.h"

void StageOpenWorld_E4C::Initialize()
{
	Stage::Initialize(); // デフォルト

	stage_collision = new MapTile("Data/Model/Stage/Terrain_Collision.glb", 0.025f);
	stage_collision->Update(0);
	MAPTILES.Register(stage_collision);

	map = std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Map.glb", 2.5f, ModelObject::RENDER_MODE::DX11GLTF);

	const PlayerCharacterData::CharacterInfo info = PlayerCharacterData::Instance().GetCurrentCharacter();
	PlayerCharacter* player = PlayerCharacterManager::Instance().UpdatePlayerData(0, "", info.Character.pattern);
	player->SetPosition({ 5,	10, 5 });
	player->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::State::Idle));

	teleporter = std::make_unique<Teleporter>("Data/Model/Cube/testCubes.glb", 1.0);
	teleporter->SetPosition({ 50, 0, 60 });

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

	// カメラ設定
	Camera* mainCamera = CameraManager::Instance().GetCamera();
	mainCamera->SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),							// 画角
		T_GRAPHICS.GetScreenWidth() / T_GRAPHICS.GetScreenHeight(),	// 画面アスペクト比
		0.1f,														// ニアクリップ
		10000.0f													// ファークリップ
	);
	mainCamera->SetLookAt(
		{ 0, 5.0f, 10.0f },		// 視点
		{ 0, 0, 0 },			// 注視点
		{ 0, 0.969f, -0.248f }	// 上ベクトル
	);

	cameraController = std::make_unique<ThridPersonCameraController>();
	cameraController->SyncCameraToController(mainCamera);
	cameraController->SetEnable(true);
	cameraController->SetPlayer(player);
}

void StageOpenWorld_E4C::Update(float elapsedTime)
{
	Camera* camera = CameraManager::Instance().GetCamera();
	std::vector<DirectX::XMFLOAT3> cameraFocusPoints = {
		camera->GetFocus(),
		camera->GetFocus(),
		camera->GetFocus(),
		camera->GetFocus()
		//{CameraManager::Instance().GetCamera()->GetFocus().x, CameraManager::Instance().GetCamera()->GetFocus().y, CameraManager::Instance().GetCamera()->GetFocus().z}
	};
	// ゲームループ内で

	if (T_INPUT.KeyPress(VK_SHIFT))
	{
		CameraManager::Instance().GetCamera()->MovePointToCamera(cameraPositions, cameraFocusPoints, transitionTime, transitionDuration, elapsedTime);
	}
	else
	{
		cameraController->SyncContrllerToCamera(CameraManager::Instance().GetCamera());
		cameraController->Update(elapsedTime);
		CameraManager::Instance().GetCamera()->GetSegment() = 0;
		transitionTime = 0;
	}

	PlayerCharacterManager::Instance().Update(elapsedTime);
	teleporter->Update(elapsedTime);
	plane->Update(elapsedTime);
	portal->Update(elapsedTime);

	teleporter->CheckPlayer(PlayerCharacterManager::Instance().GetPlayerCharacterById(GAME_DATA.GetClientId())->GetPosition(), elapsedTime);

	if (teleporter->GetPortalReady()) STAGES.ChangeStage(new TestingStage);

	map->Update(elapsedTime);

	timer += elapsedTime;
}

void StageOpenWorld_E4C::Render()
{
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->Clear(T_GRAPHICS.GetDeviceContext(), 0.2f, 0.2f, 0.2f, 1);
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->SetRenderTarget(T_GRAPHICS.GetDeviceContext());
#ifdef _DEBUG
	{
		T_GRAPHICS.GetDebugRenderer()->DrawSphere(cameraPositions, 2, { 1,0,0,1 });
		T_GRAPHICS.GetDebugRenderer()->DrawSphere(CameraManager::Instance().GetCamera(0)->GetEye(), 2, { 1,1,0,1 });
	}
#endif // _DEBUG

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

	teleporter->Render(rc);
	plane->Render(rc);
	portal->Render(rc);

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
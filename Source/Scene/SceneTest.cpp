#include <profiler.h>
#include "Scene/SceneTest.h"
#include "Map/MapTileManager.h"

void SceneTest::Initialize()
{
	stage = new MapTile("Data/Model/Stage/Terrain_Collision.glb", 0.1);
	stage->Update(0);
	MAPTILES.Register(stage);

	PlayerCharacterData::CharacterInfo charInfo = {
		true,			// visible
		"",				// save
		{				//Character
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}
	};

	newPlayer = std::make_unique<PlayerCharacter>(charInfo);
	newPlayer->SetPosition({ 5,	100, 5 });
	newPlayer->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::State::Idle));

	//player = std::make_unique<Player>("Data/Model/Character/Barbarian.glb", 1.0f);
	//player->SetPosition({ 5, 50, 5 });
	//player->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));
	//knight = std::make_unique<ModelObject>("Data/Model/Character/Knight.glb");
	//knight->SetAnimation(22, true, 0.0f);
	//knight->SetPosition({ 1.08f, 0.0f, 2.12f });
	//knight->SetAngle({ 0.0f, -1.22f, 0.0f });
	//knight->GetModel()->FindNode("1H_Sword_Offhand")->visible = false;
	//knight->GetModel()->FindNode("2H_Sword")->visible = false;
	//knight->GetModel()->FindNode("Badge_Shield")->visible = false;
	//knight->GetModel()->FindNode("Round_Shield")->visible = false;
	//knight->GetModel()->FindNode("Spike_Shield")->visible = false;

	// 光
	LightManager::Instance().SetAmbientColor({ 0, 0, 0, 0 });
	Light* dl = new Light(LightType::Directional);
	dl->SetDirection({ 0.0f, -0.503f, -0.864f });
	LightManager::Instance().Register(dl);

	CameraManager& cameraManager = CameraManager::Instance();
	Camera* mainCamera = new Camera();
	cameraManager.Register(mainCamera);
	cameraManager.SetCamera(0);

	// カメラ設定
	CameraManager::Instance().GetCamera()->SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),							// 画角
		T_GRAPHICS.GetScreenWidth() / T_GRAPHICS.GetScreenHeight(),	// 画面アスペクト比
		0.1f,														// ニアクリップ
		10000.0f													// ファークリップ
	);
	CameraManager::Instance().GetCamera()->SetLookAt(
		{ 0, 5.0f, 10.0f },	// 視点
		{ 0, 0, 0 },	// 注視点
		{ 0, 0.969f, -0.248f } // 上ベクトル
	);

	cameraController = std::make_unique<ThridPersonCameraController>();
	cameraController->SyncCameraToController(CameraManager::Instance().GetCamera());
	//cameraController->SetEnable(false);
	cameraController->SetEnable(true);
	cameraController->SetPlayer(newPlayer.get());
}

void SceneTest::Finalize()
{
	LightManager::Instance().Clear();
	CameraManager::Instance().Clear();
	MAPTILES.Clear();
}

// 更新処理
void SceneTest::Update(float elapsedTime)
{
	cameraController->Update(elapsedTime);
	cameraController->SyncContrllerToCamera(CameraManager::Instance().GetCamera());

	MAPTILES.Update(elapsedTime);

	//player->Update(elapsedTime);
	newPlayer->Update(elapsedTime);
}

// 描画処理
void SceneTest::Render()
{
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->Clear(T_GRAPHICS.GetDeviceContext(), 0.2f, 0.2f, 0.2f, 1);
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->SetRenderTarget(T_GRAPHICS.GetDeviceContext());

	// 描画コンテキスト設定
	RenderContext rc;
	rc.camera = CameraManager::Instance().GetCamera();
	rc.deviceContext = T_GRAPHICS.GetDeviceContext();
	rc.renderState = T_GRAPHICS.GetRenderState();

	// ライトの情報を詰め込む
	LightManager::Instance().PushRenderContext(rc);

	// 描画
	//knight->Render(rc);
	//player->Render(rc);
	MAPTILES.Render(rc);
	newPlayer->Render(rc);

	ProfileDrawUI();
}
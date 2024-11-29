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
	newPlayer->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::IDLE));

	// 光
	LightManager::Instance().SetAmbientColor({ 0.3f, 0.3f, 0.3f, 0.0f });
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
	MAPTILES.Render(rc);
	newPlayer->Render(rc);

	ProfileDrawUI();
}
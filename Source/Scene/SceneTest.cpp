#include <profiler.h>
#include "Scene/SceneTest.h"
#include "Map/MapTileManager.h"

void SceneTest::Initialize()
{
	stage = std::make_unique<MapTile>("Data/Model/Stage/scene.glb", 1000);
	MAPTILES.Register(stage.get());
	MAPTILES.Update(0);

	player = std::make_unique<Knight>();
	//knight = std::make_unique<ModelObject>("Data/Model/Character/Knight.glb");
	//knight->SetAnimation(22, true, 0.0f);
	//knight->SetPosition({ 1.08f, 0.0f, 2.12f });
	//knight->SetAngle({ 0.0f, -1.22f, 0.0f });
	//knight->GetModel()->FindNode("1H_Sword_Offhand")->visible = false;
	//knight->GetModel()->FindNode("2H_Sword")->visible = false;
	//knight->GetModel()->FindNode("Badge_Shield")->visible = false;
	//knight->GetModel()->FindNode("Round_Shield")->visible = false;
	//knight->GetModel()->FindNode("Spike_Shield")->visible = false;
	player->SetPosition({ 5, -50, 5 });
	player->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));

	// 光
	LightManager::Instance().SetAmbientColor({ 0, 0, 0, 0 });
	Light* dl = new Light(LightType::Directional);
	dl->SetDirection({ 0.0f, -0.503f, -0.864f });
	LightManager::Instance().Register(dl);

	// カメラ設定
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),							// 画角
		T_GRAPHICS.GetScreenWidth() / T_GRAPHICS.GetScreenHeight(),	// 画面アスペクト比
		0.1f,														// ニアクリップ
		10000.0f													// ファークリップ
	);
	camera.SetLookAt(
		{ 0, 5.0f, 10.0f },	// 視点
		{ 0, 0, 0 },	// 注視点
		{ 0, 0.969f, -0.248f } // 上ベクトル
	);

	cameraController = std::make_unique<ThridPersonCameraController>();
	cameraController->SyncCameraToController(camera);
	//cameraController->SetEnable(false);
	cameraController->SetEnable(true);
	cameraController->SetPlayer(player.get());

}

void SceneTest::Finalize()
{
	LightManager::Instance().Clear();
}

// 更新処理
void SceneTest::Update(float elapsedTime)
{
	cameraController->Update(elapsedTime);
	cameraController->SyncContrllerToCamera(camera);

	stage->Update(elapsedTime);

	player->Update(elapsedTime);
}

// 描画処理
void SceneTest::Render()
{
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->Clear(T_GRAPHICS.GetDeviceContext(), 0.2f, 0.2f, 0.2f, 1);
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->SetRenderTargets(T_GRAPHICS.GetDeviceContext());

	// 描画コンテキスト設定
	RenderContext rc;
	rc.camera = &camera;
	rc.deviceContext = T_GRAPHICS.GetDeviceContext();
	rc.renderState = T_GRAPHICS.GetRenderState();

	// ライトの情報を詰め込む
	LightManager::Instance().PushRenderContext(rc);

	// 描画
	stage->Render(rc);
	//knight->Render(rc);
	player->Render(rc);
}


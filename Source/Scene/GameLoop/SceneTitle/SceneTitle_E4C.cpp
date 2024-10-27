#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Network/HttpRequest.h"
#include "TAKOEngine/Rendering/ResourceManager.h"
#include "TAKOEngine/GUI/UIManager.h"

#include <imgui.h>
#include <string>

#include "Scene/SceneManager.h"
#include "SceneTitle_E4C.h"
#include "SceneTitle_E4CState.h"

#include "GameData.h"

//テスト用
float SceneTitle_E4C::time = { 0 };

void SceneTitle_E4C::Initialize()
{
	// Sprite Resource Preload
	for (auto& filename : spriteList)
	{
		spritePreLoad.insert(RESOURCE.LoadSpriteResource(filename));
	}

	//シャドウマップレンダラ
	shadowMapRenderer->Initialize();

	// フレームバッファマネージャー
	m_framBuffer = T_GRAPHICS.GetFramBufferManager();

	// モデル
	{
		m_sprites[0] = std::make_unique<SpriteDX12>(1, "Data/Sprites/UI/start.png");
		m_sprites[0] = std::make_unique<SpriteDX12>(1, "Data/Sprites/UI/exit.png");
	}

	// 光
	LightManager::Instance().SetAmbientColor({ 0, 0, 0, 0 });
	Light* dl = new Light(LightType::Directional);
	dl->SetDirection({ 0.0f, -0.503f, -0.864f });
	LightManager::Instance().Register(dl);
	shadowMapRenderer->SetShadowLight(dl);

	// カメラ設定
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),		// 画角
		SCREEN_W / SCREEN_H,					// 画面アスペクト比
		0.1f,									// ニアクリップ
		10000.0f								// ファークリップ
	);
	camera.SetLookAt(
		{ -5.661f, 2.5f, 5.584f },				// 視点
		{ 0.0f, 2.0, 0.0f },					// 注視点
		{ 0.036f, 0.999f, -0.035f }				// 上ベクトル
	);
	cameraController = std::make_unique<FreeCameraController>();
	cameraController->SyncCameraToController(camera);
	cameraController->SetEnable(false);

	// ステート
	stateMachine = std::make_unique<StateMachine<SceneTitle_E4C>>();
	stateMachine->RegisterState(STATE::INIT, new SceneTitle_E4CState::InitState(this));
	stateMachine->RegisterState(STATE::START, new SceneTitle_E4CState::StartState(this));
	stateMachine->RegisterState(STATE::EXIT, new SceneTitle_E4CState::ExitState(this));
	stateMachine->SetState(STATE::INIT);
}

void SceneTitle_E4C::Finalize()
{
	spritePreLoad.clear();
	UI.Clear();
	shadowMapRenderer->Clear();
}

// 更新処理
void SceneTitle_E4C::Update(float elapsedTime)
{
	time += elapsedTime;

	stateMachine->Update(elapsedTime);

#ifdef _DEBUG
	// カメラ更新
	cameraController->Update();
	cameraController->SyncContrllerToCamera(camera);
#endif // _DEBUG

	UI.Update(elapsedTime);
}

// 描画処理
void SceneTitle_E4C::Render()
{
	T_TEXT.Begin();

	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->Clear(T_GRAPHICS.GetDeviceContext(), 0.2f, 0.2f, 0.2f, 1);
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->SetRenderTarget(T_GRAPHICS.GetDeviceContext());

	// 描画コンテキスト設定
	RenderContext rc;
	rc.camera = &camera;
	rc.deviceContext = T_GRAPHICS.GetDeviceContext();
	rc.renderState = T_GRAPHICS.GetRenderState();

	// ライトの情報を詰め込む
	LightManager::Instance().PushRenderContext(rc);

	//	シャドウマップ描画
	shadowMapRenderer->Render();
	rc.shadowMapData = shadowMapRenderer->GetShadowMapData();

	UI.Render(rc);

	T_TEXT.End();

#ifdef _DEBUG
	// DebugIMGUI
	//DrawSceneGUI();
	//shadowMapRenderer->DrawDebugGUI();
#endif // _DEBUG
}

void SceneTitle_E4C::RenderDX12()
{
	TentacleLib::graphics.BeginRender();
	{
		Camera& camera = Camera::Instance();

		// シーン用定数バッファ更新
		const Descriptor* scene_cbv_descriptor = TentacleLib::graphics.UpdateSceneConstantBuffer(
			Camera::Instance());

		// レンダーコンテキスト設定
		RenderContextDX12 rc;
		rc.d3d_command_list = m_framBuffer->GetCommandList();
		rc.scene_cbv_descriptor = scene_cbv_descriptor;

		// スプライト描画
		if (m_sprites[0] != nullptr)
		{
			m_sprites[0]->Begin(rc);
			m_sprites[0]->Draw(0, 0, 100, 100, 0, 1, 1, 1, 1);
			m_sprites[0]->End(m_framBuffer->GetCommandList());
		}
	}
	TentacleLib::graphics.End();
}

void SceneTitle_E4C::DrawSceneGUI()
{
}
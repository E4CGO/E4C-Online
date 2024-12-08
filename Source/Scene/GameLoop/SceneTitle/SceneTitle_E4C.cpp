#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Network/HttpRequest.h"
#include "TAKOEngine/Rendering/ResourceManager.h"
#include "TAKOEngine/GUI/UIManager.h"

#include "TAKOEngine/Sound/Sound.h"
#include "TAKOEngine/Editor/Camera/CameraManager.h"

#include <imgui.h>
#include <string>
#include <fstream>

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
	m_frameBuffer = T_GRAPHICS.GetFrameBufferManager();

	// 光
	LightManager::Instance().SetAmbientColor({ 0.3f, 0.3f, 0.3f, 0.0f });
	Light* dl = new Light(LightType::Directional);
	dl->SetDirection({ 0.0f, -0.503f, -0.864f });
	LightManager::Instance().Register(dl);
	shadowMapRenderer->SetShadowLight(dl);

	CameraManager& cameramanager = CameraManager::Instance();
	Camera* mainCamera = new Camera();
	cameramanager.Register(mainCamera);
	cameramanager.SetCamera(0);

	// カメラ設定
	CameraManager::Instance().GetCamera()->SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),		// 画角
		SCREEN_W / SCREEN_H,					// 画面アスペクト比
		0.1f,									// ニアクリップ
		10000.0f								// ファークリップ
	);
	CameraManager::Instance().GetCamera()->SetLookAt(
		{ -5.661f, 2.5f, 5.584f },				// 視点
		{ 0.0f, 2.0, 0.0f },					// 注視点
		{ 0.036f, 0.999f, -0.035f }				// 上ベクトル
	);
	cameraController = std::make_unique<FreeCameraController>();
	cameraController->SyncCameraToController(CameraManager::Instance().GetCamera());
	cameraController->SetEnable(false);

	// ステート
	stateMachine = std::make_unique<StateMachine<SceneTitle_E4C>>();
	stateMachine->RegisterState(STATE::INIT, new SceneTitle_E4CState::InitState(this));
	stateMachine->RegisterState(STATE::START, new SceneTitle_E4CState::StartState(this));
	stateMachine->RegisterState(STATE::EXIT, new SceneTitle_E4CState::ExitState(this));
	stateMachine->SetState(STATE::INIT);

	Sound::Instance().InitAudio();
	Sound::Instance().LoadAudio("Data/Sound/TitleTheme.mp3");
	Sound::Instance().PlayAudio(0);

	graphicsMemory = std::make_unique<GraphicsMemory>(T_GRAPHICS.GetDeviceDX12());

	m_resourceDescriptors = std::make_unique<DirectX::DescriptorHeap>(T_GRAPHICS.GetDeviceDX12(),
		Descriptors::Count);

	ResourceUploadBatch resourceUpload(T_GRAPHICS.GetDeviceDX12());

	resourceUpload.Begin();

	RenderTargetState rtState(RenderTargetFormat, DepthStencilFormat);

	SpriteBatchPipelineStateDescription pd(rtState);
	m_spriteBatch = std::make_unique<DirectX::DX12::SpriteBatch>(T_GRAPHICS.GetDeviceDX12(), resourceUpload, pd);

	m_font = std::make_unique <DirectX::DX12::SpriteFont>(T_GRAPHICS.GetDeviceDX12(), resourceUpload,
		L"Data/Font/SegoeUI_48.spritefont",
		m_resourceDescriptors->GetCpuHandle(Descriptors::MyFont),
		m_resourceDescriptors->GetGpuHandle(Descriptors::MyFont));

	auto viewport = T_GRAPHICS.GetViewport();
	m_spriteBatch->SetViewport(viewport);

	m_fontPos.x = float(T_GRAPHICS.GetScreenWidth()) / 2.f;
	m_fontPos.y = float(T_GRAPHICS.GetScreenHeight()) / 2.f;

	auto uploadResourcesFinished = resourceUpload.End(
		T_GRAPHICS.GetCommandQueue().d3d_command_queue.Get());

	uploadResourcesFinished.wait();
}

void SceneTitle_E4C::Finalize()
{
	spritePreLoad.clear();
	UI.Clear();
	shadowMapRenderer->Clear();
	Sound::Instance().StopAudio(0);
	CameraManager::Instance().Clear();
	LightManager::Instance().Clear();
}

// 更新処理
void SceneTitle_E4C::Update(float elapsedTime)
{
	time += elapsedTime;

	stateMachine->Update(elapsedTime);

#ifdef _DEBUG
	// カメラ更新
	cameraController->Update();
	cameraController->SyncContrllerToCamera(CameraManager::Instance().GetCamera());
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
	rc.camera = CameraManager::Instance().GetCamera();
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
	T_GRAPHICS.BeginRender();
	{
		// シーン用定数バッファ更新
		const Descriptor* scene_cbv_descriptor = T_GRAPHICS.UpdateSceneConstantBuffer(
			CameraManager::Instance().GetCamera());

		// レンダーコンテキスト設定
		RenderContextDX12 rc;
		rc.d3d_command_list = m_frameBuffer->GetCommandList();
		rc.scene_cbv_descriptor = scene_cbv_descriptor;

		m_spriteBatch->Begin(m_frameBuffer->GetCommandList());

		const wchar_t* output = L"MOJI BYOGA WA MUZUKASHII";

		DirectX::SimpleMath::Vector2 origin = m_font->MeasureString(output) / 2.f;

		m_font->DrawString(m_spriteBatch.get(), output,
			m_fontPos, Colors::White, 0.f, origin);

		m_spriteBatch->End();

		UI.RenderDX12(rc);
	}
	T_GRAPHICS.End();
}

void SceneTitle_E4C::DrawSceneGUI()
{
}
#include "TAKOEngine/Runtime/tentacle_lib.h"

#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "PreloadManager.h"
// 初期化
void SceneLoading::Initialize()
{
	PRELOAD.Join("SceneLoading.Sprite");
	if (T_GRAPHICS.isDX12Active)
	{
		m_frameBuffer = T_GRAPHICS.GetFrameBufferManager();

		m_loadingPlane = std::make_unique<PlaneDX12>("Data/Sprites/gear.png", 1.0f, XMFLOAT3{ 0.0f, 0.0f, -10.0f }, 0.0f, 1.5f);
		m_loadingPlane->SetShaderDX12(ModelShaderDX12Id::Loading);

		mainCamera = std::make_unique<Camera>();
		mainCamera->SetPerspectiveFov(
			DirectX::XMConvertToRadians(45),							// 画角
			T_GRAPHICS.GetScreenWidth() / T_GRAPHICS.GetScreenHeight(),	// 画面アスペクト比
			0.1f,														// ニアクリップ
			10000.0f													// ファークリップ
		);

		mainCamera->SetLookAt(
			{ 0.0, 0.0f, 1.0f },		// 視点
			{ 0.0f, 0.0f, 0.0f },	// 注視点
			{ 0, 0.969f, -0.248f }	// 上ベクトル
		);
	}

	thread = new std::thread(LoadingThread, this);
}
// 終了化
void SceneLoading::Finalize()
{
	if (thread != nullptr)
	{
		thread->join();
		delete thread;
		thread = nullptr;
	}
}
// 更新処理
void SceneLoading::Update(float elapsedTime)
{
	if (nextScene->IsReady())
	{
		SceneManager::Instance().ChangeScene(nextScene);
	}

	m_timer += elapsedTime;
}
// 描画処理
void SceneLoading::Render()
{
	// 画面クリア
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->Clear(T_GRAPHICS.GetDeviceContext(), 0, 0, 0, 1);

	T_TEXT.Begin();

	T_TEXT.Render(
		FONT_ID::HGpop,
		"Now Loading...",
		T_GRAPHICS.GetScreenWidth(), T_GRAPHICS.GetScreenHeight(),
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		FONT_ALIGN::BOTTOM_RIGHT
	);

	T_TEXT.End();
}

void SceneLoading::RenderDX12()
{
	RenderContextDX12 rc;

	T_GRAPHICS.BeginRender();

	m_frameBuffer->WaitUntilToPossibleSetRenderTarget(T_GRAPHICS.GetFrameBufferDX12(FrameBufferDX12Id::Scene));
	m_frameBuffer->SetRenderTarget(T_GRAPHICS.GetFrameBufferDX12(FrameBufferDX12Id::Scene));
	m_frameBuffer->Clear(T_GRAPHICS.GetFrameBufferDX12(FrameBufferDX12Id::Scene));

	const Descriptor* scene_cbv_descriptor = T_GRAPHICS.UpdateSceneConstantBuffer(
		mainCamera.get(), m_timer, 0);

	rc.d3d_command_list = m_frameBuffer->GetCommandList();
	rc.scene_cbv_descriptor = scene_cbv_descriptor;

	m_loadingPlane->RenderDX12(rc);

	m_frameBuffer->WaitUntilFinishDrawingToRenderTarget(T_GRAPHICS.GetFrameBufferDX12(FrameBufferDX12Id::Scene));

	postprocessingRenderer->Render(m_frameBuffer);

	T_TEXT.BeginDX12();

	T_TEXT.RenderDX12(
		FONT_ID::HGpop,
		L"Now Loading...",
		T_GRAPHICS.GetScreenWidth() * 0.95f, T_GRAPHICS.GetScreenHeight() * 0.95f,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		FONT_ALIGN::RIGHT
	);

	T_TEXT.EndDX12();

	T_GRAPHICS.GetImGUIRenderer()->RenderDX12(T_GRAPHICS.GetFrameBufferManager()->GetCommandList());
	T_GRAPHICS.End();
}

void SceneLoading::LoadingThread(SceneLoading* scene)
{
	srand(static_cast <unsigned> (time(NULL)));

	// COM関連の初期化でスレッド毎に呼ぶ必要がある
	CoInitialize(nullptr);

	// 次のシーンの初期化を行う
	scene->nextScene->Initialize();

	// スレッドが終わる前にCOM関連の終了化
	CoUninitialize();

	// 次のシーンの準備完了設定
	scene->nextScene->SetReady();
}
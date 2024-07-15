#include <memory>
#include <sstream>
#include <imgui.h>

#include "Framework.h"

#include "TAKOEngine/Tool/ImGuiRenderer.h"
#include "TAKOEngine/Effects/EffectManager.h"

#include "Scene/SceneGame.h"
#include "Scene/SceneTitle.h"
#include "Scene/SceneManager.h"

#include "GameData.h"

// 垂直同期間隔設定
static const int syncInterval = 1;
extern bool gPause;

// コンストラクタ
Framework::Framework(HWND hWnd)
	: hWnd(hWnd)
{
	TentacleLib::SetSyncInterval(syncInterval);
	TentacleLib::SetShowFPS(true);

	// IMGUI初期化
	ImGuiRenderer::Initialize(hWnd, T_GRAPHICS.GetDevice(), T_GRAPHICS.GetDeviceContext());

	// ネットワーク
	Network::Initialize();
	// IPを取得
	char address[256];
	Network::GetIpAddress(address, sizeof(address));
	GAME_DATA.SetIp(address);

	// エフェクトマネージャー初期化
	EFFECTS.Initialize();

	// シーン初期化
	SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
	//SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame("nagi", "192.168.0.236", "7000", new HostNetworkController)));
}

// デストラクタ
Framework::~Framework()
{
	SceneManager::Instance().Clear();

	// エフェクトマネージャー終了化
	EFFECTS.Finalize();

	// IMGUI終了
	ImGuiRenderer::Finalize();

	Network::Finalize();

	T_GRAPHICS.WaitIdle();
}

// 更新処理
void Framework::Update(float elapsedTime)
{
	if (T_INPUT.KeyDown(VK_F1)) DX12API = !DX12API;
	// シーン更新処理
	SceneManager::Instance().Update(elapsedTime);
}

// 描画処理
void Framework::Render(float elapsedTime)
{
	// 別スレッド中にデバイスコンテキストが使われていた場合に
	// 同時アクセスしないように排他制御す
	std::lock_guard<std::mutex> lock(T_GRAPHICS.GetMutex());

	if (!DX12API)
	{
		ID3D11DeviceContext* dc = T_GRAPHICS.GetDeviceContext();

		// IMGUI処理開始
		ImGuiRenderer::NewFrame();

		// レンダーターゲット設定
		T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->SetRenderTarget(dc);

		// シーン描画処理
		SceneManager::Instance().Render();

		// IMGUI描画
		ImGuiRenderer::Render(dc);

		// 画面表示
		TentacleLib::Draw();
	}
	else
	{
		T_GRAPHICS.Execute();
		SceneManager::Instance().RenderDX12();
	}
}

// アプリケーションループ
int Framework::Run()
{
	MSG msg = {};

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// TODO: Move update
			TentacleLib::Update();
			float elapsedTime = TentacleLib::timer.Delta();

			if (gPause) elapsedTime = 0.0f;
			Update(elapsedTime);
			Render(elapsedTime);
		}
	}

	TentacleLib::Finalize();

	return static_cast<int>(msg.wParam);
}

// メッセージハンドラ
LRESULT CALLBACK Framework::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGuiRenderer::HandleMessage(hWnd, msg, wParam, lParam)) return true;

	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_KEYDOWN:
		//if (wParam == VK_ESCAPE) PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	case WM_MOUSEWHEEL:
	{
		T_INPUT.UpdateMouseScroll(wParam);
	}
	break;
	case WM_ENTERSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		//timer.Stop();
		break;
	case WM_EXITSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
		//timer.Start();
		break;

	case WM_SYSKEYUP:
	case WM_SYSKEYDOWN:
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
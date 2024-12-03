#include <memory>
#include <sstream>

#include "Framework.h"
#include <imgui.h>

#include "TAKOEngine/Tool/ImGuiRenderer.h"
#include "TAKOEngine/Effects/EffectManager.h"

#define MINIMP3_IMPLEMENTATION
#include "TAKOEngine/Sound/Sound.h"
#include "TAKOEngine/Network/Network.h"
#include "Scene/SceneManager.h"
#include "Scene/SceneTest.h"
#include "Scene/GameLoop/SceneTitle/SceneTitle_E4C.h"
#include "Scene/GameLoop/SceneRoomTest/SceneRoomTest_E4C.h"

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

	if (T_GRAPHICS.isDX11Active)
	{
		// IMGUI初期化
		ImGuiRenderer::Initialize(hWnd, T_GRAPHICS.GetDevice(), T_GRAPHICS.GetDeviceContext());
	}

	// ネットワーク
	Network::Initialize();
	// IPを取得
	char address[256];
	Network::GetIpAddress(address, sizeof(address));
	GAME_DATA.SetIp(address);

	// エフェクトマネージャー初期化
	if (T_GRAPHICS.isDX11Active)
	{
		EFFECTS.Initialize();
	}

	if (T_GRAPHICS.isDX12Active)
	{
		//EFFECTS.InitializeDX12();
	}

	// シーン初期化
	//SceneManager::Instance().ChangeScene(new SceneLoading(new SceneRoomTest_E4C));
	SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle_E4C));
}

// デストラクタ
Framework::~Framework()
{
	T_GRAPHICS.WaitIdle();

	SceneManager::Instance().Clear();

	// エフェクトマネージャー終了化
	EFFECTS.Finalize();

	if (T_GRAPHICS.isDX11Active)
	{
		// IMGUI終了
		ImGuiRenderer::Finalize();
	}

	Network::Finalize();

	Sound::Instance().Finalize();
}

// 更新処理
void Framework::Update(float elapsedTime)
{
	if (T_INPUT.KeyDown(VK_F1))
	{
		T_GRAPHICS.isDX12Active = !T_GRAPHICS.isDX12Active;
		T_GRAPHICS.isDX11Active = !T_GRAPHICS.isDX11Active;
	}

	// シーン更新処理
	SceneManager::Instance().Update(elapsedTime);
}

// 描画処理
void Framework::Render(float elapsedTime)
{
	// 別スレッド中にデバイスコンテキストが使われていた場合に
	// 同時アクセスしないように排他制御す
	std::lock_guard<std::mutex> lock(T_GRAPHICS.GetMutex());

	if (T_GRAPHICS.isDX11Active)
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
	if (T_GRAPHICS.isDX12Active)
	{
		T_GRAPHICS.GetImGUIRenderer()->NewFrameDX12();

		SceneManager::Instance().RenderDX12();

		T_GRAPHICS.Execute();
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
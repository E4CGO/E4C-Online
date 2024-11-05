#include "tentacle_lib.h"

#include <sstream>
#include <memory>
#include <assert.h>
#include <tchar.h>

//#define FULLSCREEN

namespace TentacleLib
{
	HWND Init(LPCWSTR APPLICATION_NAME, HINSTANCE instance, LONG width, LONG height, LPWSTR cmd_line, INT cmd_show, WNDPROC callback)
	{
		srand(static_cast <unsigned> (time(NULL)));

#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

#endif

		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = callback;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = instance;
		wcex.hIcon = 0;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = APPLICATION_NAME;
		wcex.hIconSm = 0;
		RegisterClassEx(&wcex);

		SetProcessDPIAware();
		RECT rc = { 0, 0, width, height };

#ifdef FULLSCREEN

		width = GetSystemMetrics(SM_CXSCREEN);
		height = GetSystemMetrics(SM_CYSCREEN);

		hWnd = CreateWindow(APPLICATION_NAME, _T(""), WS_POPUP, 0, 0, width, height, NULL, NULL, instance, NULL);
#else
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
		hWnd = CreateWindow(APPLICATION_NAME, _T(""), WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, instance, NULL);

#endif// FULLSCREEN

		ShowWindow(hWnd, cmd_show);

		std::wostringstream outs;
		outs.precision(6);
		outs << APPLICATION_NAME;
		SetWindowTextW(hWnd, APPLICATION_NAME);

		setting.title = APPLICATION_NAME;

		//SDL_Init(SDL_INIT_AUDIO);
		input.Init(hWnd);
		timer.Init();
		//audio.Init();
		graphics.Initalize(hWnd, 2);

		textSprite.Init(graphics.GetDevice());

		return hWnd;
	}

	void Finalize()
	{
		//SDL_Quit();
	}

	void SetSyncInterval(UINT syncInterval)
	{
		setting.syncInterval = syncInterval;
	}
	void SetShowFPS(bool showFPS) {
		setting.showFPS = showFPS;
	}

	void Update()
	{
		if (timer.Tick()) // 1sec
		{
			if (setting.showFPS)
			{
				std::wostringstream outs;
				outs.precision(6);
				float fps = static_cast<float>(timer.getFPS());
				outs << setting.title << " | FPS: " << fps << " | Frame Time :" << 1000.0f / fps << " (ms) | " << timer.Delta();
				SetWindowTextW(hWnd, outs.str().c_str());
			}
		}

		input.Update();
	}

	void Draw()
	{
		graphics.Present(setting.syncInterval);
	}

	bool isFocus()
	{
		return GetFocus() == hWnd;
	}

	bool isShowCursor()
	{
		CURSORINFO ci = { sizeof(CURSORINFO) };
		if (GetCursorInfo(&ci))
			return ci.flags & CURSOR_SHOWING;
		return false;
	}
}
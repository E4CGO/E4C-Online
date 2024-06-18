#include <tchar.h>
#include <iostream>
#include "Framework.h"

const LONG SCREEN_WIDTH = 1280;
const LONG SCREEN_HEIGHT = 720;

LPCWSTR GAME_TITLE = _T("P2Pダンジョン・アドベンチャーDX12");

LRESULT CALLBACK fnWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Framework* f = reinterpret_cast<Framework*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	return f ? f->HandleMessage(hwnd, msg, wparam, lparam) : DefWindowProc(hwnd, msg, wparam, lparam);
}

INT WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPWSTR cmd_line, INT cmd_show)
{
	HWND hWnd = TentacleLib::Init(GAME_TITLE, instance, SCREEN_WIDTH, SCREEN_HEIGHT, cmd_line, cmd_show, fnWndProc);

	Framework f(hWnd);
	SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&f));
	return f.Run();
}
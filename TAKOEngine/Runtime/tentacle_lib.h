#pragma once

#define _WINSOCKAPI_

#define IM_ASSERT(_EXPR) ((void)(_EXPR))

#pragma warning(disable : 4099)

#include <unordered_map>

#include "TAKOEngine/Rendering/Graphics.h"
#include "TAKOEngine/Rendering/ResourceManager.h"
#include "TAKOEngine/Rendering/Sprite.h"
#include "TAKOEngine/Rendering/TextSprite.h"
#include "TAKOEngine/Editor/Input/Input.h"
#include "TAKOEngine/Tool/Timer.h"
#include "TAKOEngine/Tool/Console.h"
#include "TAKOEngine/Tool/XMFLOAT.h"
#include "TAKOEngine/Rendering/Misc.h"

#include <windows.h>

namespace TentacleLib
{
	static struct SETTING
	{
		LPCWSTR title;
		UINT syncInterval = 1;
		bool showFPS = false;
	} setting;
	static HWND hWnd;

	HWND Init(LPCWSTR, HINSTANCE, LONG width, LONG height, LPWSTR cmd_line, INT cmd_show, WNDPROC);
	void Finalize();

	void SetSyncInterval(UINT syncInterval);
	void SetShowFPS(bool showFPS);

	void Update();
	void Draw();

	bool isFocus();
	bool isShowCursor();

	static Graphics& graphics = Graphics::Instance();
	static TentacleLib::Timer& timer = TentacleLib::Timer::Instance();
	static Input& input = Input::Instance();
	static Console& console = Console::Instance();
	static ResourceManager& resource = ResourceManager::Instance();
	static TextSprite& textSprite = TextSprite::Instance();
}
#define T_GRAPHICS TentacleLib::graphics
#define T_TIMER TentacleLib::timer
#define T_INPUT TentacleLib::input
#define T_RESOURCE TentacleLib::resource
#define T_TEXT TentacleLib::textSprite
#define T_CONSOLE TentacleLib::console
#define SCREEN_W TentacleLib::graphics.GetScreenWidth()
#define SCREEN_H TentacleLib::graphics.GetScreenHeight()
#define RADIAN1 0.01745329251f
#define CURSOR_OFF while (ShowCursor(false) >= 0)
#define CURSOR_ON while (ShowCursor(true) <= 0)
#define RANDOMI(l,h) (rand() % (h) + (l))
#define RANDOMF(l,h) ((l) + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/((h)-(l)))))
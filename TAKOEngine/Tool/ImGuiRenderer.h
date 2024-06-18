#pragma once

#undef _WINSOCKAPI_
#define _WINSOCKAPI
#include <windows.h>
#include <d3d11.h>
#include <string>
#include <imgui.h>

class ImGuiRenderer
{
public:
	// 初期化
	static void Initialize(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* dc);

	// 終了化
	static void Finalize();

	// フレーム開始処理
	static void NewFrame();

	// 描画実行
	static void Render(ID3D11DeviceContext* context);

	// WIN32メッセージハンドラー
	static LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	static std::string path;
	static void LoadFontFile(const char* filename, float size);
	static void CacheFonts(int idx);
	static bool LoadFontsCache(int idx);
public:
	enum FONT {
		DEFAULT,
		TAMANEGI,
	};
};

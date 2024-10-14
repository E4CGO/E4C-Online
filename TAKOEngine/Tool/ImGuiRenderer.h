//! @file ImGuiRenderer.h
//! @note ImGuiを管理

#ifndef __INCLUDED_IMGUI_RENDERER_H__
#define __INCLUDED_IMGUI_RENDERER_H__

#undef _WINSOCKAPI_
#define _WINSOCKAPI

#include <windows.h>
#include <d3d11.h>
#include <d3d12.h>
#include <memory>
#include <string>
#include <imgui.h>

#include "TAKOEngine/Rendering/Descriptor.h"

/**************************************************************************//**
		@class		ImGuiRenderer
		@brief		ImGui管理クラス
		@par		ImGuiを設定とレンダリング
*//***************************************************************************/
class ImGuiRenderer
{
public:
	// 初期化
	static void Initialize(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* dc);

	// DX12で初期化
	ImGuiRenderer(
		HWND hwnd,
		ID3D12Device* d3d_device,
		DXGI_FORMAT dxgi_format,
		int buffer_count,
		std::shared_ptr<DescriptorHeap> descriptor_heap);

	// 終了化
	static void Finalize();

	// 終了化
	void FinalizeDX12();

	// リサイズ
	void Resize(float width, float height);

	// フレーム開始処理
	static void NewFrame();

	// フレーム開始処理
	void NewFrameDX12();

	// 描画実行
	static void Render(ID3D11DeviceContext* context);

	// DX12で描画実行
	void RenderDX12(ID3D12GraphicsCommandList* d3d_command_list);

	// WIN32メッセージハンドラー
	static LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	enum FONT {
		DEFAULT,
		TAMANEGI,
	};

private:
	static std::string path;
	static void LoadFontFile(const char* filename, float size);
	static void CacheFonts(int idx);
	static bool LoadFontsCache(int idx);

	std::shared_ptr<DescriptorHeap>	m_descriptor_heap;
	const Descriptor* m_pFont_srv_descriptor = nullptr;
	ImGuiContext* m_pImgui_context = nullptr;
};

#endif // __INCLUDED_IMGUI_RENDERER_H__

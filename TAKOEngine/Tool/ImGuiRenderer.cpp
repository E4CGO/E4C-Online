#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include "ImGuiRenderer.h"
#include <string_view>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <profiler.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool gPause = false;

std::string ImGuiRenderer::path = "Data/Font/Fonts.imfont";

// 初期化
void ImGuiRenderer::Initialize(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* dc)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
#ifndef _DEBUG
	io.IniFilename = NULL;
	io.LogFilename = NULL;
#endif // !_DEBUG

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;
	io.ConfigViewportsNoDefaultParent = true;
	//io.ConfigDockingAlwaysTabBar = true;
	//io.ConfigDockingTransparentPayload = true;
#if 1
	io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
	io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI
#endif

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(device, dc);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);

	LoadFontFile("Data/Font/ArialUni.ttf", 16.0f);

	ProfileInitialize(&gPause, [](bool pause) { gPause = pause; });
}

// 終了化
void ImGuiRenderer::Finalize()
{
	ProfileShutdown();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

// フレーム開始処理
void ImGuiRenderer::NewFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	ProfileNewFrame();

#if 0
	// Docking
	const ImGuiWindowFlags window_flags = ImGuiWindowFlags_None
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoBringToFrontOnFocus
		| ImGuiWindowFlags_NoNavFocus
		| ImGuiWindowFlags_NoBackground
		;
	const ImGuiDockNodeFlags docspace_flags = ImGuiDockNodeFlags_None
		//| ImGuiDockNodeFlags_KeepAliveOnly
		| ImGuiDockNodeFlags_PassthruCentralNode
		;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	bool dock_open = true;
	if (ImGui::Begin("MainDockspace", &dock_open, window_flags))
	{
		ImGui::PopStyleVar(3);

		ImGuiID dockspaceId = ImGui::GetID("MyDockspace");
		ImGui::DockSpace(dockspaceId, ImVec2(0, 0), docspace_flags);
	}
	ImGui::End();
#endif
}

// 描画
void ImGuiRenderer::Render(ID3D11DeviceContext* context)
{
	// Rendering
	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

// WIN32メッセージハンドラー
LRESULT ImGuiRenderer::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
}

void ImGuiRenderer::LoadFontFile(const char* filename, float size)
{
	ImGuiIO& io = ImGui::GetIO();
	ImFont* font = io.Fonts->AddFontFromFileTTF(filename, size, NULL, io.Fonts->GetGlyphRangesJapanese());
	IM_ASSERT(font != NULL);
}

void ImGuiRenderer::CacheFonts(int idx)
{
	ImFontAtlas* font_atlas = ImGui::GetIO().Fonts;

	std::ofstream file(std::string_view(path + std::to_string(idx)).data(), std::ios::out | std::ios::binary);

	file.write(reinterpret_cast<const char*>(&font_atlas->TexUvWhitePixel), sizeof(font_atlas->TexUvWhitePixel));
	file.write(reinterpret_cast<const char*>(font_atlas->TexUvLines), sizeof(font_atlas->TexUvLines));

	auto size = font_atlas->Fonts[idx]->Glyphs.size();
	file.write(reinterpret_cast<const char*>(&size), sizeof(int));
	file.write(reinterpret_cast<const char*>(font_atlas->Fonts[idx]->Glyphs.Data), sizeof(ImFontGlyph) * size);
	file.write(reinterpret_cast<const char*>(&font_atlas->Fonts[idx]->FontSize), sizeof(float));

	unsigned char* out_pixels{};
	int out_width{};
	int out_height{};
	font_atlas->GetTexDataAsAlpha8(&out_pixels, &out_width, &out_height);
	file.write(reinterpret_cast<const char*>(&out_width), sizeof(int));
	file.write(reinterpret_cast<const char*>(&out_height), sizeof(int));
	file.write(reinterpret_cast<const char*>(out_pixels), out_width * out_height);
}
bool ImGuiRenderer::LoadFontsCache(int idx)
{
	if (!std::filesystem::exists(std::string_view(path + std::to_string(idx))))
	{
		return false;
	}
	std::ifstream file(std::string_view(path + std::to_string(idx)).data(), std::ios::in | std::ios::binary);

	// Load
	ImVec2 TexUvWhitePixel;
	file.read(reinterpret_cast<char*>(&TexUvWhitePixel), sizeof(TexUvWhitePixel));

	ImVec4 TexUvLines[IM_DRAWLIST_TEX_LINES_WIDTH_MAX + 1];
	file.read(reinterpret_cast<char*>(TexUvLines), sizeof(TexUvLines));

	int glyph_size{};
	std::vector<ImFontGlyph> glyphs;
	file.read(reinterpret_cast<char*>(&glyph_size), sizeof(int));
	glyphs.resize(glyph_size);
	glyphs.shrink_to_fit();
	file.read(reinterpret_cast<char*>(glyphs.data()), sizeof(ImFontGlyph) * glyph_size);

	float font_size;
	file.read(reinterpret_cast<char*>(&font_size), sizeof(float));

	int out_width;
	int out_height;
	file.read(reinterpret_cast<char*>(&out_width), sizeof(int));
	file.read(reinterpret_cast<char*>(&out_height), sizeof(int));

	std::unique_ptr<unsigned char[]> image_data;
	image_data = std::make_unique<unsigned char[]>(out_width * out_height);
	file.read(reinterpret_cast<char*>(image_data.get()), out_width * out_height);

	// Input Cache
	ImFontAtlas* font_atlas = ImGui::GetIO().Fonts;

	ImFontConfig dummy_config{};
	dummy_config.FontData = new unsigned char;
	dummy_config.FontDataSize = 1;
	dummy_config.SizePixels = 1;
	auto* font = font_atlas->AddFont(&dummy_config);
	font->FontSize = font_size;
	font->ConfigData = &dummy_config;
	font->ConfigDataCount = 1;
	font->ContainerAtlas = font_atlas;

	font_atlas->ClearTexData();
	font_atlas->TexPixelsAlpha8 = image_data.release();
	font_atlas->TexWidth = out_width;
	font_atlas->TexHeight = out_height;
	font_atlas->TexUvWhitePixel = TexUvWhitePixel;
	std::memcpy(font_atlas->TexUvLines, TexUvLines, sizeof(TexUvLines));

	for (auto&& glyph : glyphs) {
		font->AddGlyph(&dummy_config, glyph.Codepoint, glyph.X0, glyph.Y0, glyph.X1, glyph.Y1,
			glyph.U0, glyph.V0, glyph.U1, glyph.V1, glyph.AdvanceX);
		font->SetGlyphVisible(glyph.Codepoint, glyph.Visible);
	}

	font->BuildLookupTable();
	//font_atlas->TexReady = true;

	return true;
}
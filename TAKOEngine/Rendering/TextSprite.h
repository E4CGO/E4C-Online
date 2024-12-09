#pragma once

#include <d3d11.h>
#include <memory>
#include <string>

#include "SimpleMath.h"
#include "SpriteFont.h"

#include "External/DirectXTK12/Inc/SpriteFont.h"
#include "External/DirectXTK12/Inc/SpriteBatch.h"
#include "External/DirectXTK12/Inc/DescriptorHeap.h"
#include "External/DirectXTK12/Inc/ResourceUploadBatch.h"

#include "TAKOEngine/Rendering/RenderContext.h"

enum class FONT_ID
{
	MsGothic,
	HGpop,
	SegoeUI,

	EnumCount
};
enum class FONT_ALIGN
{
	TOP_LEFT,
	TOP,
	TOP_RIGHT,
	LEFT,
	CENTER,
	RIGHT,
	BOTTOM_LEFT,
	BOTTOM,
	BOTTOM_RIGHT
};

class TextSprite
{
private:
	TextSprite() = default;
	~TextSprite();

public:
	// インスタンス
	static TextSprite& Instance()
	{
		static TextSprite instance;
		return instance;
	}

	void Init(ID3D11Device* device);

	void Begin();
	void BeginDX12(const RenderContextDX12& rc);
	void End();
	void EndDX12();
	void Render(
		FONT_ID font,
		const wchar_t* text,
		float x, float y,
		float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
		float angle = 0.0f,
		FONT_ALIGN align = FONT_ALIGN::TOP_LEFT,
		float scale = 1.0f,
		int border = 0,
		const DirectX::XMFLOAT4& borderColor = { 0.0f, 0.0f, 0.0f, 1.0f }
	);
	void Render(
		FONT_ID font,
		const std::wstring& text,
		float x, float y,
		float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
		float angle = 0.0f,
		FONT_ALIGN align = FONT_ALIGN::TOP_LEFT,
		float scale = 1.0f,
		int border = 0,
		const DirectX::XMFLOAT4& borderColor = { 0.0f, 0.0f, 0.0f, 1.0f }
	);
	void Render(
		FONT_ID font,
		const char* text,
		float x, float y,
		float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
		float angle = 0.0f,
		FONT_ALIGN align = FONT_ALIGN::TOP_LEFT,
		float scale = 1.0f,
		int border = 0,
		const DirectX::XMFLOAT4& borderColor = { 0.0f, 0.0f, 0.0f, 1.0f }
	);

	void TextBox(
		FONT_ID font,
		std::string text,
		int linelength,
		float x, float y,
		float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
		float scale = 1.0f
	);

	void RenderDX12(
		FONT_ID font,
		const wchar_t* text,
		float x, float y,
		float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
		float angle = 0.0f,
		FONT_ALIGN align = FONT_ALIGN::TOP_LEFT,
		float scale = 1.0f,
		int border = 0,
		const DirectX::XMFLOAT4& borderColor = { 0.0f, 0.0f, 0.0f, 1.0f }
	);
	void RenderDX12(
		FONT_ID font,
		const std::wstring& text,
		float x, float y,
		float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
		float angle = 0.0f,
		FONT_ALIGN align = FONT_ALIGN::TOP_LEFT,
		float scale = 1.0f,
		int border = 0,
		const DirectX::XMFLOAT4& borderColor = { 0.0f, 0.0f, 0.0f, 1.0f }
	);
	void RenderDX12(
		FONT_ID font,
		const char* text,
		float x, float y,
		float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
		float angle = 0.0f,
		FONT_ALIGN align = FONT_ALIGN::TOP_LEFT,
		float scale = 1.0f,
		int border = 0,
		const DirectX::XMFLOAT4& borderColor = { 0.0f, 0.0f, 0.0f, 1.0f }
	);

	DirectX::XMFLOAT2 TextSize(FONT_ID font, const char* text);
private:
	DirectX::SimpleMath::Vector2 AlignOrig(const wchar_t* text, const FONT_ID& font, FONT_ALIGN align);
private:

	enum Descriptors
	{
		MsGothic,
		HGpop,
		SegoeUI,

		Count
	};

	std::unique_ptr<DirectX::DX11::SpriteFont> fonts[static_cast<int>(FONT_ID::EnumCount)];
	std::unique_ptr<DirectX::DX12::SpriteFont> fontsDX12[static_cast<int>(FONT_ID::EnumCount)];

	//DX12
	std::unique_ptr<DirectX::DescriptorHeap> m_resourceDescriptors;

	std::unique_ptr<DirectX::DX11::SpriteBatch> m_spriteBatch;
	std::unique_ptr<DirectX::DX12::SpriteBatch> m_spriteBatchDX12;
};

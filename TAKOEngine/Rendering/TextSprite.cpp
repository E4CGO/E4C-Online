#include "TextSprite.h"

#include <CommonStates.h>
#include <locale.h>

#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Tool/Encode.h"

void TextSprite::Init(ID3D11Device* device)
{
	fonts[static_cast<int>(FONT_ID::HGpop)] = std::make_unique<DirectX::DX11::SpriteFont>(device, L"Data/Font/HGpop.spritefont");
	fonts[static_cast<int>(FONT_ID::MsGothic)] = std::make_unique<DirectX::DX11::SpriteFont>(device, L"Data/Font/MsGothic.spritefont");
	fonts[static_cast<int>(FONT_ID::SegoeUI)] = std::make_unique<DirectX::DX11::SpriteFont>(device, L"Data/Font/SegoeUI_48.spritefont");

	m_spriteBatch = std::make_unique<DirectX::DX11::SpriteBatch>(T_GRAPHICS.GetDeviceContext());

	////DX12
	m_resourceDescriptors = std::make_unique<DirectX::DescriptorHeap>(T_GRAPHICS.GetDeviceDX12(),
		Descriptors::Count);

	ResourceUploadBatch resourceUpload(T_GRAPHICS.GetDeviceDX12());

	resourceUpload.Begin();

	RenderTargetState rtState(RenderTargetFormat, DepthStencilFormat);

	SpriteBatchPipelineStateDescription pd(rtState);
	m_spriteBatchDX12 = std::make_unique<DirectX::DX12::SpriteBatch>(T_GRAPHICS.GetDeviceDX12(), resourceUpload, pd);

	fontsDX12[static_cast<int>(FONT_ID::HGpop)] = std::make_unique <DirectX::DX12::SpriteFont>(T_GRAPHICS.GetDeviceDX12(), resourceUpload,
		L"Data/Font/HGpop.spritefont",
		m_resourceDescriptors->GetCpuHandle(Descriptors::HGpop),
		m_resourceDescriptors->GetGpuHandle(Descriptors::HGpop));

	fontsDX12[static_cast<int>(FONT_ID::MsGothic)] = std::make_unique <DirectX::DX12::SpriteFont>(T_GRAPHICS.GetDeviceDX12(), resourceUpload,
		L"Data/Font/MsGothic.spritefont",
		m_resourceDescriptors->GetCpuHandle(Descriptors::MsGothic),
		m_resourceDescriptors->GetGpuHandle(Descriptors::MsGothic));

	fontsDX12[static_cast<int>(FONT_ID::SegoeUI)] = std::make_unique <DirectX::DX12::SpriteFont>(T_GRAPHICS.GetDeviceDX12(), resourceUpload,
		L"Data/Font/SegoeUI_48.spritefont",
		m_resourceDescriptors->GetCpuHandle(Descriptors::SegoeUI),
		m_resourceDescriptors->GetGpuHandle(Descriptors::SegoeUI));

	auto viewport = T_GRAPHICS.GetViewport();
	m_spriteBatchDX12->SetViewport(viewport);

	auto uploadResourcesFinished = resourceUpload.End(
		T_GRAPHICS.GetCommandQueue().d3d_command_queue.Get());

	uploadResourcesFinished.wait();
}

TextSprite::~TextSprite()
{
	fonts[static_cast<int>(FONT_ID::HGpop)].reset();
	fonts[static_cast<int>(FONT_ID::MsGothic)].reset();

	fontsDX12[static_cast<int>(FONT_ID::HGpop)].reset();
	fontsDX12[static_cast<int>(FONT_ID::MsGothic)].reset();
	fontsDX12[static_cast<int>(FONT_ID::SegoeUI)].reset();

	m_resourceDescriptors.reset();
}

void TextSprite::Begin()
{
	CommonStates states(T_GRAPHICS.GetDevice());
	m_spriteBatch->Begin(DirectX::DX11::SpriteSortMode_Deferred, states.NonPremultiplied());
}

void TextSprite::BeginDX12(const RenderContextDX12& rc)
{
	m_spriteBatchDX12->Begin(rc.d3d_command_list);
}

void TextSprite::End()
{
	m_spriteBatch->End();
}

void TextSprite::EndDX12()
{
	m_spriteBatchDX12->End();
}

void TextSprite::Render(
	FONT_ID font,
	const wchar_t* text,
	float x, float y,
	float r, float g, float b, float a,
	float angle,
	FONT_ALIGN align,
	float scale,
	int border,
	const DirectX::XMFLOAT4& borderColor
)
{
	// 枠線
	if (border > 0 && borderColor.w > 0.0f) // 透明ではない・太さ0以上
	{
		fonts[static_cast<int>(font)]->DrawString(
			m_spriteBatch.get(),
			text,
			{ x - border, y - border },
			DirectX::XMVECTORF32{ { { borderColor.x, borderColor.y, borderColor.z, borderColor.w } } },
			angle,
			AlignOrig(text, font, align),
			scale
		);
		fonts[static_cast<int>(font)]->DrawString(
			m_spriteBatch.get(),
			text,
			{ x - border, y + border },
			DirectX::XMVECTORF32{ { { borderColor.x, borderColor.y, borderColor.z, borderColor.w } } },
			angle,
			AlignOrig(text, font, align),
			scale
		);
		fonts[static_cast<int>(font)]->DrawString(
			m_spriteBatch.get(),
			text,
			{ x + border, y - border },
			DirectX::XMVECTORF32{ { { borderColor.x, borderColor.y, borderColor.z, borderColor.w } } },
			angle,
			AlignOrig(text, font, align),
			scale
		);
		fonts[static_cast<int>(font)]->DrawString(
			m_spriteBatch.get(),
			text,
			{ x + border, y + border },
			DirectX::XMVECTORF32{ { { borderColor.x, borderColor.y, borderColor.z, borderColor.w } } },
			angle,
			AlignOrig(text, font, align),
			scale
		);
	}

	fonts[static_cast<int>(font)]->DrawString(
		m_spriteBatch.get(),
		text,
		{ x, y },
		DirectX::XMVECTORF32{ { { r, g, b, a } } },
		angle,
		AlignOrig(text, font, align),
		scale
	);
}

void TextSprite::Render(
	FONT_ID font,
	const std::wstring& text,
	float x, float y,
	float r, float g, float b, float a,
	float angle,
	FONT_ALIGN align,
	float scale,
	int border,
	const DirectX::XMFLOAT4& borderColor
)
{
	Render(font, text.c_str(), x, y, r, g, b, a, angle, align, scale, border, borderColor);
}

void TextSprite::Render(
	FONT_ID font,
	const char* text,
	float x, float y,
	float r, float g, float b, float a,
	float angle,
	FONT_ALIGN align,
	float scale,
	int border,
	const DirectX::XMFLOAT4& borderColor
)
{
	Render(font, Encode::string_to_wstring(text), x, y, r, g, b, a, angle, align, scale, border, borderColor);
}

void TextSprite::TextBox(
	FONT_ID font,
	std::string text,
	int linelength,
	float x, float y,
	float r, float g, float b, float a,
	float scale
)
{
	std::wstring t = Encode::string_to_wstring(text);
	// 改行なし
	if (linelength <= 0)
	{
		Render(font, t.c_str(), x, y, r, g, b, a, 0.0f, FONT_ALIGN::TOP_LEFT, scale);
		return;
	}
	int i = 0;
	float insertY = y;

	while (i < t.length())
	{
		int length = min(linelength, static_cast<int>(t.length()) - i);
		std::wstring line = t.substr(i, length).c_str();
		Render(
			font,
			line.c_str(),
			x, insertY,
			r, g, b, a,
			0,
			FONT_ALIGN::TOP_LEFT,
			scale
		);
		i += linelength;
		DirectX::SimpleMath::Vector2 spriteSize = fonts[static_cast<int>(font)]->MeasureString(t.c_str());
		insertY += spriteSize.y * scale;
	}
}

void TextSprite::RenderDX12(FONT_ID font, const wchar_t* text, float x, float y, float r, float g, float b, float a, float angle, FONT_ALIGN align, float scale, int border, const DirectX::XMFLOAT4& borderColor)
{
	fontsDX12[static_cast<int>(font)]->DrawString(
		m_spriteBatchDX12.get(),
		text,
		{ x, y },
		DirectX::XMVECTORF32{ { { r, g, b, a } } },
		angle,
		AlignOrig(text, font, align),
		scale
	);
}

void TextSprite::RenderDX12(
	FONT_ID font,
	const std::wstring& text,
	float x, float y,
	float r, float g, float b, float a,
	float angle,
	FONT_ALIGN align,
	float scale,
	int border,
	const DirectX::XMFLOAT4& borderColor
)
{
	RenderDX12(font, text.c_str(), x, y, r, g, b, a, angle, align, scale, border, borderColor);
}

void TextSprite::RenderDX12(
	FONT_ID font,
	const char* text,
	float x, float y,
	float r, float g, float b, float a,
	float angle,
	FONT_ALIGN align,
	float scale,
	int border,
	const DirectX::XMFLOAT4& borderColor
)
{
	RenderDX12(font, Encode::string_to_wstring(text), x, y, r, g, b, a, angle, align, scale, border, borderColor);
}

DirectX::SimpleMath::Vector2 TextSprite::AlignOrig(const wchar_t* text, const FONT_ID& font, FONT_ALIGN align)
{
	if (align == FONT_ALIGN::TOP_LEFT) return { 0, 0 };

	DirectX::SimpleMath::Vector2 origin = fonts[static_cast<int>(font)]->MeasureString(text);

	switch (align)
	{
	case FONT_ALIGN::TOP:
		origin.x /= 2.0f;
		origin.y = 0.0f;
		break;
	case FONT_ALIGN::TOP_RIGHT:
		origin.y = 0.0f;
		break;
	case FONT_ALIGN::LEFT:
		origin.x = 0.0f;
		origin.y /= 2.0f;
		break;
	case FONT_ALIGN::CENTER:
		origin /= 2.0f;
		break;
	case FONT_ALIGN::RIGHT:
		origin.y /= 2.0f;
		break;
	case FONT_ALIGN::BOTTOM_LEFT:
		origin.x = 0.0f;
		break;
	case FONT_ALIGN::BOTTOM:
		origin.x /= 2.0f;
		break;
	}
	return origin;
}

DirectX::XMFLOAT2 TextSprite::TextSize(FONT_ID font, const char* text)
{
	DirectX::XMFLOAT2 size;
	std::string str = text;
	DirectX::XMStoreFloat2(&size, fonts[static_cast<int>(font)]->MeasureString(Encode::string_to_wstring(text).c_str()));
	return size;
}
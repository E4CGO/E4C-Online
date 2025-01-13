#include "TextSprite.h"

#include <CommonStates.h>
#include <locale.h>

#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Tool/Encode.h"

void TextSprite::Init(ID3D11Device* device)
{
	fonts[static_cast<int>(FONT_ID::HGpop)] = std::make_unique<SpriteFont>(device, L"Data/Font/HGpop.spritefont");
	fonts[static_cast<int>(FONT_ID::MsGothic)] = std::make_unique<SpriteFont>(device, L"Data/Font/MsGothic.spritefont");

	m_spriteBatch = std::make_unique<SpriteBatch>(T_GRAPHICS.GetDeviceContext());

	{
		T_GRAPHICS.GetD2D1DeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_textBrush);
		T_GRAPHICS.GetDWriteFactory()->CreateTextFormat(
			L"Meiryo",
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			32,
			L"ja-jp",
			&m_textFormat
		);
		m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
}

TextSprite::~TextSprite()
{
	fonts[static_cast<int>(FONT_ID::HGpop)].reset();
	fonts[static_cast<int>(FONT_ID::MsGothic)].reset();
}

void TextSprite::Begin()
{
	CommonStates states(T_GRAPHICS.GetDevice());
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states.NonPremultiplied());
}

void TextSprite::BeginDX12()
{
	// Acquire our wrapped render target resource for the current back buffer.
	T_GRAPHICS.GetD3D11On12Device()->AcquireWrappedResources(T_GRAPHICS.GetD3D11BackBuffer(T_GRAPHICS.GetCurrentBufferIndex()).GetAddressOf(), 1);

	// Render text directly to the back buffer.
	T_GRAPHICS.GetD2D1DeviceContext()->SetTarget(T_GRAPHICS.GetD2D1RenderTargets(T_GRAPHICS.GetCurrentBufferIndex()));
}

void TextSprite::End()
{
	m_spriteBatch->End();
}

void TextSprite::EndDX12()
{
	// Release our wrapped render target resource. Releasing
	// transitions the back buffer resource to the state specified
	// as the OutState when the wrapped resource was created.
	T_GRAPHICS.GetD3D11On12Device()->ReleaseWrappedResources(T_GRAPHICS.GetD3D11BackBuffer(T_GRAPHICS.GetCurrentBufferIndex()).GetAddressOf(), 1);
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

void TextSprite::RenderDX12(
	FONT_ID font,
	std::wstring text,
	float x, float y,
	float r, float g, float b, float a,
	float angle,
	FONT_ALIGN align,
	float scale,
	int border,
	const DirectX::XMFLOAT4& borderColor)
{
	DirectX::XMFLOAT2 size;
	DirectX::XMStoreFloat2(&size, fonts[static_cast<int>(font)]->MeasureString(text.c_str()));

	D2D1_RECT_F textRect = D2D1::RectF(x, y, x + size.x, y + size.y);

	m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

	switch (align)
	{
	case FONT_ALIGN::TOP_LEFT:
		break;
	case FONT_ALIGN::TOP:
		break;
	case FONT_ALIGN::TOP_RIGHT:
		break;
	case FONT_ALIGN::LEFT:
		break;
	case FONT_ALIGN::CENTER:
		m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		break;
	case FONT_ALIGN::RIGHT:
		break;
	case FONT_ALIGN::BOTTOM_LEFT:
		break;
	case FONT_ALIGN::BOTTOM:
		break;
	case FONT_ALIGN::BOTTOM_RIGHT:
		break;
	default:
		break;
	}

	T_GRAPHICS.GetD2D1DeviceContext()->BeginDraw();

	D2D1::Matrix3x2F scaleMatrix = D2D1::Matrix3x2F::Scale(
		D2D1_SIZE_F{ scale, scale },  // Scaling factors for X and Y
		D2D1_POINT_2F{ (textRect.right + textRect.left) * 0.5f, (textRect.top + textRect.bottom) * 0.5f } // Center of scaling
	);

	T_GRAPHICS.GetD2D1DeviceContext()->SetTransform(scaleMatrix);

	T_GRAPHICS.GetD2D1DeviceContext()->DrawText(
		text.c_str(),
		text.length(),
		m_textFormat.Get(),
		&textRect,
		m_textBrush.Get()
	);
	T_GRAPHICS.GetD2D1DeviceContext()->SetTransform(D2D1::Matrix3x2F::Identity());

	T_GRAPHICS.GetD2D1DeviceContext()->EndDraw();
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
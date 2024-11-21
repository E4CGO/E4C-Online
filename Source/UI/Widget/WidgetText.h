//! @file WidgetText.h
//! @note

#ifndef __INCLUDED_UI_WIDGET_TEXT__
#define __INCLUDED_UI_WIDGET_TEXT__

#include <string>

#include "TAKOEngine/GUI/Widget.h"
#include "TAKOEngine/Rendering/TextSprite.h"

/**************************************************************************//**
	@class	WidgetText
	@brief	文字ウィジェットクラス
	@par	[説明]
			文字描画用UI
*//***************************************************************************/
class WidgetText : public Widget
{
public:
	// コンストラクタ
	WidgetText() {};
	// コンストラクタ（引数付き）
	WidgetText(const char* text, float scale = 1.0f);

	// 描画処理
	void Render(const RenderContext& rc) override;

	void RenderDX12(const RenderContextDX12& rc) override;

	DirectX::XMFLOAT4 GetColor() { return this->m_color; }

	void SetText(const char* text) { m_str = text; };
	void SetColor(DirectX::XMFLOAT4 color) { this->m_color = color; }
	void SetScale(float scale) { this->m_scale = scale; }
	void SetAngle(float angle) { this->m_angle = angle; }
	void SetFont(FONT_ID font) { this->m_font = font; }
	void SetAlign(FONT_ALIGN align) { this->m_align = align; }
	void SetBorder(int border) { this->m_border = border; }
	void SetBorderColor(DirectX::XMFLOAT4 color) { this->m_borderColor = color; }

	void Show() { m_enable = true; }
	void Hide() { m_enable = false; }
protected:
	std::string m_str;

	DirectX::XMFLOAT4 m_color = { 1, 1, 1, 1 };

	float m_scale = 1.0f;
	float m_angle = 0.0f;
	FONT_ID m_font = FONT_ID::HGpop;
	FONT_ALIGN m_align = FONT_ALIGN::TOP_LEFT;
	int m_border = 0;
	DirectX::XMFLOAT4 m_borderColor = { 0.0f, 0.0f, 0.0f, 1.0f };

	bool m_enable = true;
};

#endif // !__INCLUDED_UI_WIDGET_TEXT__
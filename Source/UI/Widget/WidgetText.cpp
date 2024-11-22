//! @file WidgetText.cpp
//! @note

#include "WidgetText.h"
#include "TAKOEngine/Runtime/tentacle_lib.h"

/**************************************************************************//**
	@brief	コンストラクタ
	@param[in]	text	UI文字
	@param[in]	scale	UIの大きさ
*//***************************************************************************/
WidgetText::WidgetText(const char* text, float scale) : m_str(text), m_scale(scale) {}

/**************************************************************************//**
	@brief		描画処理
	@param[in]	rc	レンダーコンテンツ参照
*//***************************************************************************/
void WidgetText::Render(const RenderContext& rc)
{
	if (!m_enable) return;

	T_TEXT.Render(
		m_font,
		m_str.c_str(),
		m_position.x, m_position.y,
		m_color.x, m_color.y, m_color.z, m_color.w,
		m_angle,
		m_align,
		m_scale,
		m_border,
		m_borderColor
	);
}

void WidgetText::RenderDX12(const RenderContextDX12& rc)
{
}
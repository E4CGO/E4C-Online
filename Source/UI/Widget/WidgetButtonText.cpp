//! @file WidgetButtonText.cpp
//! @note 

#include "WidgetButtonText.h"
#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Tool/XMFLOAT.h"

/**************************************************************************//**
	@brief		コンストラクタ
	@param[in]	text	描画する文字
	@param[in]	f		クリックコールバックラムダ
*//***************************************************************************/
WidgetButtonText::WidgetButtonText(const char* text, std::function<void(WidgetButton*)> f) : m_text(text)
{
	m_size = T_TEXT.TextSize(FONT_ID::HGpop, text) * m_scale;

	SetClick(f);
}

/**************************************************************************//**
	@brief		描画処理
	@param[in]	rc レンダーコンテンツ
	@return		なし
*//***************************************************************************/
void WidgetButtonText::Render(const RenderContext& rc)
{
	T_TEXT.Render(
		FONT_ID::HGpop,
		m_text.c_str(),
		m_position.x, m_position.y,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		FONT_ALIGN::TOP_LEFT,
		m_scale,
		0
	);
}
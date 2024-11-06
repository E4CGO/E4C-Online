//! @file WidgetInputBool.cpp
//! @note 

#include "WidgetInputBool.h"

#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Rendering/ResourceManager.h"

/**************************************************************************//**
 	@brief	コンストラクタ
	@param[in]	label	ラベル
	@param[in]	value	bool値の参照ポインタ
*//***************************************************************************/
WidgetInputBool::WidgetInputBool(const char* label, bool* value) : m_label(label), m_pValue(value)
{
	m_trueImage = RESOURCE.LoadSpriteResource("Data/Sprites/button_agree.png");
	m_falseImage = RESOURCE.LoadSpriteResource("Data/Sprites/button3_ready.png");
};

/**************************************************************************//**
 	@brief		描画処理
	@param[in]	rc レンダーコンテンツ
	@return		なし
*//***************************************************************************/
void WidgetInputBool::Render(const RenderContext& rc)
{
	DirectX::XMFLOAT2 checkboxSize = { m_size.y, m_size.y };
	DirectX::XMFLOAT2 LabelSize = T_TEXT.TextSize(FONT_ID::HGpop, m_label.c_str());

	T_TEXT.Render(
		FONT_ID::HGpop,
		m_label.c_str(),
		m_position.x, m_position.y + m_size.y / 2.0f,
		1, 1, 1, 1,
		0.0f,
		FONT_ALIGN::LEFT,
		0.5f,
		1
	);

	if (*m_pValue)
	{
		//True
		m_trueImage->Render(
			rc.deviceContext,
			m_position.x + m_size.x - checkboxSize.x, m_position.y, 0,
			checkboxSize.x, checkboxSize.y
		);
	}
	else
	{
		//False
		m_falseImage->Render(
			rc.deviceContext,
			m_position.x + m_size.x - checkboxSize.x, m_position.y, 0,
			checkboxSize.x, checkboxSize.y
		);
	}
}
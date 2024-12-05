//! @file WidgetDragFloat.cpp
//! @note

#include "WidgetDragFloat.h"

#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Rendering/ResourceManager.h"

/**************************************************************************//**
	@brief		コンストラクタ
	@param[in]	rate	初期値%
*//***************************************************************************/
WidgetDragBar::WidgetDragBar(float rate) : m_rate(rate)
{
	if (T_GRAPHICS.isDX11Active)
	{
		m_scrollBar = RESOURCE.LoadSpriteResource("Data/Sprites/button_frame.png");
		m_scrollBtn = RESOURCE.LoadSpriteResource("Data/Sprites/button2_ready_on.png");
	}
	else
	{
		m_scrollBarDX12 = RESOURCE.LoadSpriteResourceDX12("Data/Sprites/button_frame.png");
		m_scrollBtnDX12 = RESOURCE.LoadSpriteResourceDX12("Data/Sprites/button2_ready_on.png");
	}
}

/**************************************************************************//**
	@brief		推し続ける処理
	@param[in]	なし
	@return		なし
*//***************************************************************************/
void WidgetDragBar::OnPress()
{
	DirectX::XMFLOAT2 mousePos = T_INPUT.GetMouseWinPos();
	m_rate = (mousePos.x - m_position.x) / (m_size.x);
	T_INPUT.SetCursorWinPositionY(m_position.y + m_size.y * 0.5f);
}
/**************************************************************************//**
	@brief		描画処理
	@param[in]	rc	レンダーコンテンツ
	@return		なし
*//***************************************************************************/
void WidgetDragBar::Render(const RenderContext& rc)
{
	m_scrollBar->Render(
		rc.deviceContext,
		m_position.x, m_position.y + m_size.y * 0.25f, 0,
		m_size.x, m_size.y * 0.5f
	);

	m_scrollBtn->Render(
		rc.deviceContext,
		m_position.x - m_size.x * 0.025f + m_size.x * m_rate, m_position.y, 0,
		m_size.x * 0.05f, m_size.y
	);
}
/**************************************************************************//**
	@brief		描画処理
	@param[in]	rc	レンダーコンテンツ
	@return		なし
*//***************************************************************************/
void WidgetDragBar::RenderDX12(const RenderContextDX12& rc)
{
	m_scrollBarDX12->Begin(rc);
	m_scrollBarDX12->Draw(
		m_position.x, m_position.y + m_size.y * 0.25f,
		m_size.x, m_size.y * 0.5f,
		0,
		1, 1, 1, 1);
	m_scrollBarDX12->End(rc.d3d_command_list);

	m_scrollBtnDX12->Begin(rc);
	m_scrollBtnDX12->Draw(
		m_position.x - m_size.x * 0.025f + m_size.x * m_rate, m_position.y, 
		m_size.x * 0.05f, m_size.y,
		0,
		1, 1, 1, 1);
	m_scrollBtnDX12->End(rc.d3d_command_list);
}
/**************************************************************************//**
	@brief		コンストラクタ
	@param[in]	label		ラベル
	@param[in]	value		float値参照ポインタ
	@param[in]	minValue	最大値
	@param[in]	maxValue	最小値
*//***************************************************************************/
WidgetDragFloat::WidgetDragFloat(const char* label, float* value, float minValue, float maxValue) : m_label(label), m_pValue(value), m_minValue(minValue), m_maxValue(maxValue)
{
	m_pBar = new WidgetDragBar((*value - minValue) / (maxValue - minValue));
}

/**************************************************************************//**
	@brief		位置を設定
	@param[in]	position	位置
	@return		なし
*//***************************************************************************/
void WidgetDragFloat::SetPosition(const DirectX::XMFLOAT2& position)
{
	Widget::SetPosition(position);
	m_pBar->SetPosition(position + DirectX::XMFLOAT2{ 0.0f,  m_size.y * 0.6f });
}
/**************************************************************************//**
	@brief		サイズ設定
	@param[in]	size	サイズ
	@return		なし
*//***************************************************************************/
void WidgetDragFloat::SetSize(const DirectX::XMFLOAT2& size)
{
	Widget::SetSize(size);
	m_pBar->SetSize({ size.x, size.y * 0.3f });
}
/**************************************************************************//**
	@brief		更新処理
	@param[in]	elapsedTime	経過時間
	@return		なし
*//***************************************************************************/
void WidgetDragFloat::Update(float elapsedTime)
{
	m_pBar->Update(elapsedTime);

	*this->m_pValue = m_minValue + (m_maxValue - m_minValue) * m_pBar->GetRate();
}
/**************************************************************************//**
	@brief		描画処理
	@param[in]	rc	レンダーコンテンツ
	@return		なし
*//***************************************************************************/
void WidgetDragFloat::Render(const RenderContext& rc)
{
	T_TEXT.Render(
		FONT_ID::HGpop,
		m_label.c_str(),
		m_position.x, m_position.y,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		FONT_ALIGN::TOP_LEFT,
		0.5f,
		1
	);
	m_pBar->Render(rc);
}

/**************************************************************************//**
	@brief		描画処理
	@param[in]	rc	レンダーコンテンツ
	@return		なし
*//***************************************************************************/
void WidgetDragFloat::RenderDX12(const RenderContextDX12& rc)
{
	m_pBar->RenderDX12(rc);
}
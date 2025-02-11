//! @file WidgetUpDown.cpp
//! @note

#include "WidgetUpDown.h"
#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Rendering/ResourceManager.h"

/**************************************************************************//**
	 @brief		画僧と変数設定
	@param[in]    value		値
	@param[in]    minValue	最低値
	@param[in]    maxValue	最大値
*//***************************************************************************/
WidgetArrowLeft::WidgetArrowLeft(uint8_t* value, uint8_t minValue, uint8_t maxValue)
{
	btn = RESOURCE.LoadSpriteResource("Data/Sprites/UI/Character/left_button.png");
	btnDX12 = RESOURCE.LoadSpriteResourceDX12("Data/Sprites/UI/Character/right_button.png");
	this->value = value;
	this->minValue = minValue;
	this->maxValue = maxValue;
}

/**************************************************************************//**
	 @brief		レンダリング関数
	@param[in]    rc	RenderContext
*//***************************************************************************/
void WidgetArrowLeft::Render(const RenderContext& rc)
{
	btn->Render(
		rc.deviceContext,
		m_position.x, m_position.y, 0,
		m_size.x, m_size.y
	);
}

void WidgetArrowLeft::RenderDX12(const RenderContextDX12& rc)
{
	btnDX12->Begin(rc);
	btnDX12->Draw(
		m_position.x, m_position.y,
		m_size.x, m_size.y,
		0,
		1.0f, 1.0f, 1.0f, 1.0f
	);
	btnDX12->End(rc.d3d_command_list);
}

/**************************************************************************//**
	 @brief	値を変更関数
*//***************************************************************************/
void WidgetArrowLeft::OnClick()
{
	if (*value == 0)
	{
		*value = maxValue;
		return;
	}

	(*value)--;
	if (*value < minValue) *value = maxValue;
}

/**************************************************************************//**
	 @brief		画僧と変数設定
	@param[in]    value		値
	@param[in]    minValue	最低値
	@param[in]    maxValue	最大値
*//***************************************************************************/
WidgetArrowRight::WidgetArrowRight(uint8_t* value, uint8_t minValue, uint8_t maxValue)
{
	btn = RESOURCE.LoadSpriteResource("Data/Sprites/UI/keybinds/key_Right.png");
	btnDX12 = RESOURCE.LoadSpriteResourceDX12("Data/Sprites/UI/keybinds/key_Right.png");
	this->value = value;
	this->minValue = minValue;
	this->maxValue = maxValue;
}

/**************************************************************************//**
	 @brief		レンダリング関数
	@param[in]    rc	RenderContext
*//***************************************************************************/
void WidgetArrowRight::Render(const RenderContext& rc)
{
	btn->Render(
		rc.deviceContext,
		m_position.x, m_position.y, 0,
		m_size.x, m_size.y
	);
}

/**************************************************************************//**
	 @brief		レンダリング関数
	@param[in]    rc	RenderContext
*//***************************************************************************/
void WidgetArrowRight::RenderDX12(const RenderContextDX12& rc)
{
	btnDX12->Begin(rc);
	btnDX12->Draw(
		m_position.x, m_position.y,
		m_size.x, m_size.y,
		0,
		1.0f, 1.0f, 1.0f, 1.0f
	);
	btnDX12->End(rc.d3d_command_list);
}

/**************************************************************************//**
	 @brief	値を変更関数
*//***************************************************************************/
void WidgetArrowRight::OnClick()
{
	if (*value == 255)
	{
		*value = minValue;
		return;
	}
	(*value)++;
	if (*value > maxValue) *value = minValue;
}

/**************************************************************************//**
	@brief		コンストラクタ
	@param[in]	label		名前
	@param[in]	uvalue	値
	@param[in]	minValue	最低値
	@param[in]	maxValue	最大値
*//***************************************************************************/
WidgetUpDown::WidgetUpDown(const char* label, uint8_t* uvalue, uint8_t minValue, uint8_t maxValue) : label(label), uvalue(uvalue)
{
	m_ArrowLeft = new WidgetArrowLeft(uvalue, minValue, maxValue);
	m_ArrowRight = new WidgetArrowRight(uvalue, minValue, maxValue);
}

/**************************************************************************//**
	 @brief			位置設定関数
	@param[in]    position
*//***************************************************************************/
void WidgetUpDown::SetPosition(const DirectX::XMFLOAT2& position)
{
	Widget::SetPosition(position);
	m_ArrowLeft->SetPosition(position + DirectX::XMFLOAT2{ 0.0f,  m_size.y });
	m_ArrowRight->SetPosition(position + DirectX::XMFLOAT2{ m_size.x,  m_size.y });
}

/**************************************************************************//**
	@brief		サイズ設定関数
	@param[in]    size
*//***************************************************************************/
void WidgetUpDown::SetSize(const DirectX::XMFLOAT2& size)
{
	Widget::SetSize(size);
	m_ArrowLeft->SetSize({ size.x, size.y });
	m_ArrowRight->SetSize({ size.x, size.y });
}

/**************************************************************************//**
	@brief		アップデート関数
	@param[in]    elapsedTime
*//***************************************************************************/
void WidgetUpDown::Update(float elapsedTime)
{
	m_ArrowLeft->Update(elapsedTime);
	m_ArrowRight->Update(elapsedTime);

	this->uvalue = m_ArrowLeft->GetValue();
}

/**************************************************************************//**
	@brief		レンダリング関数
	@param[in]    rc
*//***************************************************************************/
void WidgetUpDown::Render(const RenderContext& rc)
{
	T_TEXT.Render(
		FONT_ID::HGpop,
		label.c_str(),
		m_position.x, m_position.y,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		FONT_ALIGN::TOP_LEFT,
		0.5f,
		1
	);
	m_ArrowLeft->Render(rc);
	m_ArrowRight->Render(rc);
}

/**************************************************************************//**
	@brief		レンダリング関数
	@param[in]    rc
*//***************************************************************************/
void WidgetUpDown::RenderDX12(const RenderContextDX12& rc)
{
	m_ArrowLeft->RenderDX12(rc);
	m_ArrowRight->RenderDX12(rc);
}
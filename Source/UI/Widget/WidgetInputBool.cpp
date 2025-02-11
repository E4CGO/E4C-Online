﻿//! @file WidgetInputBool.cpp
//! @note

#include "WidgetInputBool.h"

#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Rendering/ResourceManager.h"
#include "TAKOEngine/Tool/Encode.h"

/**************************************************************************//**
	@brief	コンストラクタ
	@param[in]	label	ラベル
	@param[in]	value	bool値の参照ポインタ
*//***************************************************************************/
WidgetInputBool::WidgetInputBool(const char* label, bool* value) : m_label(label), m_pValue(value)
{
	if (T_GRAPHICS.isDX11Active)
	{
		m_trueImage = RESOURCE.LoadSpriteResource("Data/Sprites/UI/Game/slider button.png");
		m_falseImage = RESOURCE.LoadSpriteResource("Data/Sprites/UI/Game/check box.png");
	}
	else
	{
		m_trueImageDX12 = RESOURCE.LoadSpriteResourceDX12("Data/Sprites/UI/Game/slider button.png");
		m_falseImageDX12 = RESOURCE.LoadSpriteResourceDX12("Data/Sprites/UI/Game/check box.png");
	}
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

/**************************************************************************//**
	@brief		描画処理
	@param[in]	rc レンダーコンテンツ
	@return		なし
*//***************************************************************************/
void WidgetInputBool::RenderDX12(const RenderContextDX12& rc)
{
	DirectX::XMFLOAT2 checkboxSize = { m_size.y, m_size.y };

	T_TEXT.RenderDX12(
		FONT_ID::HGpop,
		Encode::string_to_wstring(m_label),
		m_position.x, m_position.y + m_size.y / 2.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		FONT_ALIGN::TOP_LEFT,
		0.5f,
		1);

	if (*m_pValue)
	{
		// True
		m_trueImageDX12->Begin(rc);
		m_trueImageDX12->Draw(
			m_position.x + m_size.x - checkboxSize.x, m_position.y,
			checkboxSize.x, checkboxSize.y,
			0,
			1, 1, 1, 1);
		m_trueImageDX12->End(rc.d3d_command_list);
	}
	else
	{
		// False
		m_falseImageDX12->Begin(rc);
		m_falseImageDX12->Draw(
			m_position.x + m_size.x - checkboxSize.x, m_position.y,
			checkboxSize.x, checkboxSize.y,
			0,
			1, 1, 1, 1);
		m_falseImageDX12->End(rc.d3d_command_list);
	}
}
//! @file WidgetButtonImage.cpp
//! @note

#include "WidgetButtonImage.h"

#include "TAKOEngine/Rendering/ResourceManager.h"
#include "TAKOEngine/Tool/XMFLOAT.h"
#include "TAKOEngine/Runtime/tentacle_lib.h"

/**************************************************************************//**
	@brief		コンストラクタ
	@param[in]	text		ボタン文字
	@param[in]	image		ボタン画像
	@param[in]	hoverImage	ボタンホバー画像
	@param[in]	f			クリックコールバックラムダ
*//***************************************************************************/
WidgetButtonImage::WidgetButtonImage(const char* text, const char* image, const char* hoverImage, std::function<void(WidgetButton*)> f)
{
	if (T_GRAPHICS.isDX11Active)
	{
		m_btnImage = RESOURCE.LoadSpriteResource(image);
		m_hoverBtnImage = RESOURCE.LoadSpriteResource(hoverImage);
	}
	else
	{
		m_btnImageDX12 = RESOURCE.LoadSpriteResourceDX12(image);
		m_hoverBtnImageDX12 = RESOURCE.LoadSpriteResourceDX12(hoverImage);
	}

	m_size = m_btnImage->GetTextureSize();

	this->m_text = std::make_unique<WidgetText>(text);
	this->m_text->SetAlign(FONT_ALIGN::CENTER);
	this->m_text->SetFont(FONT_ID::HGpop);
	this->m_text->SetBorder(1);

	SetClick(f);
}

/**************************************************************************//**
	@brief		コンストラクタ(ホバーなし)
	@param[in]	text		ボタン文字
	@param[in]	image		ボタン画像
	@param[in]	f			クリックコールバックラムダ
*//***************************************************************************/
WidgetButtonImage::WidgetButtonImage(const char* text, const char* image, std::function<void(WidgetButton*)> f) : WidgetButtonImage(text, image, image, f) {}

void WidgetButtonImage::Render(const RenderContext& rc)
{
	if (isHover)
	{
		this->m_hoverBtnImage->Render(
			rc.deviceContext,
			m_position.x, m_position.y, 0,
			m_size.x, m_size.y,
			0,
			m_color.x, m_color.y, m_color.z, m_color.w
		);
	}
	else
	{
		this->m_btnImage->Render(
			rc.deviceContext,
			m_position.x, m_position.y, 0,
			m_size.x, m_size.y,
			0,
			m_color.x, m_color.y, m_color.z, m_color.w
		);
	}

	this->m_text->SetPosition(m_position + (m_size * 0.5f));
	this->m_text->Render(rc);
}

void WidgetButtonImage::RenderDX12(const RenderContextDX12& rc)
{
	if (isHover)
	{
		this->m_hoverBtnImageDX12->Begin(rc);
		this->m_hoverBtnImageDX12->Draw(
			m_position.x, m_position.y,
			m_size.x, m_size.y,
			0,
			m_color.x, m_color.y, m_color.z, m_color.w
		);
		this->m_hoverBtnImageDX12->End(rc.d3d_command_list);
	}
	else
	{
		this->m_btnImageDX12->Begin(rc);
		this->m_btnImageDX12->Draw(
			m_position.x, m_position.y,
			m_size.x, m_size.y,
			0,
			m_color.x, m_color.y, m_color.z, m_color.w
		);
		this->m_btnImageDX12->End(rc.d3d_command_list);
	}

	this->m_text->SetPosition(m_position + (m_size * 0.5f));
	this->m_text->RenderDX12(rc);
}
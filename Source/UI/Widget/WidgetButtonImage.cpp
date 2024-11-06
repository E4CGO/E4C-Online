//! @file WidgetButtonImage.cpp
//! @note 

#include "WidgetButtonImage.h"

#include "TAKOEngine/Rendering/ResourceManager.h"
#include "TAKOEngine/Tool/XMFLOAT.h"

/**************************************************************************//**
 	@brief		コンストラクタ
	@param[in]	text		ボタン文字
	@param[in]	image		ボタン画像
	@param[in]	hoverImage	ボタンホバー画像
	@param[in]	f			クリックコールバックラムダ
*//***************************************************************************/
WidgetButtonImage::WidgetButtonImage(const char* text, const char* image, const char* hoverImage, std::function<void(WidgetButton*)> f)
{
	m_btnImage = RESOURCE.LoadSpriteResource(image);
	m_hoverBtnImage = RESOURCE.LoadSpriteResource(hoverImage);
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
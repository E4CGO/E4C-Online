//! @file WidgetImage.cpp
//! @note 

#include "WidgetImage.h"
#include "TAKOEngine/Rendering/ResourceManager.h"

/**************************************************************************//**
 	@brief		コンストラクタ
	@param[in]	filename ファイル名前
*//***************************************************************************/
WidgetImage::WidgetImage(const char* filename)
{
	m_pSprite = RESOURCE.LoadSpriteResource(filename);
}

/**************************************************************************//**
 	@brief		描画処理
	@param[in]	rc
*//***************************************************************************/
void WidgetImage::Render(const RenderContext& rc)
{
	m_pSprite->Render(
		rc.deviceContext,
		m_position.x, m_position.y, 0,
		m_size.x, m_size.y,
		m_angle,
		m_color.x, m_color.y, m_color.z, m_color.w
	);
}
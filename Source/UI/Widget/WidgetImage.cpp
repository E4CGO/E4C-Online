//! @file WidgetImage.cpp
//! @note

#include "WidgetImage.h"
#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Rendering/ResourceManager.h"

/**************************************************************************//**
	@brief		コンストラクタ
	@param[in]	filename ファイル名前
*//***************************************************************************/
WidgetImage::WidgetImage(const char* filename)
{
	if (T_GRAPHICS.isDX11Active) m_pSprite = RESOURCE.LoadSpriteResource(filename);
	else m_pSpriteDX12 = RESOURCE.LoadSpriteResourceDX12(filename);
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

/**************************************************************************//**
	@brief		描画処理
	@param[in]	rc
*//***************************************************************************/
void WidgetImage::RenderDX12(const RenderContextDX12& rc)
{
	m_pSpriteDX12->Begin(rc);
	m_pSpriteDX12->Draw(
		m_position.x, m_position.y,
		m_size.x, m_size.y,
		m_angle,
		m_color.x, m_color.y, m_color.z, m_color.w);
	m_pSpriteDX12->End(rc.d3d_command_list);
}
//! @file WidgetStripeAnimation.cpp
//! @note

#include "WidgetStripeAnimation.h"
#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Rendering/ResourceManager.h"

/**************************************************************************//**
	@brief		コンストラクタ
	@param[in]	filename ファイル名前
*//***************************************************************************/
WidgetStripeAnimation::WidgetStripeAnimation(const char* filename,
	float posX,
	float posY,
	float width,
	float height,
	float texturePosX,
	float texturePosY,
	float textureWidth,
	float textureHeight,
	int totalFrames,
	float frameDuration) : m_posX(posX), m_posY(posY), m_width(width), m_height(height),
	m_texturePosX(texturePosX), m_texturePosY(texturePosY), m_textureWidth(textureWidth), m_textureHeight(textureHeight),
	m_totalFrames(totalFrames), m_frameDuration(frameDuration)
{
	if (T_GRAPHICS.isDX11Active)
	{
		m_pSprite = RESOURCE.LoadSpriteResource(filename);
		m_size = m_pSprite->GetTextureSize();
	}
	else
	{
		m_pSpriteDX12 = RESOURCE.LoadSpriteResourceDX12(filename);
		m_size = m_pSpriteDX12->GetTextureSize();
	}
}

void WidgetStripeAnimation::Update(float elapsedTime)
{
	if (m_elapsedTime >= m_frameDuration)
	{
		m_elapsedTime -= m_frameDuration;
		m_currentFrame = (m_currentFrame + 1) % m_totalFrames;
	}

	m_elapsedTime += elapsedTime;
}

/**************************************************************************//**
	@brief		描画処理
	@param[in]	rc
*//***************************************************************************/
void WidgetStripeAnimation::Render(const RenderContext& rc)
{
	m_pSprite->Render(
		rc.deviceContext,
		m_posX, m_posY, 0, m_width, m_height,
		m_texturePosX * m_currentFrame, m_texturePosY, m_textureWidth, m_textureHeight,
		m_angle,
		m_color.x, m_color.y, m_color.z, m_color.w
	);
}

/**************************************************************************//**
	@brief		描画処理
	@param[in]	rc
*//***************************************************************************/
void WidgetStripeAnimation::RenderDX12(const RenderContextDX12& rc)
{
	m_pSpriteDX12->Begin(rc);
	m_pSpriteDX12->Draw(m_posX, m_posY, m_width, m_height,
		m_texturePosX * m_currentFrame, m_texturePosY, m_textureWidth, m_textureHeight,
		m_angle, m_color.x, m_color.y, m_color.z, m_color.w);
	m_pSpriteDX12->End(rc.d3d_command_list);
}
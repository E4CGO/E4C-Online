//! @file WidgetStripeAnimation.h
//! @note

#ifndef __INCLUDED_WIDGET_STRIPE_ANIMATION__
#define __INCLUDED_WIDGET_STRIPE_ANIMATION__

#include <memory>

#include "TAKOEngine/Rendering/Sprite.h"
#include "TAKOEngine/GUI/Widget.h"

/**************************************************************************//**
	@class	WidgetStripeAnimation
	@brief	画像アニメーション描画用ウェジット
	@par	[説明]
*//***************************************************************************/
class WidgetStripeAnimation : public Widget
{
public:
	// コンストラクタ
	WidgetStripeAnimation(const char* filename,
		float posX,
		float posY,
		float width,
		float height,
		float texturePosX,
		float texturePosY,
		float textureWidth,
		float textureHight,
		int totalFrames,
		float frameDuration);

	void Update(float elapsedTime) override;

	// 描画処理
	void Render(const RenderContext& rc) override;

	void RenderDX12(const RenderContextDX12& rc) override;

	// 色を取得
	DirectX::XMFLOAT4 GetColor() { return this->m_color; }
	// 色を設定
	void SetColor(DirectX::XMFLOAT4 color) { this->m_color = color; }
protected:
	// 画像スプライト
	std::shared_ptr<Sprite> m_pSprite;
	std::shared_ptr<SpriteDX12> m_pSpriteDX12;
	// 角度
	float m_angle = 0;
	// 色
	DirectX::XMFLOAT4 m_color = { 1, 1, 1, 1 };

	int m_currentFrame = 0;
	int m_totalFrames = 0;
	float m_elapsedTime = 0.0f;
	float m_frameDuration = 0.1f;

	float m_posX;
	float m_posY;
	float m_width;
	float m_height;
	float m_texturePosX;
	float m_texturePosY;
	float m_textureHeight;
	float m_textureWidth;
};
#endif // !__INCLUDED_WIDGET_STRIPE_ANIMATION__

//! @file WidgetImage.h
//! @note

#ifndef __INCLUDE_WIDGET_IMAGE__
#define __INCLUDE_WIDGET_IMAGE__

#include <memory>

#include "TAKOEngine/Rendering/Sprite.h"
#include "TAKOEngine/GUI/Widget.h"

/**************************************************************************//**
	@class	WidgetImage
	@brief	画像描画用ウェジット
	@par	[説明]
*//***************************************************************************/
class WidgetImage : public Widget
{
public:
	// コンストラクタ
	WidgetImage(const char* filename);

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
	// 角度
	float m_angle = 0;
	// 色
	DirectX::XMFLOAT4 m_color = { 1, 1, 1, 1 };
};
#endif // !__INCLUDE_WIDGET_IMAGE__

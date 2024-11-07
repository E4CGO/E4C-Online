//! @file WidgetButtonImage.h
//! @note 

#ifndef __INCLUDE_WIDGET_BUTTON_IMAGE__
#define __INCLUDE_WIDGET_BUTTON_IMAGE__

#include <memory>

#include "TAKOEngine/Rendering/Sprite.h"

#include "UI/Widget/WidgetButton.h"
#include "UI/Widget/WidgetText.h"

/**************************************************************************//**
	@class	WidgetButtonImage
	@brief	画像ボタンクラス
	@par	[説明]
		クリックができる画像UI
*//***************************************************************************/
class WidgetButtonImage : public WidgetButton
{
public:
	// コンストラクタ(ホバー引数付き)
	WidgetButtonImage(const char* text, const char* image, const char* hoverImage, std::function<void(WidgetButton*)> f);
	// コンストラクタ(引数付き）
	WidgetButtonImage(const char* text, const char* image, std::function<void(WidgetButton*)> f);
	// デストラクタ
	~WidgetButtonImage() {};

	// ボタンの文字を取得
	WidgetText* GetText() { return this->m_text.get(); }

	// 描画処理
	virtual void Render(const RenderContext& rc);
	// 画像色を設定
	void SetColor(const DirectX::XMFLOAT4& color) { this->m_color = color; }
protected:
	// 色
	DirectX::XMFLOAT4 m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	// ボタン画像
	std::shared_ptr<Sprite> m_btnImage;
	// ボタンホバー画像
	std::shared_ptr<Sprite> m_hoverBtnImage;
	// ボタン文字
	std::unique_ptr<WidgetText> m_text;
};
#endif // !__INCLUDE_WIDGET_BUTTON_IMAGE__

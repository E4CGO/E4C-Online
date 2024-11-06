//! @file WidgetButtonText.h
//! @note 

#ifndef __INCLUDED_WIDGET_BUTTON_TEXT__
#define __INCLUDED_WIDGET_BUTTON_TEXT__

#include <string>
#include "WidgetButton.h"

/**************************************************************************//**
	@class	WidgetButtonText
	@brief	文字ボタンUIウェジットクラス
	@par	[説明]
		画像の代わりに文字を描画するボタン
*//***************************************************************************/
class WidgetButtonText : public WidgetButton
{
public:
	// コンストラクタ
	WidgetButtonText(const char* text, std::function<void(WidgetButton*)> f);
	// デストラクタ
	~WidgetButtonText() {};
	// 描画処理
	virtual void Render(const RenderContext& rc);
protected:
	// 文字
	std::string m_text;
	// サイズ
	float m_scale = 0.5f;
};
#endif // !__INCLUDED_WIDGET_BUTTON_TEXT__

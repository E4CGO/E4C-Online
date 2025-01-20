//! @file WidgetButton.h
//! @note 

#ifndef __INCLUDE_WIDGET_BUTTON_H__
#define __INCLUDE_WIDGET_BUTTON_H__

#include "Widget.h"

#include <functional>

/**************************************************************************//**
	@class	WidgetButton
	@brief	ボタン基底クラス
	@par	[説明]
*//***************************************************************************/
class WidgetButton : public Widget
{
public:
	// コンストラクタ
	WidgetButton() {};
	// デストラクタ
	~WidgetButton() {};

	// 更新処理
	virtual void Update(float elapsedTime);

	// ボバー処理
	virtual void OnHover() { if (hoverFunction) hoverFunction(this); };
	virtual void OnUnhover() { if (unhoverFunction) unhoverFunction(this); };
	// クリック処理
	virtual void OnClick() { if (clickFunction) clickFunction(this); };
	// 推し続け処理
	virtual void OnPress() { if (pressFunction) pressFunction(this); };

	// ホバーイベント処理を設定
	void SetHover(std::function<void(WidgetButton*)> f) { hoverFunction = f; }
	// ホバー解放イベント処理
	void SetUnhover(std::function<void(WidgetButton*)> f) { unhoverFunction = f; }
	// クリック処理
	void SetClick(std::function<void(WidgetButton*)> f) { clickFunction = f; }
	// 推し続け処理
	void SetPress(std::function<void(WidgetButton*)> f) { pressFunction = f; }

	bool IsHover() { return isHover; }
	void SetEnable(bool enable = true) { this->enable = enable; }
protected:
	// 有効化フラグ
	bool enable = true;
	// ホバーフラグ
	bool isHover = false;

	// ホバーイベントラムダ
	std::function<void(WidgetButton*)> hoverFunction = nullptr;
	// ホバー解放イベントラムダ
	std::function<void(WidgetButton*)> unhoverFunction = nullptr;
	// クリックイベントラムダ
	std::function<void(WidgetButton*)> clickFunction = nullptr;
	// 推し続けイベントラムダ
	std::function<void(WidgetButton*)> pressFunction = nullptr;
};
#endif // !__INCLUDE_WIDGET_BUTTON_H__

//! @file WidgetCrosshair.h
//! @note 

#ifndef __INCLUDED_WIDGET_CROSSHAIR__
#define __INCLUDED_WIDGET_CROSSHAIR__

#include "WidgetImage.h"

/**************************************************************************//**
	@class	WidgetCrossHair
	@brief	クロスヘア用ウェジット
	@par	[説明]
*//***************************************************************************/
class WidgetCrosshair : public WidgetImage
{
public:
	// コンストラクタ
	WidgetCrosshair();

	// 更新処理
	void Update(float elapsedTime) override;

	// 表示
	void Show() { this->m_enable = true; }
	// 非表示
	void Hide() { this->m_enable = false; }

	// インスタンス取得
	static WidgetCrosshair& Instance();
private:
	bool m_enable = false;
};

#define CROSS_HAIR WidgetCrosshair::Instance()

#endif // !__INCLUDED_WIDGET_CROSSHAIR__


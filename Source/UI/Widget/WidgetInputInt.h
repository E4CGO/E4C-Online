//! @file WidgetInputInt.h
//! @note 


#ifndef __INCLUDED_WIDGET_INPUT_INT_H__
#define __INCLUDED_WIDGET_INPUT_INT_H__

#include "WidgetButtonImage.h"

/**************************************************************************//**
	@class	WidgetInputInt
	@brief	整数調整UIクラス
	@par	[説明]
*//***************************************************************************/
template <typename T>
class WidgetInputInt : public Widget
{
public:
	WidgetInputInt<T>(T* integer, T min, T max, bool loop = true);
	~WidgetInputInt();

	// 更新処理
	void Update(float elapsedTime);
	// 描画処理
	void Render(const RenderContext& rc);

	void RenderDX12(const RenderContextDX12& rc);
private:
	T* m_pInt = nullptr;
	T m_min;
	T m_max;
	bool m_loopFlag;
	WidgetButtonImage* m_pBtnLeft;
	WidgetButtonImage* m_pBtnRight;
};

#endif // !__INCLUDED_WIDGET_INPUT_INT_H__

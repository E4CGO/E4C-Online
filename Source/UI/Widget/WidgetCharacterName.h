//! @file WidgetCharacterName.h
//! @note 

#ifndef __INCLUDED_WIDGET_CHARACTER_NAME_H__
#define __INCLUDED_WIDGET_CHARACTER_NAME_H__

#include "TAKOEngine/GUI/Widget.h"

class WidgetCharacterName : public Widget
{
public:
	WidgetCharacterName() {};
	~WidgetCharacterName() {};

	// 更新処理
	void Update(float elapsedTime) {};
	// 描画処理
	void Render(const RenderContext& rc);

	void RenderDX12(const RenderContextDX12& rc);
};

#endif // !__INCLUDED_WIDGET_CHARACTER_NAME_H__






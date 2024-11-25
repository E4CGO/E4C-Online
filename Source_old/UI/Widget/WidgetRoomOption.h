#pragma once

#include <string>

#include "TAKOEngine/GUI/Widget.h"

class WidgetRoomOption : public Widget
{
public:
	WidgetRoomOption();
	~WidgetRoomOption() {}

	virtual void Render(const RenderContext& rc);

	virtual void RenderDX12(const RenderContextDX12& rc);
private:
	void NameWithToken();
private:
	// 入力
	char address[32];
	char name[32];
	char port[16];
};

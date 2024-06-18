#pragma once

#include <string>

#include "TAKOEngine/GUI/Widget.h"

class WidgetRoomOption : public Widget
{
public:
	WidgetRoomOption();
	~WidgetRoomOption() {}

	virtual void Render(const RenderContext& rc);
private:
	void NameWithToken();
private:
	// “ü—Í
	char address[32];
	char name[32];
	char port[16];
};

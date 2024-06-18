#pragma once

#include <string>
#include "WidgetButton.h"

class WidgetButtonText : public WidgetButton
{
public:
	WidgetButtonText(const char* text, std::function<void(WidgetButton*)> f);
	~WidgetButtonText() {};

	virtual void Render(const RenderContext& rc);
protected:
	std::string text;
	float scale = 0.5f;
};

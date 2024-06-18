#include "WidgetButtonText.h"

#include "TAKOEngine/Runtime/tentacle_lib.h"

#include "TAKOEngine/Tool/XMFLOAT.h"

WidgetButtonText::WidgetButtonText(const char* text, std::function<void(WidgetButton*)> f) : text(text)
{
	size = T_TEXT.TextSize(FONT_ID::HGpop, text) * scale;

	SetClick(f);
}

void WidgetButtonText::Render(const RenderContext& rc)
{
	T_TEXT.Render(
		FONT_ID::HGpop,
		text.c_str(),
		position.x, position.y,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		FONT_ALIGN::TOP_LEFT,
		scale,
		1
	);
}
#include "TAKOEngine/Runtime/tentacle_lib.h"

#include "WidgetText.h"

void WidgetText::Render(const RenderContext& rc)
{
	if (!enable) return;

	T_TEXT.Render(
		font,
		str.c_str(),
		position.x, position.y,
		color.x, color.y, color.z, color.w,
		angle,
		align,
		scale,
		border,
		borderColor
	);
}
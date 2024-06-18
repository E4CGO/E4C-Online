#include "WidgetInputBool.h"

#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Rendering/ResourceManager.h"

WidgetInputBool::WidgetInputBool(const char* label, bool* value) : label(label), value(value)
{
	trueImage = RESOURCE.LoadSpriteResource("Data/Sprites/button_agree.png");
	falseImage = RESOURCE.LoadSpriteResource("Data/Sprites/button3_ready.png");
};

void WidgetInputBool::Render(const RenderContext& rc)
{
	DirectX::XMFLOAT2 checkboxSize = { size.y, size.y };
	DirectX::XMFLOAT2 LabelSize = T_TEXT.TextSize(FONT_ID::HGpop, label.c_str());

	T_TEXT.Render(
		FONT_ID::HGpop,
		label.c_str(),
		position.x, position.y + size.y / 2.0f,
		1, 1, 1, 1,
		0.0f,
		FONT_ALIGN::LEFT,
		0.5f,
		1
	);

	if (*value)
	{
		//True
		trueImage->Render(
			rc.deviceContext,
			position.x + size.x - checkboxSize.x, position.y, 0,
			checkboxSize.x, checkboxSize.y
		);
	}
	else
	{
		//False
		falseImage->Render(
			rc.deviceContext,
			position.x + size.x - checkboxSize.x, position.y, 0,
			checkboxSize.x, checkboxSize.y
		);
	}
}
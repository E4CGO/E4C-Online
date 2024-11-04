#include "WidgetDragFloat.h"

#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Rendering/ResourceManager.h"

WidgetDragBar::WidgetDragBar(float rate) : rate(rate)
{
	scrollBar = RESOURCE.LoadSpriteResource("Data/Sprites/button_frame.png");
	scrollBtn = RESOURCE.LoadSpriteResource("Data/Sprites/button2_ready_on.png");
}

void WidgetDragBar::OnPress()
{
	DirectX::XMFLOAT2 mousePos = T_INPUT.GetMouseWinPos();
	rate = (mousePos.x - position.x) / (size.x);
	T_INPUT.SetCursorWinPositionY(position.y + size.y * 0.5f);
}

void WidgetDragBar::Render(const RenderContext& rc)
{
	scrollBar->Render(
		rc.deviceContext,
		position.x, position.y + size.y * 0.25f, 0,
		size.x, size.y * 0.5f
	);

	scrollBtn->Render(
		rc.deviceContext,
		position.x - size.x * 0.025f + size.x * rate, position.y, 0,
		size.x * 0.05f, size.y
	);
}

WidgetDragFloat::WidgetDragFloat(const char* label, float* value, float minValue, float maxValue) : label(label), value(value), minValue(minValue), maxValue(maxValue)
{
	bar = new WidgetDragBar((*value - minValue) / (maxValue - minValue));
}

WidgetDragFloat::WidgetDragFloat(const char* label, uint8_t* uvalue, float minValue, float maxValue) : label(label), uvalue(uvalue), minValue(minValue), maxValue(maxValue)
{
	bar = new WidgetDragBar((*uvalue - minValue) / (maxValue - minValue));
}

void WidgetDragFloat::SetPosition(const DirectX::XMFLOAT2& position)
{
	Widget::SetPosition(position);
	bar->SetPosition(position + DirectX::XMFLOAT2{ 0.0f,  size.y * 0.6f });
}
void WidgetDragFloat::SetSize(const DirectX::XMFLOAT2& size)
{
	Widget::SetSize(size);
	bar->SetSize({ size.x, size.y * 0.3f });
}

void WidgetDragFloat::Update(float elapsedTime)
{
	bar->Update(elapsedTime);
	if (value != nullptr)
	{
		*this->value = minValue + (maxValue - minValue) * bar->GetRate();
	}
	else
	{
		newrate = bar->GetRate();

		*this->uvalue = minValue + (maxValue - minValue) * newrate;
	}
}

void WidgetDragFloat::Render(const RenderContext& rc)
{
	T_TEXT.Render(
		FONT_ID::HGpop,
		label.c_str(),
		position.x, position.y,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		FONT_ALIGN::TOP_LEFT,
		0.5f,
		1
	);
	bar->Render(rc);
}
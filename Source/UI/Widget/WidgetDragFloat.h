#pragma once

#include <string>
#include <memory>

#include "TAKOEngine/Rendering/Sprite.h"

#include "UI/Widget/WidgetButton.h"

class WidgetDragBar : public WidgetButton
{
public:
	WidgetDragBar(float rate = 0.0f);
	~WidgetDragBar() {};

	void Render(const RenderContext& rc) override;

	void OnPress() override;
	float GetRate() { return rate; };
	float SetRate(float rate) { this->rate = rate; }
private:
	float rate;

	std::shared_ptr<Sprite> scrollBar;
	std::shared_ptr<Sprite> scrollBtn;
};

class WidgetDragFloat : public Widget
{
public:
	WidgetDragFloat(const char* label, float* value, float minValue, float maxValue);
	WidgetDragFloat(const char* label, uint8_t* uvalue, float minValue, float maxValue);
	~WidgetDragFloat() { delete bar; };

	void Update(float elapsedTime) override;
	void Render(const RenderContext& rc) override;

	void SetPosition(const DirectX::XMFLOAT2& position) override;
	void SetSize(const DirectX::XMFLOAT2& size) override;
private:
	std::string label;
	float* value;
	uint8_t* uvalue;
	float newrate;
	float minValue;
	float maxValue;
	WidgetDragBar* bar;
};

#pragma once

#include <memory>
#include <string>

#include "TAKOEngine/Rendering/Sprite.h"

#include "UI/Widget/WidgetButton.h"

class WidgetInputBool : public WidgetButton
{
public:
	WidgetInputBool(const char* label, bool* value);
	~WidgetInputBool() {};

	void OnClick() override { *(this->value) = !*(this->value); };

	void Render(const RenderContext& rc) override;
private:
	std::shared_ptr<Sprite> trueImage;
	std::shared_ptr<Sprite> falseImage;

	std::string label;
	bool* value;
};

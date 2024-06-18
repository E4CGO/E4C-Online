#pragma once

#include "TAKOEngine/GUI/Widget.h"

#include <functional>

class WidgetButton : public Widget
{
public:
	WidgetButton() {};
	~WidgetButton() {};

	virtual void Update(float elapsedTime);

	virtual void OnHover() { if (hoverFunction) hoverFunction(this); };
	virtual void OnUnhover() { if (unhoverFunction) unhoverFunction(this); };
	virtual void OnClick() { if (clickFunction) clickFunction(this); };
	virtual void OnPress() { if (pressFunction) pressFunction(this); };

	void SetHover(std::function<void(WidgetButton*)> f) { hoverFunction = f; }
	void SetUnhover(std::function<void(WidgetButton*)> f) { unhoverFunction = f; }
	void SetClick(std::function<void(WidgetButton*)> f) { clickFunction = f; }
	void SetPress(std::function<void(WidgetButton*)> f) { pressFunction = f; }

	bool IsHover() { return isHover; }
	void SetEnable(bool enable = true) { this->enable = enable; }
protected:
	bool enable = true;
	bool isHover = false;

	std::function<void(WidgetButton*)> hoverFunction = nullptr;
	std::function<void(WidgetButton*)> unhoverFunction = nullptr;
	std::function<void(WidgetButton*)> clickFunction = nullptr;
	std::function<void(WidgetButton*)> pressFunction = nullptr;
};

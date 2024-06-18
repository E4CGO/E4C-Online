#pragma once

#include <memory>
#include <vector>

#include "TAKOEngine/Rendering/Sprite.h"

#include "UI/Widget/WidgetSettingWindow.h"
#include "UI/Widget/WidgetButton.h"

class WidgetMenu : public Widget
{
public:
	WidgetMenu();
	~WidgetMenu();

	void Update(float elapsedTime);
	void Render(const RenderContext& rc);

	bool IsShow() { return enable; }
	void Show();
	void Hide();
private:
	DirectX::XMFLOAT2 GetOptionsSize();
private:
	int selectIdx = 0;
	bool enable = false;
	float moveSpeed = 0.3f; // èoåªÅEâBÇ∑ïbêî
	std::shared_ptr<Sprite> background;

	std::vector<WidgetButton*> options;
	WidgetSettingWindow* settingWindow = nullptr;
};

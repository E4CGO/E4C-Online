#pragma once

#include <memory>
#include <vector>

#include "TAKOEngine/Rendering/Sprite.h"
#include "TAKOEngine/GUI/Widget.h"

#include "UI/Widget/WidgetButton.h"
#include "UI/Widget/WidgetImage.h"
#include "UI/Widget/WidgetText.h"
#include "UI/Widget/WidgetButtonImage.h"

#include "GameData.h"

class WidgetSettingWindow : public Widget
{
public:
	WidgetSettingWindow();
	~WidgetSettingWindow();

	void Update(float elapsedTime) override;
	void Render(const RenderContext& rc)override;

	bool IsEnd() { return isEnd; }
private:
	bool isEnd = false; // Š®Œ‹ƒtƒ‰ƒO

	GameData::GameSetting cacheGameSetting;

	std::shared_ptr<Sprite> background;

	WidgetButton* closeBtn;
	WidgetButton* saveBtn;

	std::vector<Widget*> OptionsList;
};

//! @file WidgetSettingWindow.h
//! @note 

#ifndef __INCLUDED_WIDGET_SETTING_WINDOW__
#define __INCLUDED_WIDGET_SETTING_WINDOW__

#include <memory>
#include <vector>

#include "TAKOEngine/Rendering/Sprite.h"
#include "TAKOEngine/GUI/Widget.h"

#include "UI/Widget/WidgetButton.h"
#include "UI/Widget/WidgetImage.h"
#include "UI/Widget/WidgetText.h"
#include "UI/Widget/WidgetButtonImage.h"

#include "GameData.h"

/**************************************************************************//**
	@class	WidgetSettingWindow
	@brief	設定ウィンドウズウィジェットクラス
	@par	[説明]
		設定画面
*//***************************************************************************/
class WidgetSettingWindow : public Widget
{
public:
	WidgetSettingWindow();
	~WidgetSettingWindow();

	void Update(float elapsedTime) override;
	void Render(const RenderContext& rc)override;

	bool IsEnd() { return isEnd; }
private:
	bool isEnd = false; // 完結フラグ

	GameData::GameSetting cacheGameSetting;

	std::shared_ptr<Sprite> background;

	WidgetButton* closeBtn;
	WidgetButton* saveBtn;

	std::vector<Widget*> OptionsList;
};


#endif // !__INCLUDED_WIDGET_SETTING_WINDOW__

//! @file WidgetMenu.h
//! @note 

#ifndef __INCLUDE_WIDGET_MENU__
#define __INCLUDE_WIDGET_MENU__



#include <memory>
#include <vector>

#include "TAKOEngine/Rendering/Sprite.h"

#include "UI/Widget/WidgetSettingWindow.h"
#include "UI/Widget/WidgetButton.h"

/**************************************************************************//**
	@class	WidgetMenu
	@brief	UIメニューウェジット
	@par	[説明]
		シーンゲームを呼び出すUI
*//***************************************************************************/
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
	float moveSpeed = 0.3f; // 出現・隠す秒数
	std::shared_ptr<Sprite> background;

	std::vector<WidgetButton*> options;
	WidgetSettingWindow* settingWindow = nullptr;
};
#endif // !__INCLUDE_WIDGET_MENU__

//! @file WidgetPauseMenu.h
//! @note

#ifndef __INCLUDED_PAUSE_MENU_WINDOW__
#define __INCLUDED_PAUSE_MENU_WINDOW__

#include <memory>
#include <vector>

#include "TAKOEngine/Rendering/Sprite.h"
#include "TAKOEngine/GUI/Widget.h"

#include "UI/Widget/WidgetButton.h"
#include "UI/Widget/WidgetImage.h"
#include "UI/Widget/WidgetText.h"
#include "UI/Widget/WidgetButtonImage.h"
#include "UI/Widget/WidgetSettingWindow.h"

#include "GameData.h"

/**************************************************************************//**
	@class	WidgetPauseMenu
	@brief	設定ウィンドウズウィジェットクラス
	@par	[説明]
		ポーズ画面
*//***************************************************************************/
class WidgetPauseMenu : public Widget
{
public:
	WidgetPauseMenu();
	virtual~WidgetPauseMenu() = default;

	void Update(float elapsedTime) override;
	void Render(const RenderContext& rc)override;

	void RenderDX12(const RenderContextDX12& rc) override;

	bool IsActive() { return isActive; }
private:
	bool isActive = false; // 完結フラグ
	bool isSettingsActive = false;

	std::shared_ptr<Sprite> background;
	std::shared_ptr<SpriteDX12> backgroundDX12;

	WidgetButton* m_pauseButton;

	WidgetButton* m_resumeButton;
	WidgetButton* m_toTitleButton;
	WidgetButton* m_settingsButton;
	WidgetButton* m_exitButton;

	WidgetSettingWindow* m_settingsWindow = nullptr;
};

#endif // !__INCLUDED_PAUSE_MENU_WINDOW__
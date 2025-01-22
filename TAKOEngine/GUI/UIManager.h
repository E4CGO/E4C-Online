//! @file UIManager.h
//! @note 

#ifndef __INCLUDE_UI_MANAGER_H__
#define __INCLUDE_UI_MANAGER_H__

#include "TAKOEngine/Tool/Singleton.h"
#include "TAKOEngine/GUI/Widget.h"

#include "GameObject/ObjectManager.h"

#include "WidgetButton.h"

/**************************************************************************//**
	@class	UIManager
	@brief	UI管理クラス
	@par	[説明]
*//***************************************************************************/
class UIManager : public ObjectManager<Widget>, public Singleton<UIManager>
{
	friend class Singleton<UIManager>;
protected:
	UIManager() = default;
	~UIManager() = default;

public:
	// 更新処理
	void Update(float elapsedTime) override;

	void SetCursorName(LPWSTR lpCursorName);

	void SetClickedWidget(WidgetButton* widget) { m_pClickedWidget = widget; }
	void SetPressedWidget(WidgetButton* widget) { m_pPressedWidget = widget; }
private:
	LPWSTR lpCursorName;

	// 最後にクリックしたUI
	WidgetButton* m_pClickedWidget = nullptr;
	// 最後に押したUI
	WidgetButton* m_pPressedWidget = nullptr;
};

#define UI UIManager::Instance()

#endif // !__INCLUDE_UI_MANAGER_H__

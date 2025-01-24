//! @file UIManager.cpp
//! @note 

#include "UIManager.h"

/**************************************************************************//**
 	@brief		更新処理
	@param[in]	elapsedTime	経過時間
*//***************************************************************************/
void UIManager::Update(float elapsedTime)
{
	this->lpCursorName = IDC_ARROW; // デフォルト

	m_pPressedWidget = nullptr;
	m_pClickedWidget = nullptr;

	ObjectManager::Update(elapsedTime);

	if (m_pClickedWidget != nullptr)
	{
		m_pClickedWidget->OnClick();
	} else if (m_pPressedWidget != nullptr)
	{
		m_pPressedWidget->OnPress();
	}

	SetCursor(::LoadCursor(NULL, this->lpCursorName));
}
/**************************************************************************//**
 	@brief		カーソル設定
	@param[in]	lpCursorName
*//***************************************************************************/
void UIManager::SetCursorName(LPWSTR lpCursorName)
{
	this->lpCursorName = lpCursorName;
}
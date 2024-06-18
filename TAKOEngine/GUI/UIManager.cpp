#include "UIManager.h"

void UIManager::Update(float elapsedTime)
{
	this->lpCursorName = IDC_ARROW; // デフォルト
	ObjectManager::Update(elapsedTime);
	SetCursor(::LoadCursor(NULL, this->lpCursorName));
}

void UIManager::SetCursorName(LPWSTR lpCursorName)
{
	this->lpCursorName = lpCursorName;
}
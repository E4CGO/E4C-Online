#include "WidgetButton.h"

#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/GUI/UIManager.h"

void WidgetButton::Update(float elapsedTime)
{
	if (enable)
	{
		const DirectX::XMFLOAT2& mousePos = T_INPUT.GetMouseWinPos();
		if (
			mousePos.x >= position.x &&
			mousePos.x <= position.x + size.x &&
			mousePos.y >= position.y &&
			mousePos.y <= position.y + size.y
			)
		{
			if (!isHover) OnHover();
			isHover = true;
			UI.SetCursorName(IDC_HAND);
		}
		else
		{
			if (isHover) OnUnhover();
			isHover = false;
		}
	}
	else
	{
		if (isHover) OnUnhover();
		isHover = false;
	}

	if (isHover)
	{
		if (T_INPUT.KeyPress(VK_LBUTTON))
		{
			OnPress();
		}
		if (T_INPUT.KeyDown(VK_LBUTTON))
		{
			OnClick();
		}
	}
}
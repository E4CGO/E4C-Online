//! @file WidgetButton.cpp
//! @note 

#include "WidgetButton.h"
#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/GUI/UIManager.h"


/**************************************************************************//**
 	@brief		更新処理
	@param[in]	elapsedTime	経過時間
	@return		なし
*//***************************************************************************/
void WidgetButton::Update(float elapsedTime)
{
	if (enable)
	{
		const DirectX::XMFLOAT2& mousePos = T_INPUT.GetMouseWinPos();
		if (
			mousePos.x >= m_position.x &&
			mousePos.x <= m_position.x + m_size.x &&
			mousePos.y >= m_position.y &&
			mousePos.y <= m_position.y + m_size.y
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
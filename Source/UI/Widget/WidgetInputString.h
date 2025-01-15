//! @file WidgetInputInt.h
//! @note 

#ifndef __INCLUDED_WIDGET_INPUT_STRING_H__
#define __INCLUDED_WIDGET_INPUT_STRING_H__

#include <string>
#include "WidgetButtonImage.h"

/**************************************************************************//**
	@class	WidgetInputString
	@brief	文字列調整UIクラス
	@par	[説明]
*//***************************************************************************/
class WidgetInputString : public Widget
{
public:
	WidgetInputString(const char* label, std::string* string, uint32_t limit = 10.0f);
	~WidgetInputString();

	// 更新処理
	void Update(float elapsedTime);
	// 描画処理
	void Render(const RenderContext& rc);

	void RenderDX12(const RenderContextDX12& rc);
private:
	std::string m_label;			// IMGUI用ラベル
	std::string* m_pString;
	//std::vector<char> buffer;
	//uint32_t m_limit;
	size_t m_strSize = 0;
};

#endif // !__INCLUDED_WIDGET_INPUT_STRING_H__

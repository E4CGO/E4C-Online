//! @file WidgetInputBool.h
//! @note

#ifndef __INCLUDED_WIDGET_INPUT_BOOL__
#define __INCLUDED_WIDGET_INPUT_BOOL__

#include <memory>
#include <string>

#include "TAKOEngine/Rendering/Sprite.h"

#include "TAKOEngine/GUI/WidgetButton.h"

/**************************************************************************//**
	@class	WidgetInputBool
	@brief	Bool値を管理用UI
	@par	[説明]
		Bool用のGUI
*//***************************************************************************/
class WidgetInputBool : public WidgetButton
{
public:
	// コンストラクタ
	WidgetInputBool(const char* label, bool* value);
	// デストラクタ
	~WidgetInputBool() {};

	// クリック
	void OnClick() override { *(this->m_pValue) = !*(this->m_pValue); };

	// 描画処理
	void Render(const RenderContext& rc) override;
	void RenderDX12(const RenderContextDX12& rc) override;
private:
	// TRUE画像
	std::shared_ptr<Sprite> m_trueImage;
	std::shared_ptr<SpriteDX12> m_trueImageDX12;
	// FALSE画像
	std::shared_ptr<Sprite> m_falseImage;
	std::shared_ptr<SpriteDX12> m_falseImageDX12;
	// UIラベル
	std::string m_label;
	// bool値参照ポインタ
	bool* m_pValue;
};
#endif // !__INCLUDED_WIDGET_INPUT_BOOL__

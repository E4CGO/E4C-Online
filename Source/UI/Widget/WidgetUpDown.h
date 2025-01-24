//! @file WidgetUpDown.h
//! @note ステップで値を変わるUIコントロールクラス

#ifndef __WIDGET_UP_DOWN_H__
#define __WIDGET_UP_DOWN_H__

#include <string>
#include <memory>

#include "TAKOEngine/Rendering/Sprite.h"

#include "TAKOEngine/GUI/WidgetButton.h"

/**************************************************************************//**
	@class	WidgetArrowLeft
	@brief	左側コントロール
	@par    [説明]
*//***************************************************************************/
class WidgetArrowLeft : public WidgetButton
{
public:
	WidgetArrowLeft(uint8_t* value, uint8_t minValue, uint8_t maxValue);
	~WidgetArrowLeft() {};

	void Render(const RenderContext& rc) override;
	void RenderDX12(const RenderContextDX12& rc) override;

	void OnClick() override;

	uint8_t* GetValue() { return value; }
private:
	uint8_t* value;
	uint8_t minValue;
	uint8_t maxValue;

	std::shared_ptr<Sprite> btn;
	std::shared_ptr<SpriteDX12> btnDX12;
};

/**************************************************************************//**
	@class	WidgetArrowRight
	@brief	右側コントロール
	@par    [説明]
*//***************************************************************************/
class WidgetArrowRight : public WidgetButton
{
public:
	WidgetArrowRight(uint8_t* value, uint8_t minValue, uint8_t maxValue);
	~WidgetArrowRight() {};

	void Render(const RenderContext& rc) override;

	void RenderDX12(const RenderContextDX12& rc);

	void OnClick() override;

	uint8_t* GetValue() { return value; }
private:
	uint8_t* value;
	uint8_t minValue;
	uint8_t maxValue;
	std::shared_ptr<Sprite> btn;
	std::shared_ptr<SpriteDX12> btnDX12;
};

/**************************************************************************//**
	@class	WidgetUpDown
	@brief	全体値をチェンジコントロール
	@par    [説明]
*//***************************************************************************/
class WidgetUpDown : public Widget
{
public:
	WidgetUpDown(const char* label, uint8_t* uvalue, uint8_t minValue = 0, uint8_t maxValue = 1);
	~WidgetUpDown() { delete m_ArrowLeft; delete m_ArrowRight; };

	void Update(float elapsedTime) override;
	void Render(const RenderContext& rc) override;
	void RenderDX12(const RenderContextDX12& rc) override;

	void SetPosition(const DirectX::XMFLOAT2& position) override;
	void SetSize(const DirectX::XMFLOAT2& size) override;
private:
	std::string label;
	uint8_t* uvalue;
	uint8_t* minValue;
	uint8_t* maxValue;
	WidgetArrowLeft* m_ArrowLeft;
	WidgetArrowRight* m_ArrowRight;
};

#endif //__WIDGET_UP_DOWN_H__

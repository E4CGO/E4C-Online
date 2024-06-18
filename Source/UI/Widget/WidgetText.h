#pragma once

#include <string>

#include "TAKOEngine/GUI/Widget.h"
#include "TAKOEngine/Rendering/TextSprite.h"

class WidgetText : public Widget
{
public:
	WidgetText() {};
	WidgetText(const char* text, float scale = 1.0f) : str(text), scale(scale) {};

	void Render(const RenderContext& rc) override;

	DirectX::XMFLOAT4 GetColor() { return this->color; }

	void SetText(const char* text) { str = text; };
	void SetColor(DirectX::XMFLOAT4 color) { this->color = color; }
	void SetScale(float scale) { this->scale = scale; }
	void SetAngle(float angle) { this->angle = angle; }
	void SetFont(FONT_ID font) { this->font = font; }
	void SetAlign(FONT_ALIGN align) { this->align = align; }
	void SetBorder(int border) { this->border = border; }
	void SetBorderColor(DirectX::XMFLOAT4 color) { this->borderColor = color; }

	void Show() { enable = true; }
	void Hide() { enable = false; }
protected:
	std::string str;

	DirectX::XMFLOAT4 color = { 1, 1, 1, 1 };

	float scale = 1.0f;
	float angle = 0.0f;
	FONT_ID font = FONT_ID::HGpop;
	FONT_ALIGN align = FONT_ALIGN::TOP_LEFT;
	int border = 0;
	DirectX::XMFLOAT4 borderColor = { 0.0f, 0.0f, 0.0f, 1.0f };

	bool enable = true;
};

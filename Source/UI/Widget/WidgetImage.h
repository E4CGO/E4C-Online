#pragma once

#include <memory>

#include "TAKOEngine/Rendering/Sprite.h"
#include "TAKOEngine/GUI/Widget.h"

class WidgetImage : public Widget
{
public:
	WidgetImage(const char* filename);

	void Render(const RenderContext& rc) override;

	DirectX::XMFLOAT4 GetColor() { return this->color; }
	void SetColor(DirectX::XMFLOAT4 color) { this->color = color; }
protected:
	std::shared_ptr<Sprite> sprite;

	float angle = 0;
	DirectX::XMFLOAT4 color = { 1, 1, 1, 1 };
};

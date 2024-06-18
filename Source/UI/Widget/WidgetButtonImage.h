#pragma once

#include <memory>
#include <string>

#include "TAKOEngine/Rendering/Sprite.h"

#include "UI/Widget/WidgetButton.h"
#include "UI/Widget/WidgetText.h"

class WidgetButtonImage : public WidgetButton
{
public:
	WidgetButtonImage(const char* text, const char* image, const char* hoverImage, std::function<void(WidgetButton*)> f);
	WidgetButtonImage(const char* text, const char* image, std::function<void(WidgetButton*)> f);
	~WidgetButtonImage() {};

	WidgetText* GetText() { return this->text.get(); }

	virtual void Render(const RenderContext& rc);
	void SetColor(const DirectX::XMFLOAT4& color) { this->color = color; }
protected:
	DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	std::shared_ptr<Sprite> btnImage;
	std::shared_ptr<Sprite> hoverBtnImage;
	std::unique_ptr<WidgetText> text;
};

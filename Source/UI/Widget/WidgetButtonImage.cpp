#include "WidgetButtonImage.h"

#include "TAKOEngine/Rendering/ResourceManager.h"
#include "TAKOEngine/Tool/XMFLOAT.h"

WidgetButtonImage::WidgetButtonImage(const char* text, const char* image, const char* hoverImage, std::function<void(WidgetButton*)> f)
{
	btnImage = RESOURCE.LoadSpriteResource(image);
	hoverBtnImage = RESOURCE.LoadSpriteResource(hoverImage);
	size = btnImage->GetTextureSize();

	this->text = std::make_unique<WidgetText>(text);
	this->text->SetAlign(FONT_ALIGN::CENTER);
	this->text->SetFont(FONT_ID::HGpop);
	this->text->SetBorder(1);

	SetClick(f);
}

WidgetButtonImage::WidgetButtonImage(const char* text, const char* image, std::function<void(WidgetButton*)> f) : WidgetButtonImage(text, image, image, f) {}

void WidgetButtonImage::Render(const RenderContext& rc)
{
	if (isHover)
	{
		this->hoverBtnImage->Render(
			rc.deviceContext,
			position.x, position.y, 0,
			size.x, size.y,
			0,
			color.x, color.y, color.z, color.w
		);
	}
	else
	{
		this->btnImage->Render(
			rc.deviceContext,
			position.x, position.y, 0,
			size.x, size.y,
			0,
			color.x, color.y, color.z, color.w
		);
	}

	this->text->SetPosition(position + (size * 0.5f));
	this->text->Render(rc);
}
#include "WidgetImage.h"

#include "TAKOEngine/Rendering/ResourceManager.h"

WidgetImage::WidgetImage(const char* filename)
{
	sprite = RESOURCE.LoadSpriteResource(filename);
}

void WidgetImage::Render(const RenderContext& rc)
{
	sprite->Render(
		rc.deviceContext,
		position.x, position.y, 0,
		size.x, size.y,
		angle,
		color.x, color.y, color.z, color.w
	);
}
#pragma once

#include <memory>

#include "TAKOEngine/Network/HttpRequest.h"
#include "TAKOEngine/Rendering/Sprite.h"

#include "UI/Widget/WidgetButtonImage.h"
#include "UI/Widget/WidgetButtonText.h"

class WidgetGem : public WidgetButtonImage
{
public:
	WidgetGem();
	~WidgetGem() {};

	void Update(float elapsedTime) override;
	void Render(const RenderContext& rc) override;
	void RenderDX12(const RenderContextDX12& rc) override;
public:
private:
	std::shared_ptr<Sprite> gem;
	WidgetButtonText* chargeBtn;
	HttpRequest* xhr = nullptr;
};

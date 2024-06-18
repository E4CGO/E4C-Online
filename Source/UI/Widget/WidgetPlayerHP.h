#pragma once

#include <memory>

#include "TAKOEngine/GUI/Widget.h"

#include "GameObject/Character/Player/Player.h"

class WidgetPlayerHP : public Widget
{
public:
	WidgetPlayerHP();

	void Update(float elapsedTime) override;
	void Render(const RenderContext& rc) override;
private:
	Player* player = nullptr;
	std::shared_ptr<Sprite> gauge;
	std::shared_ptr<Sprite> gaugeFrame;

	// •`‰æ—p
	float tempHp;
};

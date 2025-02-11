﻿#pragma once

#include "TAKOEngine/GUI/Widget.h"

#include <string>
#include "GameObject/Character/Enemy/Enemy.h"

class WidgetBossHp : public Widget
{
public:
	WidgetBossHp(const char* name, Enemy* enemy);
	~WidgetBossHp() {}

	void Update(float elapsedTime) override;
	void Render(const RenderContext& rc) override;
	void RenderDX12(const RenderContextDX12& rc) override;
private:
	Enemy* enemy = nullptr;
	std::string name;
	std::shared_ptr<Sprite> gauge;
	std::shared_ptr<Sprite> gaugeFrame;

	std::shared_ptr<SpriteDX12> gaugeDX12;
	std::shared_ptr<SpriteDX12> gaugeFrameDX12;

	// 描画用
	float tempHp = 0;
};

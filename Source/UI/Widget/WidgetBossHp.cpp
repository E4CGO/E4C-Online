#include "WidgetBossHp.h"

#include "TAKOEngine/GUI/UIManager.h"
#include "TAKOEngine/Rendering/ResourceManager.h"

WidgetBossHp::WidgetBossHp(const char* name, Enemy* enemy) : name(name), enemy(enemy)
{
	gauge = RESOURCE.LoadSpriteResource("Data/Sprites/Gauge/Hp_line_white.png");
	gaugeFrame = RESOURCE.LoadSpriteResource("Data/Sprites/Gauge/Hp_frame.png");

	size = gaugeFrame->GetTextureSize();
	position = { SCREEN_W / 2.0f - size.x / 2.0f, SCREEN_H * 0.8f };

	tempHp = 0;
}

void WidgetBossHp::Update(float elapsedTime)
{
	// tempHP更新
	float hp = static_cast<float>(enemy->GetHp());
	if (hp <= 0)
	{
		UI.Remove(this);
		return;
	}

	if (hp > tempHp) // 回復
	{
		tempHp += min(elapsedTime * max(30.0f, hp - tempHp), hp - tempHp);
	}
	else if (hp < tempHp) // ダメージ
	{
		tempHp -= min(elapsedTime * max(10.0f, tempHp - hp), tempHp - hp);
	}
}
void WidgetBossHp::Render(const RenderContext& rc)
{
	// HPゲージ
	float maxHp = static_cast<float>(enemy->GetMaxHp());
	float tempHpRate = tempHp / maxHp;
	gauge->Render(
		rc.deviceContext,
		position.x, position.y, 0,
		size.x * tempHpRate, size.y,
		0, 0,
		1024.0f * tempHpRate, 64.0f,
		0,
		1.0f, 1.0f, 1.0f, 1.0f
	);

	float hpRate = static_cast<float>(enemy->GetHp()) / static_cast<float>(enemy->GetMaxHp());
	hpRate = min(hpRate, tempHpRate);
	gauge->Render(
		rc.deviceContext,
		position.x, position.y, 0,
		size.x * hpRate, size.y,
		0, 0,
		1024.0f * hpRate, 64.0f,
		0,
		1.0f, 0.1f, 0.1f, 1.0f
	);

	gaugeFrame->Render(
		rc.deviceContext,
		position.x, position.y, 0,
		size.x, size.y
	);

	// ボス名前
	T_TEXT.Render(
		FONT_ID::HGpop,
		name.c_str(),
		position.x, position.y,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		FONT_ALIGN::BOTTOM_LEFT,
		0.8f,
		1
	);
}
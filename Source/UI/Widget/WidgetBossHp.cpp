#include "WidgetBossHp.h"

#include "TAKOEngine/GUI/UIManager.h"
#include "TAKOEngine/Rendering/ResourceManager.h"

WidgetBossHp::WidgetBossHp(const char* name, Enemy* enemy) : name(name), enemy(enemy)
{
	if (T_GRAPHICS.isDX11Active)
	{
		gauge = RESOURCE.LoadSpriteResource("Data/Sprites/Gauge/Hp_line_white.png");
		gaugeFrame = RESOURCE.LoadSpriteResource("Data/Sprites/Gauge/Hp_frame.png");
		m_size = gaugeFrame->GetTextureSize();
		m_position = { SCREEN_W / 2.0f - m_size.x / 2.0f, SCREEN_H * 0.8f };
	}
	else
	{
		gaugeDX12 = RESOURCE.LoadSpriteResourceDX12("Data/Sprites/Gauge/Hp_line_white.png");
		gaugeFrameDX12 = RESOURCE.LoadSpriteResourceDX12("Data/Sprites/Gauge/Hp_frame.png");
		m_size = gaugeFrameDX12->GetTextureSize();
		m_position = { SCREEN_W / 2.0f - m_size.x / 2.0f, SCREEN_H * 0.8f };
	}

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
		m_position.x, m_position.y, 0,
		m_size.x * tempHpRate, m_size.y,
		0, 0,
		1024.0f * tempHpRate, 64.0f,
		0,
		1.0f, 1.0f, 1.0f, 1.0f
	);

	float hpRate = static_cast<float>(enemy->GetHp()) / static_cast<float>(enemy->GetMaxHp());
	hpRate = min(hpRate, tempHpRate);
	gauge->Render(
		rc.deviceContext,
		m_position.x, m_position.y, 0,
		m_size.x * hpRate, m_size.y,
		0, 0,
		1024.0f * hpRate, 64.0f,
		0,
		1.0f, 0.1f, 0.1f, 1.0f
	);

	gaugeFrame->Render(
		rc.deviceContext,
		m_position.x, m_position.y, 0,
		m_size.x, m_size.y
	);

	// ボス名前
	T_TEXT.Render(
		FONT_ID::HGpop,
		name.c_str(),
		m_position.x, m_position.y,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		FONT_ALIGN::BOTTOM_LEFT,
		0.8f,
		1
	);
}

void WidgetBossHp::RenderDX12(const RenderContextDX12& rc)
{
	// HPゲージ
	float maxHp = static_cast<float>(enemy->GetMaxHp());
	float tempHpRate = tempHp / maxHp;
	gaugeDX12->Begin(rc);
	gaugeDX12->Draw(
		m_position.x, m_position.y,
		m_size.x * tempHpRate, m_size.y,
		0, 0,
		1024.0f * tempHpRate, 64.0f,
		0,
		1.0f, 1.0f, 1.0f, 1.0f);
	gaugeDX12->End(rc.d3d_command_list);

	float hpRate = static_cast<float>(enemy->GetHp()) / static_cast<float>(enemy->GetMaxHp());
	hpRate = min(hpRate, tempHpRate);
	gaugeDX12->Begin(rc);
	gaugeDX12->Draw(
		m_position.x, m_position.y,
		m_size.x * hpRate, m_size.y,
		0, 0,
		1024.0f * hpRate, 64.0f,
		0,
		1.0f, 0.1f, 0.1f, 1.0f);
	gaugeDX12->End(rc.d3d_command_list);

	gaugeFrameDX12->Begin(rc);
	gaugeFrameDX12->Draw(
		m_position.x, m_position.y,
		m_size.x, m_size.y,
		0,
		1, 1, 1, 1);
	gaugeFrameDX12->End(rc.d3d_command_list);

	// ボス名前
}
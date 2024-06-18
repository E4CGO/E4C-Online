#include <string>

#include "TAKOEngine/Rendering/ResourceManager.h"

#include "GameObject/Character/Player/PlayerManager.h"
#include "UI/Widget/WidgetPlayerHP.h"

#include "GameData.h"

WidgetPlayerHP::WidgetPlayerHP() : Widget()
{
	player = PLAYERS.GetPlayerById(GAME_DATA.GetClientId());
	gauge = RESOURCE.LoadSpriteResource("Data/Sprites/Gauge/Hp_line_white.png");
	gaugeFrame = RESOURCE.LoadSpriteResource("Data/Sprites/Gauge/Hp_frame.png");

	position = { 10.0f, 40.0f };
	size = { 512.0f, 32.0f };

	tempHp = static_cast<float>(player->GetHp());
}

void WidgetPlayerHP::Update(float elapsedTime)
{
	// tempHP更新
	float hp = static_cast<float>(player->GetHp());
	if (hp > tempHp) // 回復
	{
		tempHp = static_cast<float>(player->GetHp());
	}
	else if (hp < tempHp) // ダメージ
	{
		tempHp -= min(elapsedTime * max(10.0f, tempHp - hp), tempHp - hp);
	}
}

void WidgetPlayerHP::Render(const RenderContext& rc)
{
	// MPゲージ
	DirectX::XMFLOAT3 mpColor;
	switch (player->GetClassType())
	{
	case PLAYER_CLASS::BARBARIAN:
	case PLAYER_CLASS::KNIGHT:
	case PLAYER_CLASS::ROGUE:
		mpColor = { 1.0f, 0.8f, 0.0f };
		break;
	case PLAYER_CLASS::MAGE:
		mpColor = { 0.3f, 0.3f, 1.0f };
		break;
	}

	float mpRate = player->GetMp() / player->GetMaxMp();
	gauge->Render(
		rc.deviceContext,
		position.x, position.y + size.y + 3.0f, 0.0f,
		size.x * mpRate, size.y,
		0, 0,
		1024.0f * mpRate, 64.0f,
		0,
		mpColor.x, mpColor.y, mpColor.z, 1.0f
	);
	gaugeFrame->Render(
		rc.deviceContext,
		position.x, position.y + size.y + 3.0f, 0.0f,
		size.x, size.y
	);

	// MP文字
	std::string mp = std::to_string(static_cast<int>(player->GetMp())) + "/" + std::to_string(static_cast<int>(player->GetMaxMp()));
	T_TEXT.Render(
		FONT_ID::MsGothic,
		mp.c_str(),
		position.x + (size.x * 0.5f), position.y + size.y + 3.0f + (size.y * 0.5f),
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		FONT_ALIGN::CENTER,
		0.4f,
		1
	);

	// HPゲージ
	float tempHpRate = tempHp / static_cast<float>(player->GetMaxHp());
	gauge->Render(
		rc.deviceContext,
		position.x, position.y, 0,
		size.x * tempHpRate, size.y,
		0, 0,
		1024.0f * tempHpRate, 64.0f,
		0,
		0.8f, 0.0f, 0.0f, 1.0f
	);

	float hpRate = static_cast<float>(player->GetHp()) / static_cast<float>(player->GetMaxHp());
	DirectX::XMFLOAT3 hpColor = { 0.1f, 1.0f, 0.1f }; // デフォルト緑
	if (hpRate < 0.3)
	{
		hpColor.x = 1.0f;
	}
	if (hpRate < 0.1)
	{
		hpColor.y = 0.1f;
	}
	gauge->Render(
		rc.deviceContext,
		position.x, position.y, 0,
		size.x * hpRate, size.y,
		0, 0,
		1024.0f * hpRate, 64.0f,
		0,
		hpColor.x, hpColor.y, hpColor.z, 1.0f
	);

	gaugeFrame->Render(
		rc.deviceContext,
		position.x, position.y, 0,
		size.x, size.y
	);

	// HPゲージ文字
	std::string hp = std::to_string(player->GetHp()) + "/" + std::to_string(player->GetMaxHp());
	T_TEXT.Render(
		FONT_ID::MsGothic,
		hp.c_str(),
		position.x + (size.x * 0.5f), position.y + (size.y * 0.5f),
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		FONT_ALIGN::CENTER,
		0.4f,
		1
	);
}
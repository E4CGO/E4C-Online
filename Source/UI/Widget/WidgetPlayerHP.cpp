#include <string>

#include "TAKOEngine/Rendering/ResourceManager.h"

#include "GameObject/Character/Player/PlayerCharacterManager.h"
#include "UI/Widget/WidgetPlayerHP.h"

#include "TAKOEngine/Tool/Encode.h"

#include "GameData.h"

/**************************************************************************//**
	@brief	UIの設定
*//***************************************************************************/
WidgetPlayerHP::WidgetPlayerHP() : Widget()
{
	player = PlayerCharacterManager::Instance().GetPlayerCharacterById();

	if (T_GRAPHICS.isDX11Active)
	{
		gauge = RESOURCE.LoadSpriteResource("Data/Sprites/UI/Game/hp bar.png");
		gaugeFrame = RESOURCE.LoadSpriteResource("Data/Sprites/UI/Game/hpmpsp bar_waku.png");

		m_position = { SCREEN_W * 0.01f, SCREEN_H * 0.92f };

		m_size = { gauge->GetTextureSize().x * SCREEN_W / 1920.0f, gauge->GetTextureSize().y * SCREEN_H / 1080.0f };
	}
	if (T_GRAPHICS.isDX12Active)
	{
		HPgaugeDX12 = RESOURCE.LoadSpriteResourceDX12("Data/Sprites/UI/Game/hp bar.png");
		HPgaugeFrameDX12 = RESOURCE.LoadSpriteResourceDX12("Data/Sprites/UI/Game/hpmpsp bar_waku.png");
		SPgaugeDX12 = RESOURCE.LoadSpriteResourceDX12("Data/Sprites/UI/Game/sp bar.png");
		MPgaugeDX12 = RESOURCE.LoadSpriteResourceDX12("Data/Sprites/UI/Game/mp bar.png");
		SPMPgaugeFrameDX12 = RESOURCE.LoadSpriteResourceDX12("Data/Sprites/UI/Game/hpmpsp bar_waku.png");

		m_position = { SCREEN_W * 0.01f, SCREEN_H * 0.92f };
		m_size = { HPgaugeFrameDX12->GetTextureWidth() * SCREEN_W / 1920.0f, HPgaugeFrameDX12->GetTextureHeight() * SCREEN_H / 1080.0f };
	}

	tempHp = static_cast<float>(player->GetHp());
}

/**************************************************************************//*
	@brief	UIをアップデート
	@param[in]    elapsedTime
*//***************************************************************************/
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

/**************************************************************************//**
	@brief	DX11の描画
	@param[in]    rc
*//***************************************************************************/
void WidgetPlayerHP::Render(const RenderContext& rc)
{
	// MPゲージ
	DirectX::XMFLOAT3 mpColor = { 1.0f, 0.8f, 0.0f };
	switch (player->GetEnergyType())
	{
	case PlayerCharacter::ENERGY_TYPE::STAMINA:
		mpColor = { 1.0f, 0.8f, 0.0f };
		break;
	case PlayerCharacter::ENERGY_TYPE::MANA:
		mpColor = { 0.3f, 0.3f, 1.0f };
		break;
	}

	float mpRate = player->GetMp() / player->GetMaxMp();
	gauge->Render(
		rc.deviceContext,
		m_position.x, m_position.y + m_size.y + 3.0f, 0.0f,
		m_size.x * mpRate, m_size.y,
		0, 0,
		1024.0f * mpRate, 64.0f,
		0,
		mpColor.x, mpColor.y, mpColor.z, 1.0f
	);
	gaugeFrame->Render(
		rc.deviceContext,
		m_position.x, m_position.y + m_size.y + 3.0f, 0.0f,
		m_size.x, m_size.y
	);

	// MP文字
	std::string mp = std::to_string(static_cast<int>(player->GetMp())) + "/" + std::to_string(static_cast<int>(player->GetMaxMp()));
	T_TEXT.Render(
		FONT_ID::MsGothic,
		mp.c_str(),
		m_position.x + (m_size.x * 0.5f), m_position.y + m_size.y + 3.0f + (m_size.y * 0.5f),
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
		m_position.x, m_position.y, 0,
		m_size.x * tempHpRate, m_size.y,
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
		m_position.x, m_position.y, 0,
		m_size.x * hpRate, m_size.y,
		0, 0,
		1024.0f * hpRate, 64.0f,
		0,
		hpColor.x, hpColor.y, hpColor.z, 1.0f
	);

	gaugeFrame->Render(
		rc.deviceContext,
		m_position.x, m_position.y, 0,
		m_size.x, m_size.y
	);

	// HPゲージ文字
	std::string hp = std::to_string(player->GetHp()) + "/" + std::to_string(player->GetMaxHp());
	T_TEXT.Render(
		FONT_ID::MsGothic,
		hp.c_str(),
		m_position.x + (m_size.x * 0.5f), m_position.y + (m_size.y * 0.5f),
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		FONT_ALIGN::CENTER,
		0.4f,
		1
	);
}

/**************************************************************************//**
	@brief    DX12の描画
	@param[in]    rc
*//***************************************************************************/
void WidgetPlayerHP::RenderDX12(const RenderContextDX12& rc)
{
	// MPゲージ
	this->SPMPgaugeFrameDX12->Begin(rc);
	this->SPMPgaugeFrameDX12->Draw(
		m_position.x, m_position.y + m_size.y * 2,// 0.0f,
		m_size.x, m_size.y,
		0,
		m_color.x, m_color.y, m_color.z, m_color.w
	);
	this->SPMPgaugeFrameDX12->End(rc.d3d_command_list);

	DirectX::XMFLOAT3 mpColor = { 1.0f, 1.0f, 1.0f };
	float mpRate = player->GetMp() / player->GetMaxMp();
	std::string energyName;

	switch (player->GetEnergyType())
	{
	case PlayerCharacter::ENERGY_TYPE::STAMINA:
		this->SPgaugeDX12->Begin(rc);
		this->SPgaugeDX12->Draw(
			m_position.x, m_position.y + m_size.y * 2.0f,// 0.0f,
			m_size.x * mpRate, m_size.y,
			//0, 0,
			//1024.0f * mpRate, 64.0f,
			0,
			mpColor.x, mpColor.y, mpColor.z, 1.0f
		);
		this->SPgaugeDX12->End(rc.d3d_command_list);
		energyName = "SP ";
		break;
	case PlayerCharacter::ENERGY_TYPE::MANA:
		this->MPgaugeDX12->Begin(rc);
		this->MPgaugeDX12->Draw(
			m_position.x, m_position.y + m_size.y * 2.0f,// 0.0f,
			m_size.x * mpRate, m_size.y,
			//0, 0,
			//1024.0f * mpRate, 64.0f,
			0,
			mpColor.x, mpColor.y, mpColor.z, 1.0f
		);
		this->MPgaugeDX12->End(rc.d3d_command_list);
		energyName = "MP ";
		break;
	}

	// MP文字
	std::string mp = energyName + std::to_string(static_cast<int>(player->GetMp())) + "/" + std::to_string(static_cast<int>(player->GetMaxMp()));
	T_TEXT.RenderDX12(
		FONT_ID::MsGothic,
		Encode::string_to_wstring(mp),
		0, m_position.y + m_size.y * 1.5f,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		FONT_ALIGN::LEFT,
		0.6f,
		1
	);

	this->HPgaugeFrameDX12->Begin(rc);
	this->HPgaugeFrameDX12->Draw(
		m_position.x, m_position.y,
		m_size.x, m_size.y,
		0,
		m_color.x, m_color.y, m_color.z, m_color.w
	);
	this->HPgaugeFrameDX12->End(rc.d3d_command_list);

	// HPゲージ
	float tempHpRate = tempHp / static_cast<float>(player->GetMaxHp());

	this->HPgaugeDX12->Begin(rc);
	this->HPgaugeDX12->Draw(
		m_position.x, m_position.y,
		m_size.x * tempHpRate, m_size.y,
		//0, 0,
		//1024.0f * tempHpRate, 64.0f,
		0,
		0.8f, 0.0f, 0.0f, 1.0f
	);
	this->HPgaugeDX12->End(rc.d3d_command_list);

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

	this->HPgaugeDX12->Begin(rc);
	this->HPgaugeDX12->Draw(
		m_position.x, m_position.y,
		m_size.x * hpRate, m_size.y,
		//0, 0,
		//1024.0f * hpRate, 64.0f,
		0,
		hpColor.x, hpColor.y, hpColor.z, m_color.w
	);
	this->HPgaugeDX12->End(rc.d3d_command_list);

	// HPゲージ文字
	std::string hp = "HP " + std::to_string(player->GetHp()) + "/" + std::to_string(player->GetMaxHp());
	T_TEXT.RenderDX12(
		FONT_ID::MsGothic,
		Encode::string_to_wstring(hp),
		0, m_position.y - m_size.y * 0.5f,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		FONT_ALIGN::LEFT,
		0.6f,
		1
	);
}
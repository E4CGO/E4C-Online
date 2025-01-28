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
		m_gauge = RESOURCE.LoadSpriteResource("Data/Sprites/UI/Game/hp bar.png");
		m_gaugeFrame = RESOURCE.LoadSpriteResource("Data/Sprites/UI/Game/hpmpsp bar_waku.png");

		m_iconMale = RESOURCE.LoadSpriteResource("Data/Sprites/UI/Game/boy.png");
		m_iconFemale = RESOURCE.LoadSpriteResource("Data/Sprites/UI/Game/girl.png");

		m_position = { SCREEN_W * 0.04f, SCREEN_H * 0.92f };

		m_size = { m_gauge->GetTextureSize().x * SCREEN_W / 1920.0f, m_gauge->GetTextureSize().y * SCREEN_H / 1080.0f };
	}
	if (T_GRAPHICS.isDX12Active)
	{
		m_HPgaugeDX12 = RESOURCE.LoadSpriteResourceDX12("Data/Sprites/UI/Game/hp bar.png");
		m_HPgaugeFrameDX12 = RESOURCE.LoadSpriteResourceDX12("Data/Sprites/UI/Game/hpmpsp bar_waku.png");

		m_SPgaugeDX12 = RESOURCE.LoadSpriteResourceDX12("Data/Sprites/UI/Game/sp bar.png");
		m_MPgaugeDX12 = RESOURCE.LoadSpriteResourceDX12("Data/Sprites/UI/Game/mp bar.png");
		m_SPMPgaugeFrameDX12 = RESOURCE.LoadSpriteResourceDX12("Data/Sprites/UI/Game/hpmpsp bar_waku.png");

		m_iconMaleDX12 = RESOURCE.LoadSpriteResourceDX12("Data/Sprites/UI/Game/boy.png");
		m_iconFemaleDX12 = RESOURCE.LoadSpriteResourceDX12("Data/Sprites/UI/Game/girl.png");

		m_position = { SCREEN_W * 0.04f, SCREEN_H * 0.92f };
		m_size = { m_HPgaugeFrameDX12->GetTextureWidth() * SCREEN_W / 1920.0f, m_HPgaugeFrameDX12->GetTextureHeight() * SCREEN_H / 1080.0f };
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
	m_gauge->Render(
		rc.deviceContext,
		m_position.x, m_position.y + m_size.y + 3.0f, 0.0f,
		m_size.x * mpRate, m_size.y,
		0, 0,
		1024.0f * mpRate, 64.0f,
		0,
		mpColor.x, mpColor.y, mpColor.z, 1.0f
	);
	m_gaugeFrame->Render(
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
	m_gauge->Render(
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
	m_gauge->Render(
		rc.deviceContext,
		m_position.x, m_position.y, 0,
		m_size.x * hpRate, m_size.y,
		0, 0,
		1024.0f * hpRate, 64.0f,
		0,
		hpColor.x, hpColor.y, hpColor.z, 1.0f
	);

	m_gaugeFrame->Render(
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
	switch (player->GetGenderType())
	{
	case PlayerCharacter::GENDER_TYPE::MALE:
		this->m_iconMaleDX12->Begin(rc);
		this->m_iconMaleDX12->Draw(
			SCREEN_W * 0.005f, m_position.y - m_size.y,// 0.0f,
			m_iconMaleDX12->GetTextureSize().x * 0.05f, m_iconMaleDX12->GetTextureSize().y * 0.05f,
			0.0f,
			0.98f, 0.94f, 0.84f, 1.0f
		);
		this->m_iconMaleDX12->End(rc.d3d_command_list);
		break;
	case PlayerCharacter::GENDER_TYPE::FEMALE:
		this->m_iconFemaleDX12->Begin(rc);
		this->m_iconFemaleDX12->Draw(
			SCREEN_W * 0.005f, m_position.y - m_size.y,// 0.0f,
			m_iconMaleDX12->GetTextureSize().x * 0.05f, m_iconMaleDX12->GetTextureSize().y * 0.05f,
			0.0f,
			0.98f, 0.94f, 0.84f, 1.0f
		);
		this->m_iconFemaleDX12->End(rc.d3d_command_list);
		break;
	}

	// MPゲージ
	this->m_SPMPgaugeFrameDX12->Begin(rc);
	this->m_SPMPgaugeFrameDX12->Draw(
		m_position.x, m_position.y + m_size.y * 2,// 0.0f,
		m_size.x, m_size.y,
		0,
		m_color.x, m_color.y, m_color.z, m_color.w
	);
	this->m_SPMPgaugeFrameDX12->End(rc.d3d_command_list);

	DirectX::XMFLOAT3 mpColor = { 1.0f, 1.0f, 1.0f };
	float mpRate = player->GetMp() / player->GetMaxMp();
	std::string energyName;

	switch (player->GetEnergyType())
	{
	case PlayerCharacter::ENERGY_TYPE::STAMINA:
		this->m_SPgaugeDX12->Begin(rc);
		this->m_SPgaugeDX12->Draw(
			m_position.x, m_position.y + m_size.y * 2.0f,// 0.0f,
			m_size.x * mpRate, m_size.y,
			//0, 0,
			//1024.0f * mpRate, 64.0f,
			0,
			mpColor.x, mpColor.y, mpColor.z, 1.0f
		);
		this->m_SPgaugeDX12->End(rc.d3d_command_list);
		energyName = "SP ";
		break;
	case PlayerCharacter::ENERGY_TYPE::MANA:
		this->m_MPgaugeDX12->Begin(rc);
		this->m_MPgaugeDX12->Draw(
			m_position.x, m_position.y + m_size.y * 2.0f,// 0.0f,
			m_size.x * mpRate, m_size.y,
			//0, 0,
			//1024.0f * mpRate, 64.0f,
			0,
			mpColor.x, mpColor.y, mpColor.z, 1.0f
		);
		this->m_MPgaugeDX12->End(rc.d3d_command_list);
		energyName = "MP ";
		break;
	}

	// MP文字
	std::string mp = energyName + std::to_string(static_cast<int>(player->GetMp())) + "/" + std::to_string(static_cast<int>(player->GetMaxMp()));
	T_TEXT.RenderDX12(
		FONT_ID::MsGothic,
		Encode::string_to_wstring(mp),
		m_position.x, m_position.y + m_size.y * 1.5f,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		FONT_ALIGN::LEFT,
		0.6f,
		1
	);

	this->m_HPgaugeFrameDX12->Begin(rc);
	this->m_HPgaugeFrameDX12->Draw(
		m_position.x, m_position.y,
		m_size.x, m_size.y,
		0,
		m_color.x, m_color.y, m_color.z, m_color.w
	);
	this->m_HPgaugeFrameDX12->End(rc.d3d_command_list);

	// HPゲージ
	float tempHpRate = tempHp / static_cast<float>(player->GetMaxHp());

	this->m_HPgaugeDX12->Begin(rc);
	this->m_HPgaugeDX12->Draw(
		m_position.x, m_position.y,
		m_size.x * tempHpRate, m_size.y,
		//0, 0,
		//1024.0f * tempHpRate, 64.0f,
		0,
		0.8f, 0.0f, 0.0f, 1.0f
	);
	this->m_HPgaugeDX12->End(rc.d3d_command_list);

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

	this->m_HPgaugeDX12->Begin(rc);
	this->m_HPgaugeDX12->Draw(
		m_position.x, m_position.y,
		m_size.x * hpRate, m_size.y,
		//0, 0,
		//1024.0f * hpRate, 64.0f,
		0,
		hpColor.x, hpColor.y, hpColor.z, m_color.w
	);
	this->m_HPgaugeDX12->End(rc.d3d_command_list);

	// HPゲージ文字
	std::string hp = "HP " + std::to_string(player->GetHp()) + "/" + std::to_string(player->GetMaxHp());
	T_TEXT.RenderDX12(
		FONT_ID::MsGothic,
		Encode::string_to_wstring(hp),
		m_position.x, m_position.y - m_size.y * 0.5f,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		FONT_ALIGN::LEFT,
		0.6f,
		1
	);
}
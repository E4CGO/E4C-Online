//! @file WidgetMatching.cpp
//! @note
#include "WidgetMatching.h"

#include "Network/OnlineController.h"
#include "GameObject/Props/Teleporter.h"

WidgetMatching::WidgetMatching(Online::OnlineController* onlineController, Teleporter* teleporter) : m_pOnlineController(onlineController), m_pTeleporter(teleporter), WidgetButtonImage("", "Data/Sprites/UI/Dungeon/dungeon_b.png", [&](WidgetButton*) {
	this->Callback();
	})
{
	m_size = { WidgetButtonImage::m_btnImageDX12->GetTextureSize().x * SCREEN_W / 1920.0f, WidgetButtonImage::m_btnImageDX12->GetTextureSize().y * SCREEN_H / 1080.0f };
	m_position.x = SCREEN_W * .5f - m_size.x * 0.5f;
	m_position.y = SCREEN_H * .3f - m_size.y * 0.5f;
	m_textScale = 1.0f;
	m_matchingText = new WidgetText();

	m_dungeon = RESOURCE.LoadSpriteResourceDX12("Data/Sprites/UI/Dungeon/frame.png");
	m_enter = RESOURCE.LoadSpriteResourceDX12("Data/Sprites/UI/Dungeon/enter_t.png");
}

void WidgetMatching::Update(float elapsedTime)
{
	switch (m_state)
	{
	case STATE::INIT:
		if (m_pOnlineController->GetState() == Online::OnlineController::STATE::OFFLINE)
		{
			m_state = STATE::FULL;
		}
		else
		{
			m_state = STATE::MATCHING;
			m_pOnlineController->StartMatching(this);
		}
		break;
	case STATE::MATCHING:
		m_textToRender = std::to_string(m_playerNum) + "/" + std::to_string(m_maxPlayerNum);
		break;
	case STATE::FULL:
		m_textToRender = "準備完了";
		break;
	}

	m_matchingText->SetText(m_textToRender.c_str());
	m_size = T_TEXT.TextSize(FONT_ID::HGpop, m_textToRender.c_str()) * m_textScale;
	m_matchingText->SetPosition({ SCREEN_W - m_size.x, SCREEN_H - m_size.y });

	WidgetButtonImage::Update(elapsedTime);
	if (enable)
	{
		if (T_INPUT.KeyDown(VK_RETURN) || T_INPUT.GamePadKeyDown(GAME_PAD_BTN::START))
		{
			this->Callback();
		}
	}
}

void WidgetMatching::RenderDX12(const RenderContextDX12& rc)
{
	// BG
	this->m_dungeon->Begin(rc);
	this->m_dungeon->Draw(
		SCREEN_W * 0.5f - m_dungeon->GetTextureSize().x * 0.5f, SCREEN_H * 0.3f - m_dungeon->GetTextureSize().y * 0.5f,// 0.0f,
		m_dungeon->GetTextureSize().x * 1.0f, m_dungeon->GetTextureSize().y * 1.0f,
		0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	);
	this->m_dungeon->End(rc.d3d_command_list);

	// テキスト
	this->m_enter->Begin(rc);
	this->m_enter->Draw(
		SCREEN_W * 0.5f - m_enter->GetTextureSize().x * 0.5f, SCREEN_H * 0.3f - m_enter->GetTextureSize().y * 0.5f - m_size.y,// 0.0f,
		m_enter->GetTextureSize().x * 1.0f, m_enter->GetTextureSize().y * 1.0f,
		0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	);
	this->m_enter->End(rc.d3d_command_list);

	m_matchingText->RenderDX12(rc);
	WidgetButtonImage::RenderDX12(rc);
}

void WidgetMatching::Callback()
{
	if (m_pOnlineController->GetState() == Online::OnlineController::STATE::OFFLINE)
	{
		m_pTeleporter->Teleport();
	}
	else
	{
		m_pOnlineController->ReadyMatching();
		enable = false;
	}
}

WidgetMatching::~WidgetMatching()
{
	delete m_matchingText;
	m_matchingText = nullptr;
}
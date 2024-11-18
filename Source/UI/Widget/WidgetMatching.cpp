//! @file WidgetMatching.cpp
//! @note 
#include "WidgetMatching.h"

#include "Network/OnlineController.h"
#include "GameObject/Props/Teleporter.h"

WidgetMatching::WidgetMatching(Online::OnlineController* onlineController, Teleporter* teleporter) : m_pOnlineController(onlineController), m_pTeleporter(teleporter), WidgetButtonText("", [&](WidgetButton*) {
	this->Callback();
	})
{
	m_position.x = SCREEN_W;
	m_position.y = SCREEN_H;
	m_scale = 1.0f;
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
		m_text = std::to_string(m_playerNum) + "/" + std::to_string(m_maxPlayerNum);
		break;
	case STATE::FULL:
		m_text = "€”õŠ®—¹";
		break;
	}

	m_size = T_TEXT.TextSize(FONT_ID::HGpop, m_text.c_str()) * m_scale;
	m_position.x = SCREEN_W - m_size.x;
	m_position.y = SCREEN_H - m_size.y;

	WidgetButtonText::Update(elapsedTime);
}

void WidgetMatching::Callback()
{
	if (m_pOnlineController->GetState() == Online::OnlineController::STATE::OFFLINE)
	{
		m_pTeleporter->Teleport();
	}
	else
	{

	}
}

WidgetMatching::~WidgetMatching()
{
}
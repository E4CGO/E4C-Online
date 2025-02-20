//! @file WidgetPlayers.cpp
//! @note 

#include "WidgetPlayers.h"
#include "TAKOEngine/Tool/Encode.h"

/**************************************************************************//**
 	@brief	コンストラクタ
*//***************************************************************************/
WidgetPlayers::WidgetPlayers()
{
	const float scale = SCREEN_H * 0.1f;
	for (int i = 0; i < WidgetPlayers::MAX_PLAYER; i++)
	{
		m_pMaleIcons[i] = new WidgetImage("Data/Sprites/UI/Game/boy.png");
		m_pMaleIcons[i]->SetSize(DirectX::XMFLOAT2{ scale, scale });
		m_pMaleIcons[i]->SetPosition(DirectX::XMFLOAT2{ SCREEN_W - scale, i * scale });

		m_pFemaleIcons[i] = new WidgetImage("Data/Sprites/UI/Game/girl.png");
		m_pFemaleIcons[i]->SetSize(DirectX::XMFLOAT2{ scale, scale });
		m_pFemaleIcons[i]->SetPosition(DirectX::XMFLOAT2{ SCREEN_W - scale, i * scale });
	}
}

/**************************************************************************//**
 	@brief		
	@param[in]	rc
*//***************************************************************************/
void WidgetPlayers::RenderDX12(const RenderContextDX12& rc)
{
	const float scale = SCREEN_H * 0.1f;
	DirectX::XMFLOAT2 pos = { SCREEN_W - scale,  scale * 0.5f, };
	int i = 0.0f;
	std::vector<PlayerCharacter*> players = PlayerCharacterManager::Instance().GetAll();
	for (PlayerCharacter*& player : players)
	{
		//if (player->IsPlayer())
		//{
		//	continue;
		//}

		T_TEXT.RenderDX12(
			FONT_ID::HGpop,
			Encode::string_to_wstring(player->GetName()),
			pos.x, pos.y,
			1.0f, 1.0f, 1.0f, 1.0f,
			0.0f,
			FONT_ALIGN::RIGHT
		);

		if (player->GetGenderType() == PlayerCharacter::GENDER_TYPE::MALE)
		{
			m_pMaleIcons[i]->RenderDX12(rc);
		}
		else
		{
			m_pFemaleIcons[i]->RenderDX12(rc);
		}

		pos.y += scale;
		
		i++;
		if (i >= WidgetPlayers::MAX_PLAYER)
		{
			break;
		}
	}
}


/**************************************************************************//**
 	@brief	デストラクタ
*//***************************************************************************/
WidgetPlayers::~WidgetPlayers()
{
	for (int i = 0; i < WidgetPlayers::MAX_PLAYER; i++)
	{
		delete m_pMaleIcons[i];
		m_pMaleIcons[i] = nullptr;
		delete  m_pFemaleIcons[i];
		m_pFemaleIcons[i] = nullptr;
	}
}

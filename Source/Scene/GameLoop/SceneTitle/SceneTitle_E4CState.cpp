#include "SceneTitle_E4CState.h"

#include "TAKOEngine/GUI/UIManager.h"
#include "Scene/SceneManager.h"
#include "Scene/GameLoop/SceneCharacter/SceneCharacter_E4C.h"

/**************************************************************************//**
	@brief タイトル画面初期
*//***************************************************************************/
void SceneTitle_E4CState::InitState::Enter()
{
	if (T_GRAPHICS.isDX11Active)
	{
		m_imgBackground = new WidgetImage("Data/Sprites/UI/Title/background.png");
		m_imgBackground->SetPosition({ 0, 0 });
		m_imgBackground->SetSize({ SCREEN_W, SCREEN_H });
		UI.Register(m_imgBackground);
	}
	if (T_GRAPHICS.isDX12Active)
	{
		m_backgroundAnimation = new WidgetStripeAnimation("Data/Sprites/UI/Title/Title_Anim.png",
			0, 0, SCREEN_W + 1, SCREEN_H,
			819.2, 0, 819.2, 434,
			20, 0.1f);
		UI.Register(m_backgroundAnimation);
	}

	m_imgLogo = new WidgetImage("Data/Sprites/UI/Title/title_logo.png");
	m_imgLogo->SetPosition({ SCREEN_W * .64f,  SCREEN_H * .15f });
	m_imgLogo->SetSize({ m_imgLogo->GetSize().x * .5f * SCREEN_W / 1920.0f, m_imgLogo->GetSize().y * .5f * SCREEN_H / 1080.0f });
	UI.Register(m_imgLogo);

	// TODO:
	//for (int i = 0; i < m_MenuOptions.size(); i++)
	//{
	//	std::string spriteName = "Data/Sprites/UI/Title/" + m_MenuOptions[i] + "_d.png";
	//	std::string spriteNameHover = "Data/Sprites/UI/Title/" + m_MenuOptions[i] + "_h.png";

	//	WidgetButtonImage* btnOption = new WidgetButtonImage("", spriteName.c_str(), spriteNameHover.c_str(), [&](WidgetButton*) {
	//		owner->GetStateMachine()->ChangeState(SceneTitle_E4C::STATE::START);
	//		});
	//	btnOption->SetPosition({ SCREEN_W * .7f, SCREEN_H * 0.5f + btnOption->GetSize().y * 0.75f * i });
	//	btnOption->SetSize({ btnOption->GetSize().x * .5f ,  btnOption->GetSize().y * .5f });
	//	UI.Register(btnOption);
	//}

	m_btnStart = new WidgetButtonImage("", "Data/Sprites/UI/Title/new_d.png", "Data/Sprites/UI/Title/new_h.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneTitle_E4C::STATE::COMIC);
		});
	m_btnStart->SetPosition({ SCREEN_W * .7f, SCREEN_H * 0.5f });
	m_btnStart->SetSize({ m_btnStart->GetSize().x * .5f * SCREEN_W / 1920.0f, m_btnStart->GetSize().y * .5f * SCREEN_H / 1080.0f });
	UI.Register(m_btnStart);

	m_btnOption = new WidgetButtonImage("", "Data/Sprites/UI/Title/option_d.png", "Data/Sprites/UI/Title/option_h.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneTitle_E4C::STATE::OPTION);
		});
	m_btnOption->SetPosition({ SCREEN_W * .7f, SCREEN_H * 0.5f + m_btnStart->GetSize().y * 1.2f * 1.0f });
	m_btnOption->SetSize({ m_btnOption->GetSize().x * .5f * SCREEN_W / 1920.0f, m_btnOption->GetSize().y * .5f * SCREEN_H / 1080.0f });
	UI.Register(m_btnOption);

	m_btnCredits = new WidgetButtonImage("", "Data/Sprites/UI/Title/credits_d.png", "Data/Sprites/UI/Title/credits_h.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneTitle_E4C::STATE::CREDITS);
		});
	m_btnCredits->SetPosition({ SCREEN_W * .7f, SCREEN_H * 0.5f + m_btnStart->GetSize().y * 1.2f * 2.0f });
	m_btnCredits->SetSize({ m_btnCredits->GetSize().x * .5f * SCREEN_W / 1920.0f,  m_btnCredits->GetSize().y * .5f * SCREEN_H / 1080.0f });
	UI.Register(m_btnCredits);

	m_btnExit = new WidgetButtonImage("", "Data/Sprites/UI/Title/exit_d.png", "Data/Sprites/UI/Title/exit_h.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneTitle_E4C::STATE::EXIT);
		});
	m_btnExit->SetPosition({ SCREEN_W * .7f, SCREEN_H * 0.5f + m_btnStart->GetSize().y * 1.2f * 3.0f });
	m_btnExit->SetSize({ m_btnExit->GetSize().x * .5f * SCREEN_W / 1920.0f, m_btnExit->GetSize().y * .5f * SCREEN_H / 1080.0f });
	UI.Register(m_btnExit);
}

/**************************************************************************//**
	@brief タイトル画面アップデート
	@param[in]    elapsedTime　変更時間
*//***************************************************************************/
void SceneTitle_E4CState::InitState::Execute(float elapsedTime)
{
}

/**************************************************************************//**
	@brief	切り替え終了
*//***************************************************************************/
void SceneTitle_E4CState::InitState::Exit()
{
	UI.Clear();
	SetCursor(::LoadCursor(NULL, IDC_HAND));
}

/**************************************************************************//**
	@brief	ゲームを始める準備
*//***************************************************************************/
void SceneTitle_E4CState::StartState::Enter()
{
}
/**************************************************************************//**
	@brief    ゲームに切り替え
	@param[in]    elapsedTime	変更時間
*//***************************************************************************/
void SceneTitle_E4CState::StartState::Execute(float elapsedTime)
{
	SceneManager::Instance().ChangeScene(new SceneLoading(new SceneCharacter_E4C));
}
/**************************************************************************//**
	@brief	切り替え終了
*//***************************************************************************/
void SceneTitle_E4CState::StartState::Exit()
{
	UI.Clear();
	SetCursor(::LoadCursor(NULL, IDC_HAND));
}

/**************************************************************************//*
	@brief	オープション画面初期
*//***************************************************************************/
void SceneTitle_E4CState::OptionState::Enter()
{
	m_settingWindow = new WidgetSettingWindow;

	UI.Register(m_settingWindow);
}
/**************************************************************************//**
	@brief	オープション画面アップデート
	@param[in]    elapsedTime	変更時間
*//***************************************************************************/
void SceneTitle_E4CState::OptionState::Execute(float elapsedTime)
{
	if (m_settingWindow->IsEnd())
	{
		owner->GetStateMachine()->ChangeState(SceneTitle_E4C::STATE::INIT);
	}
}
/**************************************************************************//**
	@brief	切り替え終了
*//***************************************************************************/
void SceneTitle_E4CState::OptionState::Exit()
{
	UI.Clear();
	SetCursor(::LoadCursor(NULL, IDC_HAND));
}

// タイトルステート
/**************************************************************************//**
	@brief	クレジット画面初期
*//***************************************************************************/
void SceneTitle_E4CState::CreditsState::Enter()
{
	UI.Clear();

	m_pbackground = new WidgetImage("Data/Sprites/UI/Credits/credits_bg.png");
	m_pbackground->SetSize({ SCREEN_W, SCREEN_H });
	UI.Register(m_pbackground);

	m_credits = new WidgetImage("Data/Sprites/UI/Credits/credits_t.png");
	m_credits->SetPosition({ SCREEN_W * 0.5f - m_credits->GetSize().x * 0.5f , SCREEN_H * 0.1f - m_credits->GetSize().y * .5f });
	UI.Register(m_credits);

	m_pBackBtn = new WidgetButtonImage("", "Data/Sprites/UI/Character/back_d.png", "Data/Sprites/UI/Character/back_h.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::INIT);
		});
	m_pBackBtn->SetPosition({ SCREEN_H * 0.1f, SCREEN_H * 0.1f });
	UI.Register(m_pBackBtn);

	const char* team_names_prog[7] = {
	"Dimoglo Yevgenii",
	"梁 以樂",
	"坂ノ下 陽大",
	"古川龍",
	"喜田 陽菜",
	"松村守",
	"春田 優",
	};

	m_prog = new WidgetImage("Data/Sprites/UI/Credits/prog_t.png");
	m_prog->SetPosition({ SCREEN_W * 0.15f , SCREEN_H * 0.2f - m_prog->GetSize().y * .5f });
	UI.Register(m_prog);

	const char* team_names_art[2] = {
	"杉山祐亮",
	"Bougacha Oussama"
	};

	const char* support_names_art[8] = {
	"Ben Salah Mohammed Elyes",
	"Danny"
	"Low Ye Xuan",
	"Sumida",
	"Amnuaychaisap Titipong",
	"Weijin Shi",
	"Takeshita Reina",
	"Jevonne Yenarda",
	"Touj"
	};

	m_art = new WidgetImage("Data/Sprites/UI/Credits/art_t.png");
	m_art->SetPosition({ SCREEN_W * 0.4f , SCREEN_H * 0.2f - m_art->GetSize().y * .5f });
	UI.Register(m_art);

	const char* support_names_sound[1] = {
		"Arjay",
	};

	m_sound = new WidgetImage("Data/Sprites/UI/Credits/sound_t.png");
	m_sound->SetPosition({ SCREEN_W * 0.75f , SCREEN_H * 0.2f - m_sound->GetSize().y * .5f });
	UI.Register(m_sound);

	DirectX::XMFLOAT2 pos = { SCREEN_W * 0.15f, SCREEN_H * 0.3f };

	for (uint8_t i = 0; i < 7; i++)
	{
		WidgetText* labelWidget = new WidgetText(team_names_prog[i]);
		labelWidget->SetPosition(pos);
		pos.y += labelWidget->GetSize().y;
		m_pNames.push_back(labelWidget);
	}

	pos = { SCREEN_W * 0.4f, SCREEN_H * 0.3f };

	for (uint8_t i = 0; i < 2; i++)
	{
		WidgetText* labelWidget = new WidgetText(team_names_art[i]);
		labelWidget->SetPosition(pos);
		pos.y += labelWidget->GetSize().y;
		m_pNames.push_back(labelWidget);
	}

	pos = { SCREEN_W * 0.4f, SCREEN_H * 0.6f };

	for (uint8_t i = 0; i < 8; i++)
	{
		WidgetText* labelWidget = new WidgetText(support_names_art[i]);
		labelWidget->SetPosition(pos);
		pos.y += labelWidget->GetSize().y;
		m_pNames.push_back(labelWidget);
	}

	pos = { SCREEN_W * 0.75f, SCREEN_H * 0.6f };

	for (uint8_t i = 0; i < 1; i++)
	{
		WidgetText* labelWidget = new WidgetText(support_names_sound[i]);
		labelWidget->SetPosition(pos);
		pos.y += labelWidget->GetSize().y;
		m_pNames.push_back(labelWidget);
	}

	for (auto& it : m_pNames)
	{
		UI.Register(it);
	}
}
/**************************************************************************//**
	@brief	クレジット画面初期アップデート
	@param[in]    elapsedTime	変更時間
*//***************************************************************************/
void SceneTitle_E4CState::CreditsState::Execute(float elapsedTime)
{
}
/**************************************************************************//**
	@brief	切り替え終了
*//***************************************************************************/
void SceneTitle_E4CState::CreditsState::Exit()
{
	//for (auto& it : m_pNames)
	//{
	//	delete it;
	//	it = nullptr;
	//}

	//delete m_pbackground;
	//m_pbackground = nullptr;
	//delete m_credits;
	//m_credits = nullptr;
	//delete m_prog;
	//m_prog = nullptr;
	//delete m_art;
	//m_art = nullptr;
	//delete m_sound;
	//m_sound = nullptr;

	//delete m_pBackBtn;
	//m_pBackBtn = nullptr;

	UI.Clear();
	SetCursor(::LoadCursor(NULL, IDC_HAND));
}

/**************************************************************************//**
	@brief	ゲームを終了
*//***************************************************************************/
void SceneTitle_E4CState::ExitState::Enter()
{
	PostMessage(TentacleLib::hWnd, WM_QUIT, 0, 0);
}
/**************************************************************************//**
	@brief	ゲームの終了アップデート
	@param[in]    elapsedTime	変更時間
*//***************************************************************************/
void SceneTitle_E4CState::ExitState::Execute(float elapsedTime)
{
}
/**************************************************************************//**
	@brief	切り替え終了
*//***************************************************************************/
void SceneTitle_E4CState::ExitState::Exit()
{
	SetCursor(::LoadCursor(NULL, IDC_HAND));
	UI.Clear();
}

void SceneTitle_E4CState::ComicState::Enter()
{
	UI.Clear();
	m_ComicImage = new WidgetImage("Data/Sprites/UI/Title/1P-.png");
	m_ComicImage->SetPosition({ 0,  0 });
	m_ComicImage->SetSize({ SCREEN_W, SCREEN_H });
	m_ComicImage2 = new WidgetImage("Data/Sprites/UI/Title/2P.png");
	m_ComicImage2->SetPosition({ 0,  0 });
	m_ComicImage2->SetSize({ SCREEN_W, SCREEN_H });
	m_ComicImage3 = new WidgetImage("Data/Sprites/UI/Title/3P-.png");
	m_ComicImage3->SetPosition({ 0,  0 });
	m_ComicImage3->SetSize({ SCREEN_W, SCREEN_H });
	m_StartImage = new WidgetButtonImage("", "Data/Sprites/UI/Character/start.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneTitle_E4C::STATE::START);
		});
	m_StartImage->SetPosition({ SCREEN_W * 0.85f, SCREEN_H * 0.90f });
	UI.Register(m_ComicImage);
}

void SceneTitle_E4CState::ComicState::Execute(float elapsedTime)
{
	if (m_currentImage != m_prevImage)
	{
		switch (m_currentImage)
		{
		case 1:
			UI.Register(m_ComicImage2);
			m_prevImage = m_currentImage;
			break;
		case 2:
			UI.Register(m_ComicImage3);
			m_prevImage = m_currentImage;
			break;
		case 3:
			UI.Register(m_StartImage);
			m_prevImage = m_currentImage;
			break;
		}
	}

	timer -= elapsedTime;

	if (timer <= 0)
	{
		timer = 3.0f;
		m_currentImage++;
	}
}

void SceneTitle_E4CState::ComicState::Exit()
{
	SetCursor(::LoadCursor(NULL, IDC_HAND));
	UI.Clear();
}
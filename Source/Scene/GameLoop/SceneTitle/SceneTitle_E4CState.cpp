#include "SceneTitle_E4CState.h"

#include "TAKOEngine/GUI/UIManager.h"
#include "Scene/SceneManager.h"
#include "Scene/GameLoop/SceneCharacter/SceneCharacter_E4C.h"

/**************************************************************************//**
	@brief タイトル画面初期
*//***************************************************************************/
void SceneTitle_E4CState::InitState::Enter()
{
	m_imgBackground = new WidgetImage("Data/Sprites/UI/Title/background.png");
	m_imgBackground->SetPosition({ 0, 0 });
	m_imgBackground->SetSize({ SCREEN_W, SCREEN_H });
	UI.Register(m_imgBackground);

	m_imgLogo = new WidgetImage("Data/Sprites/UI/Title/title_logo.png");
	m_imgLogo->SetPosition({ SCREEN_W * .64f,  SCREEN_H * .15f });
	m_imgLogo->SetSize({ m_imgLogo->GetSize().x * .5f, m_imgLogo->GetSize().y * .5f });
	UI.Register(m_imgLogo);

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
		owner->GetStateMachine()->ChangeState(SceneTitle_E4C::STATE::START);
		});
	m_btnStart->SetPosition({ SCREEN_W * .7f, SCREEN_H * 0.5f });
	m_btnStart->SetSize({ m_btnStart->GetSize().x * .5f , m_btnStart->GetSize().y * .5f });
	UI.Register(m_btnStart);

	m_btnOption = new WidgetButtonImage("", "Data/Sprites/UI/Title/option_d.png", "Data/Sprites/UI/Title/option_h.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneTitle_E4C::STATE::OPTION);
		});
	m_btnOption->SetPosition({ SCREEN_W * .7f, SCREEN_H * 0.5f + m_btnStart->GetSize().y * 1.2f * 1.0f });
	m_btnOption->SetSize({ m_btnOption->GetSize().x * .5f , m_btnOption->GetSize().y * .5f });
	UI.Register(m_btnOption);

	m_btnCredits = new WidgetButtonImage("", "Data/Sprites/UI/Title/credits_d.png", "Data/Sprites/UI/Title/credits_h.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneTitle_E4C::STATE::CREDITS);
		});
	m_btnCredits->SetPosition({ SCREEN_W * .7f, SCREEN_H * 0.5f + m_btnStart->GetSize().y * 1.2f * 2.0f });
	m_btnCredits->SetSize({ m_btnCredits->GetSize().x * .5f ,  m_btnCredits->GetSize().y * .5f });
	UI.Register(m_btnCredits);

	m_btnExit = new WidgetButtonImage("", "Data/Sprites/UI/Title/exit_d.png", "Data/Sprites/UI/Title/exit_h.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneTitle_E4C::STATE::EXIT);
		});
	m_btnExit->SetPosition({ SCREEN_W * .7f, SCREEN_H * 0.5f + m_btnStart->GetSize().y * 1.2f * 3.0f });
	m_btnExit->SetSize({ m_btnExit->GetSize().x * .5f, m_btnExit->GetSize().y * .5f });
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

	m_btnBack = new WidgetButtonImage("", "Data/Sprites/UI/back.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneTitle_E4C::STATE::INIT);
		});
	m_btnBack->SetPosition({ SCREEN_W * .8f, SCREEN_H * 0.8f });
	m_btnBack->SetSize({ m_btnBack->GetSize().x,  m_btnBack->GetSize().y });

	UI.Register(m_btnBack);
}
/**************************************************************************//**
	@brief	オープション画面アップデート
	@param[in]    elapsedTime	変更時間
*//***************************************************************************/
void SceneTitle_E4CState::OptionState::Execute(float elapsedTime)
{
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
}
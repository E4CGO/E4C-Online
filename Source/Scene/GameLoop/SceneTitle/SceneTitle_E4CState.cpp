#include "SceneTitle_E4CState.h"

#include "TAKOEngine/GUI/UIManager.h"
#include "Scene/SceneManager.h"
#include "Scene/GameLoop/SceneCharacter/SceneCharacter_E4C.h"

#include "Source/UI/Widget/WidgetText.h"
#include "Source/UI/Widget/WidgetButtonImage.h"

WidgetText* title = nullptr;;
WidgetButtonImage* btnStart = nullptr;;
WidgetButtonImage* btnExit = nullptr;;

void SceneTitle_E4CState::InitState::Enter()
{
	title = new WidgetText("TITLE TO BE DECIDED");
	title->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	title->SetScale(2.0f);
	title->SetBorder(1);
	title->SetBorderColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	title->SetAlign(FONT_ALIGN::CENTER);
	title->SetPosition({ SCREEN_W * 0.5f, SCREEN_H * 0.2f });
	UI.Register(title);

	btnStart = new WidgetButtonImage("", "Data/Sprites/UI/start.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneTitle_E4C::STATE::START);
		});
	btnStart->SetPosition({ SCREEN_W * 0.5f - 196.0f * 0.5f * 1.5f, SCREEN_H * 0.5f });
	btnStart->SetSize({ 196.0f * 1.5f, 92.0f * 1.5f });
	UI.Register(btnStart);

	btnExit = new WidgetButtonImage("", "Data/Sprites/UI/exit.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneTitle_E4C::STATE::EXIT);
		});
	btnExit->SetPosition({ SCREEN_W * 0.5f - 163.0f * 0.5f * 1.5f, SCREEN_H * 0.8f });
	btnExit->SetSize({ 163.0f * 1.5f, 128.0f });
	UI.Register(btnExit);
}

void SceneTitle_E4CState::InitState::Execute(float elapsedTime)
{
}

void SceneTitle_E4CState::InitState::Exit()
{
}

// タイトルステート
void SceneTitle_E4CState::StartState::Enter()
{
}
void SceneTitle_E4CState::StartState::Execute(float elapsedTime)
{
	SceneManager::Instance().ChangeScene(new SceneLoading(new SceneCharacter_E4C));
}
void SceneTitle_E4CState::StartState::Exit()
{
	UI.Clear();
	SetCursor(::LoadCursor(NULL, IDC_HAND));
}

// タイトルステート
void SceneTitle_E4CState::ExitState::Enter()
{
	PostMessage(TentacleLib::hWnd, WM_QUIT, 0, 0);
}
void SceneTitle_E4CState::ExitState::Execute(float elapsedTime)
{
}
void SceneTitle_E4CState::ExitState::Exit()
{
	SetCursor(::LoadCursor(NULL, IDC_HAND));
}
#include "SceneGame_E4CState.h"

#include "profiler.h"

#include "TAKOEngine/GUI/UIManager.h"
#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"

#include "GameObject/Character/Player/PlayerManager.h"

#include "Scene/SceneManager.h"
#include "Scene/SceneTitle.h"

// 接続待ちステート
void SceneGame_E4CState::WaitingState::Enter()
{
}
void SceneGame_E4CState::WaitingState::Execute(float elapsedTime)
{
}
void SceneGame_E4CState::WaitingState::Exit()
{
}

// 準備完了ステート（カウントダウン）
void SceneGame_E4CState::ReadyState::Enter()
{
}
void SceneGame_E4CState::ReadyState::Execute(float elapsedTime)
{
}
void SceneGame_E4CState::ReadyState::Exit()
{
}

// ゲームステート
void SceneGame_E4CState::GameState::Enter()
{
}
void SceneGame_E4CState::GameState::Execute(float elapsedTime)
{
}
void SceneGame_E4CState::GameState::Exit()
{
}

// ゲームオーバーステート
void SceneGame_E4CState::GameOverState::Enter()
{
}
void SceneGame_E4CState::GameOverState::Execute(float elapsedTime)
{
}
void SceneGame_E4CState::GameOverState::Exit()
{
}

// ゲームオーバーステート
void SceneGame_E4CState::WinState::Enter()
{
}
void SceneGame_E4CState::WinState::Execute(float elapsedTime)
{
}
void SceneGame_E4CState::WinState::Exit()
{
}
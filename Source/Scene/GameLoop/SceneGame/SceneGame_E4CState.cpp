#include "SceneGame_E4CState.h"

#include "profiler.h"

#include "TAKOEngine/GUI/UIManager.h"
#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"

#include "Scene/SceneManager.h"

// 初期化ステート
void SceneGame_E4CState::InitState::Enter()
{
}
void SceneGame_E4CState::InitState::Execute(float elapsedTime)
{
}
void SceneGame_E4CState::InitState::Exit()
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

// ゲームステート
void SceneGame_E4CState::LoadingState::Enter()
{
}
void SceneGame_E4CState::LoadingState::Execute(float elapsedTime)
{
}
void SceneGame_E4CState::LoadingState::Exit()
{
}

// ゲームステート
void SceneGame_E4CState::MatchingState::Enter()
{
}
void SceneGame_E4CState::MatchingState::Execute(float elapsedTime)
{
}
void SceneGame_E4CState::MatchingState::Exit()
{
}

// ゲームステート
void SceneGame_E4CState::ExitState::Enter()
{
	PostMessage(TentacleLib::hWnd, WM_QUIT, 0, 0);
}
void SceneGame_E4CState::ExitState::Execute(float elapsedTime)
{
}
void SceneGame_E4CState::ExitState::Exit()
{
}
#include "Scene/SceneGame.h"
#include "Scene/SceneTitle.h"
#include "Scene/SceneDungeon.h"

#include "TAKOEngine/AI/StateMachine.h"

#include "GameObject/Character/Player/Player.h"
#include "GameObject/Character/Enemy/Enemy.h"

template <typename T>
StateMachine<T>::~StateMachine()
{
	for (std::pair<int, State<T>*> state : statePool)
	{
		delete state.second;
	}
	statePool.clear();
}
// 更新処理
template <typename T>
void StateMachine<T>::Update(float elapsedTime)
{
	currentState->Execute(elapsedTime);
}
// ステートセット
template <typename T>
void StateMachine<T>::SetState(int newState)
{
	if (statePool.find(newState) == statePool.end()) return;
	currentState = statePool[newState];
	currentState->Enter();
}
// ステート変更
template <typename T>
void StateMachine<T>::ChangeState(int newState)
{
	if (statePool.find(newState) == statePool.end()) return;
	// 現在のステートのExit関数を実行、新しいステートをセット、新しいステートのEnter関数を呼び出す。
	currentState->Exit();

	SetState(newState);
}
// ステート登録
template <typename T>
void StateMachine<T>::RegisterState(int id, HierarchicalState<T>* state)
{
	if (statePool.find(id) != statePool.end()) delete statePool[id];
	// 親ステート登録
	statePool[id] = state;
}
// ステート番号取得
template <typename T>
int StateMachine<T>::GetStateIndex()
{
	for (std::pair<int, State<T>*> state : statePool)
	{
		if (state.second == currentState)
		{
			// i番号目のステートをリターン
			return state.first;
		}
	}

	// ステートが見つからなかった時
	return -1;
}

// ２層目ステート変更
template <typename T>
void StateMachine<T>::ChangeSubState(int newState)
{
	// HierarchicalStateクラスのChangeSubStateを呼び出す
	currentState->ChangeSubState(newState);
}
// ２層目ステート登録
template <typename T>
void StateMachine<T>::RegisterSubState(int index, int subIndex, State<T>* subState)
{
	if (statePool.find(index) == statePool.end()) return; // 存在しない
	statePool[index]->RegisterSubState(subIndex, subState);
}

template class StateMachine<Player>;
template class StateMachine<Enemy>;
template class StateMachine<SceneTitle>;
template class StateMachine<SceneGame>;
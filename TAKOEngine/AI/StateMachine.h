#pragma once

//#include <vector>
#include <unordered_map>

#include "TAKOEngine/AI/BaseState.h"

template <typename T>
class StateMachine
{
public:
	// コンストラクタ
	StateMachine() {}
	// デストラクタ
	~StateMachine()
	{
		for (std::pair<int, State<T>*> state : statePool)
		{
			delete state.second;
		}
		statePool.clear();
	}
	// 更新処理
	void Update(float elapsedTime)
	{
		currentState->Execute(elapsedTime);
	}
	// ステートセット
	void SetState(int newState)
	{
		if (statePool.find(newState) == statePool.end()) return;
		currentState = statePool[newState];
		currentState->Enter();
	}
	// ステート変更
	void ChangeState(int newState)
	{
		if (statePool.find(newState) == statePool.end()) return;
		currentState->Exit();

		SetState(newState);
	}
	// ステート登録
	void RegisterState(int id, HierarchicalState<T>* state)
	{
		if (statePool.find(id) != statePool.end()) delete statePool[id];
		if (state == nullptr)
		{
			statePool.erase(id);
			return;
		}
		statePool[id] = state;
	}
	// 現在のステート番号取得
	int GetStateIndex()
	{
		for (std::pair<int, State<T>*> state : statePool)
		{
			if (state.second == currentState)
			{
				return state.first;
			}
		}
		return -1;
	}

	// ２層目ステート変更
	void ChangeSubState(int newState)
	{
		currentState->ChangeSubState(newState);
	}
	// ２層目ステート登録
	void RegisterSubState(int index, int subIndex, State<T>* subState)
	{
		if (statePool.find(index) == statePool.end()) return;
		statePool[index]->RegisterSubState(subIndex, subState);
	}

	// ステート取得
	HierarchicalState<T>* GetState() { return currentState; }
	HierarchicalState<T>* GetState(int index) { return (statePool.find(index) == statePool.end()) ? nullptr : statePool[index]; }
private:
	// 現在のステート
	HierarchicalState<T>* currentState = nullptr;
	// 各ステートを保持する配列
	std::unordered_map<int, HierarchicalState<T>*> statePool;
};
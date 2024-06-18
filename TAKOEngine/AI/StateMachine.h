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
	~StateMachine();
	// 更新処理
	void Update(float elapsedTime);
	// ステートセット
	void SetState(int setstate);
	// ステート変更
	void ChangeState(int newState);
	// ステート登録
	void RegisterState(int id, HierarchicalState<T>* state);
	// 現在のステート番号取得
	int GetStateIndex();

	// ２層目ステート変更
	void ChangeSubState(int newState);
	// ２層目ステート登録
	void RegisterSubState(int index, int subIndex, State<T>* subState);
	// ステート取得
	HierarchicalState<T>* GetState() { return currentState; }
private:
	// 現在のステート
	HierarchicalState<T>* currentState = nullptr;
	// 各ステートを保持する配列
	std::unordered_map<int, HierarchicalState<T>*> statePool;
};
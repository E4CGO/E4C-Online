#pragma once

#include <unordered_map>

template<typename T>
class State
{
public:
	State(T* owner) : owner(owner) {}
	virtual ~State() = default;

	// 全て継承先で実装させる必要があるため純粋仮想関数で実装
	// ステートに入った時のメソッド
	virtual void Enter() = 0;
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime) = 0;
	// ステートから出ていくときのメソッド
	virtual void Exit() = 0;

protected:
	T* owner;
};

template<typename T>
class HierarchicalState : public State<T>
{
public:
	// コンストラクタ
	HierarchicalState(T* owner) : State<T>(owner) {};
	virtual ~HierarchicalState() {}
	// 全て継承先で実装させる必要があるため純粋仮想関数で実装
	// ステートに入った時のメソッド
	virtual void Enter() = 0;
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime) = 0;
	// ステートから出ていくときのメソッド
	virtual void Exit() = 0;
	// サブステート登録
	virtual void SetSubState(int newState)
	{
		if (statePool.find(newState) == statePool.end()) return;
		subState = statePool[newState];
		subState->Enter();
	}
	// サブステート変更
	virtual void ChangeSubState(int newState)
	{
		if (statePool.find(newState) == statePool.end()) return;
		// 現在のステートのExit関数を実行、新しいステートをセット、新しいステートのEnter関数を呼び出す。
		subState->Exit();

		SetSubState(newState);
	}
	// サブステート登録
	virtual void RegisterSubState(int id, State<T>* state)
	{
		if (statePool.find(id) == statePool.end()) delete statePool[id];
		// 親ステート登録
		statePool[id] = state;
	}
	// サブステート取得
	virtual State<T>* GetSubState() { return subState; }
	// サブステート番号取得
	virtual int GetSubStateIndex() {
		for (std::pair<int, State<T>*> state : statePool)
		{
			if (state.second == subState)
			{
				// i番号目のステートをリターン
				return state.first;
			}
		}

		// ステートが見つからなかった時
		return -1;
	};
protected:
	std::unordered_map<int, State<T>*> statePool;
	State<T>* subState = nullptr;
};
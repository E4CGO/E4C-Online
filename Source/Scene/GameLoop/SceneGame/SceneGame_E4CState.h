#pragma once

#include "TAKOEngine/AI/BaseState.h"

#include "Scene/GameLoop/SceneGame/SceneGame_E4c.h"
#include "UI/Widget/WidgetText.h"

namespace SceneGame_E4CState
{
	// 接続待ちステート
	class WaitingState : public HierarchicalState<SceneGame_E4C>
	{
	public:
		// コンストラクタ
		WaitingState(SceneGame_E4C* scene) : HierarchicalState<SceneGame_E4C>(scene) {};
		// デストラクタ
		~WaitingState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	private:
		WidgetText* connecting;
		WidgetText* connectionCount;
		WidgetText* description;
		WidgetText* charaDescription;
	};
	// 準備完了ステート（カウントダウン）
	class ReadyState : public HierarchicalState<SceneGame_E4C>
	{
	public:
		// コンストラクタ
		ReadyState(SceneGame_E4C* scene) : HierarchicalState<SceneGame_E4C>(scene) {};
		// デストラクタ
		~ReadyState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	private:
		float startTimer;
		WidgetText* countDownText;
	};
	// ゲームステート
	class GameState : public HierarchicalState<SceneGame_E4C>
	{
	public:
		// コンストラクタ
		GameState(SceneGame_E4C* scene) : HierarchicalState<SceneGame_E4C>(scene) {};
		// デストラクタ
		~GameState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	};
	// ゲームオーバーステート
	class GameOverState : public HierarchicalState<SceneGame_E4C>
	{
	public:
		// コンストラクタ
		GameOverState(SceneGame_E4C* scene) : HierarchicalState<SceneGame_E4C>(scene) {};
		// デストラクタ
		~GameOverState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	private:
		float timer = 3.0f;
	};
	// 勝利
	class WinState : public HierarchicalState<SceneGame_E4C>
	{
	public:
		// コンストラクタ
		WinState(SceneGame_E4C* scene) : HierarchicalState<SceneGame_E4C>(scene) {};
		// デストラクタ
		~WinState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	private:
		float timer = 5.0f;
	};
}

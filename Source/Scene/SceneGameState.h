#pragma once

#include "TAKOEngine/AI/BaseState.h"

#include "Scene/SceneGame.h"
#include "UI/Widget/WidgetText.h"

namespace SceneGameState
{
	// 接続待ちステート
	class WaitingState : public HierarchicalState<SceneGame>
	{
	public:
		// コンストラクタ
		WaitingState(SceneGame* scene) : HierarchicalState<SceneGame>(scene) {};
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
	class ReadyState : public HierarchicalState<SceneGame>
	{
	public:
		// コンストラクタ
		ReadyState(SceneGame* scene) : HierarchicalState<SceneGame>(scene) {};
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
	class GameState : public HierarchicalState<SceneGame>
	{
	public:
		// コンストラクタ
		GameState(SceneGame* scene) : HierarchicalState<SceneGame>(scene) {};
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
	class GameOverState : public HierarchicalState<SceneGame>
	{
	public:
		// コンストラクタ
		GameOverState(SceneGame* scene) : HierarchicalState<SceneGame>(scene) {};
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
	class WinState : public HierarchicalState<SceneGame>
	{
	public:
		// コンストラクタ
		WinState(SceneGame* scene) : HierarchicalState<SceneGame>(scene) {};
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

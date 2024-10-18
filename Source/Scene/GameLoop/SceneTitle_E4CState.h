#pragma once

#include "TAKOEngine/Network/HttpRequest.h"
#include "TAKOEngine/AI/BaseState.h"

#include "Scene/GameLoop/SceneTitle_E4C.h"

namespace SceneTitle_E4CState
{
	// タイトルステート
	class InitState : public HierarchicalState<SceneTitle_E4C>
	{
	public:
		// コンストラクタ
		InitState(SceneTitle_E4C* scene) : HierarchicalState<SceneTitle_E4C>(scene) {};
		// デストラクタ
		~InitState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	private:
		float timer = 0.0f;
	};

	// タイトルステート
	class StartState : public HierarchicalState<SceneTitle_E4C>
	{
	public:
		// コンストラクタ
		StartState(SceneTitle_E4C* scene) : HierarchicalState<SceneTitle_E4C>(scene) {};
		// デストラクタ
		~StartState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	private:
		float timer = 0.0f;
	};

	// ログインステート
	class ExitState : public HierarchicalState<SceneTitle_E4C>
	{
	public:
		// コンストラクタ
		ExitState(SceneTitle_E4C* scene) : HierarchicalState<SceneTitle_E4C>(scene) {};
		// デストラクタ
		~ExitState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	};
}
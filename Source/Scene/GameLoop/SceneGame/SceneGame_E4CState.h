//! @file SceneGame_E4CState.h
//! @note 

#ifndef __INCLUDED_SCENE_GAME_E4C_GAME__
#define __INCLUDED_SCENE_GAME_E4C_GAME__

#include "TAKOEngine/AI/BaseState.h"

#include "Scene/GameLoop/SceneGame/SceneGame_E4c.h"
#include "UI/Widget/WidgetText.h"

namespace SceneGame_E4CState
{
	class InitState : public HierarchicalState<SceneGame_E4C>
	{
	public:
		// コンストラクタ
		InitState(SceneGame_E4C* scene) : HierarchicalState<SceneGame_E4C>(scene) {}
		// デストラクタ
		~InitState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	};

	class GameState : public HierarchicalState<SceneGame_E4C>
	{
	public:
		// コンストラクタ
		GameState(SceneGame_E4C* scene) : HierarchicalState<SceneGame_E4C>(scene) {}
		// デストラクタ
		~GameState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	};

	class LoadingState : public HierarchicalState<SceneGame_E4C>
	{
	public:
		// コンストラクタ
		LoadingState(SceneGame_E4C* scene) : HierarchicalState<SceneGame_E4C>(scene) {}
		// デストラクタ
		~LoadingState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	};

	class MatchingState : public HierarchicalState<SceneGame_E4C>
	{
	public:
		// コンストラクタ
		MatchingState(SceneGame_E4C* scene) : HierarchicalState<SceneGame_E4C>(scene) {}
		// デストラクタ
		~MatchingState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	};
}
#endif // !__INCLUDED_SCENE_GAME_E4C_GAME__

#pragma once

#include "TAKOEngine/Network/HttpRequest.h"
#include "TAKOEngine/AI/BaseState.h"
#include "Source/UI/Widget/WidgetButtonImage.h"

#include "Scene/GameLoop/SceneCharacter/SceneCharacter_E4C.h"

namespace SceneCharacter_E4CState
{
	// タイトルステート
	class InitState : public HierarchicalState<SceneCharacter_E4C>
	{
	public:
		// コンストラクタ
		InitState(SceneCharacter_E4C* scene) : HierarchicalState<SceneCharacter_E4C>(scene) {};
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
	class CharacterSelectionState : public HierarchicalState<SceneCharacter_E4C>
	{
	public:
		// コンストラクタ
		CharacterSelectionState(SceneCharacter_E4C* scene) : HierarchicalState<SceneCharacter_E4C>(scene) {};
		// デストラクタ
		~CharacterSelectionState() {}
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
	class CharacterCreationStateLeft : public HierarchicalState<SceneCharacter_E4C>
	{
	public:
		// コンストラクタ
		CharacterCreationStateLeft(SceneCharacter_E4C* scene) : HierarchicalState<SceneCharacter_E4C>(scene) {};
		// デストラクタ
		~CharacterCreationStateLeft() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;

		bool clicked = false;
	};

	// ログインステート
	class CharacterCreationStateCenter : public HierarchicalState<SceneCharacter_E4C>
	{
	public:
		// コンストラクタ
		CharacterCreationStateCenter(SceneCharacter_E4C* scene) : HierarchicalState<SceneCharacter_E4C>(scene) {};
		// デストラクタ
		~CharacterCreationStateCenter() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;

		bool clicked = false;
	};

	// ログインステート
	class CharacterCreationStateRight : public HierarchicalState<SceneCharacter_E4C>
	{
	public:
		// コンストラクタ
		CharacterCreationStateRight(SceneCharacter_E4C* scene) : HierarchicalState<SceneCharacter_E4C>(scene) {};
		// デストラクタ
		~CharacterCreationStateRight() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;

		bool clicked = false;
	};

	// タイトルステート
	class StartState : public HierarchicalState<SceneCharacter_E4C>
	{
	public:
		// コンストラクタ
		StartState(SceneCharacter_E4C* scene) : HierarchicalState<SceneCharacter_E4C>(scene) {};
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
}

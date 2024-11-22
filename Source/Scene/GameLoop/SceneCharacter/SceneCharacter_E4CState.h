#pragma once

#include <vector>
#include <array>
#include <DirectXMath.h>

#include "TAKOEngine/Network/HttpRequest.h"
#include "TAKOEngine/AI/BaseState.h"
#include "Source/UI/Widget/WidgetButtonImage.h"

#include "Scene/GameLoop/SceneCharacter/SceneCharacter_E4C.h"

namespace SceneCharacter_E4CState
{
	// UIとプレイヤーデーター変数
	static WidgetButtonImage* btnCharacterLeft = nullptr;
	static WidgetButtonImage* btnCharacterCenter = nullptr;
	static WidgetButtonImage* btnCharacterRight = nullptr;

	static WidgetButtonImage* btnStartCharacter = nullptr;
	static std::shared_ptr<Sprite> background;

	static int currentState = 0;

	static PlayerCharacterData::CharacterInfo currentCharacterInfo = {
			true,			// visible
			"",				// save
			{				//Character
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
			}
	};

	static PlayerCharacterData::CharacterInfo previousCharacterInfo = {
			true,			// visible
			"",				// save
			{				//Character
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
			}
	};

	static std::vector < std::string> saveNames{
		"Left",
		"Center",
		"Right"
	};

	static std::vector<std::array<DirectX::XMFLOAT3, 4>> cameraSettings{
		{{{ 3.7f, 2.1f, 10.5f}, { 3.5f,-2.3f,-15.0f}, {0.0f, 1.0f, 0.0f}}},
		{{{ 0.0f, 2.1f, 10.5f}, { 0.0f, 0.0f, 0.0f }, {0.0f, 1.0f, 0.0f}}},
		{{{-3.7f, 2.1f, 10.5f}, {-3.5f,-2.3f,-15.0f}, {0.0f, 1.0f, 0.0f}}},
		{{{ 0.0f, 3.3f, 12.4f},	{0.06f,0.65f, 0.73f}, {0.0f, 0.1f, 0.0f}}}
	};
	static std::array<DirectX::XMFLOAT3, 3> characterPositions{
		{{3.5f, 0.0f, 5.0f}, {0.0f, 0.0f, 5.0f}, {-3.5f, 0.0f, 5.0f},}
	};

	/**************************************************************************//**
		@class	InitState
		@brief	初期化とJSONからデーターを読み込み
		@par    [説明]
	*//***************************************************************************/
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

	/**************************************************************************//**
		@class	CharacterSelectionState
		@brief	キャラセレクト
		@par    [説明]
	*//***************************************************************************/
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
		void setCurrentStateLeft();
		void setCurrentStateCenter();
		void setCurrentStateRight();
	};

	/**************************************************************************//**
		@class	CharacterCreationState
		@brief	キャラクリ
		@par    [説明]
	*//***************************************************************************/
	class CharacterCreationState : public HierarchicalState<SceneCharacter_E4C>
	{
	public:
		// コンストラクタ
		CharacterCreationState(SceneCharacter_E4C* scene) : HierarchicalState<SceneCharacter_E4C>(scene) {};
		// デストラクタ
		~CharacterCreationState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;

		bool clicked = false;
	};

	/**************************************************************************//**
		@class	StartState
		@brief	ゲームを始める
		@par    [説明]
	*//***************************************************************************/
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

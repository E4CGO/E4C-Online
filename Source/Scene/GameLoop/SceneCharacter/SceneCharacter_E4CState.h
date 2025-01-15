#pragma once

#include <vector>
#include <array>
#include <DirectXMath.h>

#include "TAKOEngine/Network/HttpRequest.h"
#include "TAKOEngine/AI/BaseState.h"
#include "Source/UI/Widget/WidgetButtonImage.h"
#include "UI/Widget/WidgetButtonText.h"
#include "Source/UI/Widget/WidgetPauseMenu.h"

#include "Scene/GameLoop/SceneCharacter/SceneCharacter_E4C.h"

#include "UI/Widget/WidgetCharacterSelect.h"
#include "UI/Widget/WidgetCharacterModify.h"
#include "UI/Widget/WidgetInputString.h"

namespace SceneCharacter_E4CState
{
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
		CharacterSelectionState(SceneCharacter_E4C* scene) : m_pWidgetCharacterSelect(nullptr), HierarchicalState<SceneCharacter_E4C>(scene) {};
		// デストラクタ
		~CharacterSelectionState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	private:
		DirectX::XMFLOAT3 m_cameraOriginPos = {};
		DirectX::XMFLOAT3 m_cameraOriginFocus = {};
		float m_cameraTimer = 0.0f;
		const float m_cameraTime = 0.5f;
		WidgetCharacterSelect* m_pWidgetCharacterSelect;
		WidgetPauseMenu* m_pPauseMenu;
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
	private:
		DirectX::XMFLOAT3 m_cameraOriginPos = {};
		DirectX::XMFLOAT3 m_cameraOriginFocus = {};
		float m_cameraTimer = 0.0f;
		const float m_cameraTime = 0.5f;

		PlayerCharacter* m_pCharacter = nullptr;

		WidgetButtonText* m_pBackBtn = nullptr;
		WidgetButtonText* m_pStartBtn = nullptr;

		WidgetCharacterModify* m_pWidgetCharacterModify = nullptr;
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

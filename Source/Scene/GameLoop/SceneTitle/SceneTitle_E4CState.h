//! @file SceneTitle_E4CState.h
//! @note　シーンタイトルステート

#ifndef __INCLUDED_SCENE_TITLE_E4C_STATE_H__
#define __INCLUDED_SCENE_TITLE_E4C_STATE_H__

#include "TAKOEngine/Network/HttpRequest.h"
#include "TAKOEngine/AI/BaseState.h"

#include "Source/UI/Widget/WidgetText.h"
#include "Source/UI/Widget/WidgetImage.h"
#include "Source/UI/Widget/WidgetButtonImage.h"
#include "Source/UI/Widget/WidgetButtonText.h"
#include "Source/UI/Widget/WidgetSettingWindow.h"
#include "Source/UI/Widget/WidgetStripeAnimation.h"

#include "Scene/GameLoop/SceneTitle/SceneTitle_E4C.h"

namespace SceneTitle_E4CState
{
	/**************************************************************************//**
		@class    InitState
		@brief    最初の画面状態
		@par    [説明]	最初の画面状態管理
	*//***************************************************************************/
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
		// 背景画像
		WidgetImage* m_imgBackground = nullptr;
		// ロゴ画僧
		WidgetImage* m_imgLogo = nullptr;

		/*std::vector<std::string> m_MenuOptions = {
			"new",
			"option",
			"credits",
			"exit",
		};*/

		// メニューボタン
		WidgetButtonImage* m_btnStart = nullptr;
		WidgetButtonImage* m_btnOption = nullptr;
		WidgetButtonImage* m_btnCredits = nullptr;
		WidgetButtonImage* m_btnExit = nullptr;

		WidgetStripeAnimation* m_backgroundAnimation;

		float timer = 0.0f;
	};

	/**************************************************************************//**
		@class	StartState
		@brief	ゲームに切り替え
		@par    [説明]	ゲームのスタートを管理
	*//***************************************************************************/
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

	class ComicState : public HierarchicalState<SceneTitle_E4C>
	{
	public:
		// コンストラクタ
		ComicState(SceneTitle_E4C* scene) : HierarchicalState<SceneTitle_E4C>(scene) {};
		// デストラクタ
		~ComicState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	private:
		float timer = 3.0f;
		int m_currentImage = 0;
		int m_prevImage = 0;
		WidgetImage* m_ComicImage = nullptr;
		WidgetImage* m_ComicImage2 = nullptr;
		WidgetImage* m_ComicImage3 = nullptr;
		WidgetButtonImage* m_StartImage = nullptr;
		WidgetButtonImage* m_ContinueImage = nullptr;
	};

	/**************************************************************************//**
		@class	OptionState
		@brief	オープションに切り替え
		@par    [説明]	オープションを管理
	*//***************************************************************************/
	class OptionState : public HierarchicalState<SceneTitle_E4C>
	{
	public:
		// コンストラクタ
		OptionState(SceneTitle_E4C* scene) : HierarchicalState<SceneTitle_E4C>(scene) {};
		// デストラクタ
		~OptionState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	private:

		// 設定ウィジェット
		WidgetSettingWindow* m_settingWindow = nullptr;
		// 前に戻すボタン
		WidgetButtonImage* m_btnBack = nullptr;

		float timer = 0.0f;
	};

	/**************************************************************************//**
		@class	CreditsState
		@brief	クレジットに切り替え
		@par    [説明]	クレジットを管理
	*//***************************************************************************/
	class CreditsState : public HierarchicalState<SceneTitle_E4C>
	{
	public:
		// コンストラクタ
		CreditsState(SceneTitle_E4C* scene) : HierarchicalState<SceneTitle_E4C>(scene) {};
		// デストラクタ
		~CreditsState() {}
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
		@class	ExitState
		@brief	ゲームを終了
		@par    [説明]	ゲームを終了管理
	*//***************************************************************************/
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

#endif // __INCLUDED_SCENE_TITLE_E4C_STATE_H__
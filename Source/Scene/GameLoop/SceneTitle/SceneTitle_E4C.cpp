#include "SceneTitle_E4C.h"
#include "SceneTitle_E4CState.h"

#include <imgui.h>
#include <string>
#include <fstream>

#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Network/HttpRequest.h"
#include "TAKOEngine/Rendering/ResourceManager.h"
#include "TAKOEngine/Editor/Camera/CameraManager.h"
#include "TAKOEngine/GUI/UIManager.h"
#include "TAKOEngine/Sound/Sound.h"

#include "Scene/SceneManager.h"

#include "GameData.h"

/**************************************************************************//**
	@brief	初期化
*//***************************************************************************/
void SceneTitle_E4C::Initialize()
{
	// Sprite Resource Preload
	for (auto& filename : spriteList)
	{
		spritePreLoad.insert(RESOURCE.LoadSpriteResource(filename));
	}

	// フレームバッファマネージャー
	m_frameBuffer = T_GRAPHICS.GetFrameBufferManager();

	// ステート
	stateMachine = std::make_unique<StateMachine<SceneTitle_E4C>>();
	stateMachine->RegisterState(STATE::INIT, new SceneTitle_E4CState::InitState(this));
	stateMachine->RegisterState(STATE::START, new SceneTitle_E4CState::StartState(this));
	stateMachine->RegisterState(STATE::OPTION, new SceneTitle_E4CState::OptionState(this));
	stateMachine->RegisterState(STATE::CREDITS, new SceneTitle_E4CState::CreditsState(this));
	stateMachine->RegisterState(STATE::EXIT, new SceneTitle_E4CState::ExitState(this));
	stateMachine->SetState(STATE::INIT);

	Sound::Instance().InitAudio();
	Sound::Instance().LoadAudio("Data/Sound/1-Parallel(Title_Theme).mp3");/*
	Sound::Instance().LoadAudio("Data/Sound/2-A_Royal_Visit.mp3");
	Sound::Instance().LoadAudio("Data/Sound/3-Dreamland(Overworld).mp3");
	Sound::Instance().LoadAudio("Data/Sound/4-Encounter(battle_theme_Overworld_Tutorial).mp3");
	Sound::Instance().LoadAudio("Data/Sound/5-Miraculous_Maze(Dungeon).mp3");
	Sound::Instance().LoadAudio("Data/Sound/7-Credits.mp3");
	Sound::Instance().LoadAudio("Data/Sound/8-Toys_Battle.mp3");
	Sound::Instance().LoadAudio("Data/Sound/9-Lead_The_Way.mp3");
	Sound::Instance().LoadAudio("Data/Sound/10-Determined_Clash.mp3");*/
	Sound::Instance().PlayAudio(0);
}

/**************************************************************************//**
	@brief	終わり
*//***************************************************************************/
void SceneTitle_E4C::Finalize()
{
	spritePreLoad.clear();
	UI.Clear();
	Sound::Instance().StopAudio(0);
}

/**************************************************************************//**
	@brief     更新処理
	@param[in]    elapsedTime	変更時間
*//***************************************************************************/
void SceneTitle_E4C::Update(float elapsedTime)
{
	stateMachine->Update(elapsedTime);

	UI.Update(elapsedTime);
}

/**************************************************************************//**
	@brief    描画処理
*//***************************************************************************/
void SceneTitle_E4C::Render()
{
	T_TEXT.Begin();

	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->Clear(T_GRAPHICS.GetDeviceContext(), 0.2f, 0.2f, 0.2f, 1);
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->SetRenderTarget(T_GRAPHICS.GetDeviceContext());

	// 描画コンテキスト設定
	RenderContext rc;
	rc.deviceContext = T_GRAPHICS.GetDeviceContext();
	rc.renderState = T_GRAPHICS.GetRenderState();

	UI.Render(rc);

	T_TEXT.End();
}

/**************************************************************************//**
	@brief	描画処理 DirectX12
*//***************************************************************************/
void SceneTitle_E4C::RenderDX12()
{
	T_GRAPHICS.BeginRender();
	{
		// レンダーコンテキスト設定
		RenderContextDX12 rc;
		rc.d3d_command_list = m_frameBuffer->GetCommandList();

		T_TEXT.BeginDX12();

		UI.RenderDX12(rc);

		T_TEXT.EndDX12();
	}

#ifdef _DEBUG
	DrawSceneGUI();
	T_GRAPHICS.GetImGUIRenderer()->RenderDX12(m_frameBuffer->GetCommandList());
#endif

	T_GRAPHICS.End();
}

/**************************************************************************//**
	@brief	IMGUI描画処理
*//***************************************************************************/
void SceneTitle_E4C::DrawSceneGUI()
{
}
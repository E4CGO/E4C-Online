#include "SceneCharacter_E4C.h"

#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Network/HttpRequest.h"
#include "TAKOEngine/Rendering/ResourceManager.h"
#include "TAKOEngine/GUI/UIManager.h"
#include "TAKOEngine/Editor/Camera/CameraManager.h"
#include "TAKOEngine/Sound/Sound.h"

#include <imgui.h>
#include <string>

#include "Scene/SceneManager.h"
#include "SceneCharacter_E4CState.h"

#include "GameData.h"
#include "PreloadManager.h"

/**************************************************************************//**
	@brief	初期化
*//***************************************************************************/
void SceneCharacter_E4C::Initialize()
{
	if (T_GRAPHICS.isDX11Active)
	{
		// Sprite Resource Preload
		for (auto& filename : spriteList)
		{
			spritePreLoad.insert(RESOURCE.LoadSpriteResource(filename));
		}
		// Model Resource Preload
		for (auto& filename : modelList)
		{
			modelPreLoad.insert(RESOURCE.LoadModelResource(filename));
		}
	}
	else
	{
		// Model Resource Preload
				//for (auto& filename : modelList)
		//{
		//	modelPreLoad.insert(RESOURCE.LoadModelDX12Resource(filename));
		//}
		PRELOAD.Join("CharacterModels");
	}
	PRELOAD.Lock();
	m_background = std::make_unique<ModelObject>("Data/Model/Object/PlaneBG.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_Phong);
	m_background->SetPosition({ 0.0, 0.0f, -10.0f });
	m_background->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	m_background->SetScale({ 20.0f, 20.0f, 1.0f });

	//シャドウマップレンダラ
	shadowMapRenderer->Initialize();

	// フレームバッファマネージャー
	m_frameBuffer = T_GRAPHICS.GetFrameBufferManager();

	// 光
	LightManager::Instance().SetAmbientColor({ 0.3f, 0.3f, 0.3f, 0.0f });
	Light* dl = new Light(LightType::Directional);
	dl->SetDirection({ 0.0f, -0.503f, -0.864f });
	LightManager::Instance().Register(dl);
	shadowMapRenderer->SetShadowLight(dl);

	CameraManager& cameraManager = CameraManager::Instance();
	Camera* mainCamera = new Camera();
	cameraManager.Register(mainCamera);
	cameraManager.SetCamera(0);

	// カメラ設定
	CameraManager::Instance().GetCamera()->SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),		// 画角
		SCREEN_W / SCREEN_H,					// 画面アスペクト比
		0.1f,									// ニアクリップ
		10000.0f								// ファークリップ
	);
	CameraManager::Instance().GetCamera()->SetLookAt(
		{ 0, 3.0f, 13.0f },				// 視点
		{ 0.0f, 0.0, 0.0f },					// 注視点
		{ 0.036f, 0.999f, -0.035f }				// 上ベクトル
	);
	cameraController = std::make_unique<FreeCameraController>();
	cameraController->SyncCameraToController(CameraManager::Instance().GetCamera());
	cameraController->SetEnable(false);

	m_previewCharacters.clear();

	// ステート
	stateMachine = std::make_unique<StateMachine<SceneCharacter_E4C>>();
	stateMachine->RegisterState(STATE::INIT, new SceneCharacter_E4CState::InitState(this));
	stateMachine->RegisterState(STATE::CHARACTER_SELECTION, new SceneCharacter_E4CState::CharacterSelectionState(this));
	stateMachine->RegisterState(STATE::CHARACTER_CREATION, new SceneCharacter_E4CState::CharacterCreationState(this));
	stateMachine->RegisterState(STATE::START, new SceneCharacter_E4CState::StartState(this));
	stateMachine->SetState(STATE::INIT);

	PRELOAD.Unlock();

	// 影初期化
	T_GRAPHICS.GetShadowRenderer()->Init(T_GRAPHICS.GetDeviceDX12());

	Sound::Instance().InitAudio();
	Sound::Instance().LoadAudio("Data/Sound/11-Character_select.mp3");
	Sound::Instance().PlayAudio(0);
}

/**************************************************************************//**
	@brief 終わり
*//***************************************************************************/
void SceneCharacter_E4C::Finalize()
{
	for (const PlayerCharacter* character : m_previewCharacters)
	{
		delete character;
	}
	m_previewCharacters.clear();

	spritePreLoad.clear();
	UI.Clear();
	shadowMapRenderer->Clear();
	CameraManager::Instance().Clear();
	Sound::Instance().StopAudio(0);
	Sound::Instance().Finalize();

	T_GRAPHICS.GetShadowRenderer()->Finalize();
}

/**************************************************************************//**
	@brief		 更新処理
	@param[in]    elapsedTime
*//***************************************************************************/
void SceneCharacter_E4C::Update(float elapsedTime)
{
	stateMachine->Update(elapsedTime);

	cameraController->SyncCameraToController(CameraManager::Instance().GetCamera());

	for (PlayerCharacter* character : m_previewCharacters)
	{
		character->Update(elapsedTime);
	}

#ifdef _DEBUG
	// カメラ更新
	cameraController->Update();
	cameraController->SyncContrllerToCamera(CameraManager::Instance().GetCamera());

	m_background->Update(elapsedTime);

	//CameraManager::Instance().GetCamera()->Move2PointToCamera(CameraManager::Instance().GetCamera()->GetEye(), { 6.f,2.f,9.f }, CameraManager::Instance().GetCamera()->GetFocus(), { -3.0f, 0.0, 0.0f }, transitiontime, 2.f, elapsedTime);
#endif // _DEBUG

	UI.Update(elapsedTime);
}

/**************************************************************************//**
	@brief	 描画処理
*//***************************************************************************/
void SceneCharacter_E4C::Render()
{
	T_TEXT.Begin();

	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->Clear(T_GRAPHICS.GetDeviceContext(), 0.2f, 0.2f, 0.2f, 1);
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->SetRenderTarget(T_GRAPHICS.GetDeviceContext());

	// 描画コンテキスト設定
	RenderContext rc;
	rc.camera = CameraManager::Instance().GetCamera();
	rc.deviceContext = T_GRAPHICS.GetDeviceContext();
	rc.renderState = T_GRAPHICS.GetRenderState();

	// ライトの情報を詰め込む
	LightManager::Instance().PushRenderContext(rc);

	//	シャドウマップ描画
	shadowMapRenderer->Render();
	rc.shadowMapData = shadowMapRenderer->GetShadowMapData();

	for (PlayerCharacter* character : m_previewCharacters)
	{
		character->Render(rc);
	}

	UI.Render(rc);

	T_TEXT.End();

#ifdef _DEBUG
	// DebugIMGUI
	DrawSceneGUI();
	//shadowMapRenderer->DrawDebugGUI();
#endif // _DEBUG
}

/**************************************************************************//**
	@brief	DX12描画処理
*//***************************************************************************/
void SceneCharacter_E4C::RenderDX12()
{
	TentacleLib::graphics.BeginRender();
	{
		// シーン用定数バッファ更新
		const Descriptor* scene_cbv_descriptor = TentacleLib::graphics.UpdateSceneConstantBuffer(
			CameraManager::Instance().GetCamera(), 0, 0);

		// レンダーコンテキスト設定
		RenderContextDX12 rc;
		rc.d3d_command_list = m_frameBuffer->GetCommandList();
		rc.scene_cbv_descriptor = scene_cbv_descriptor;

		// 3Dモデル描画
		{
			m_frameBuffer->WaitUntilToPossibleSetRenderTarget(T_GRAPHICS.GetFrameBufferDX12(FrameBufferDX12Id::Scene));
			m_frameBuffer->SetRenderTarget(T_GRAPHICS.GetFrameBufferDX12(FrameBufferDX12Id::Scene));
			m_frameBuffer->Clear(T_GRAPHICS.GetFrameBufferDX12(FrameBufferDX12Id::Scene));

			// シャドウマップ
			{
				T_GRAPHICS.GetShadowRenderer()->Render(m_frameBuffer);
				rc.shadowMap.shadow_srv_descriptor = T_GRAPHICS.GetShadowRenderer()->GetShadowSRV();
				rc.shadowMap.shadow_sampler_descriptor = T_GRAPHICS.GetShadowRenderer()->GetShadowSampler();
			}

			m_background->RenderDX12(rc);

			for (auto& it : m_previewCharacters)
			{
				if (it != nullptr) {
					it->RenderDX12(rc);
				}
			}

			// レンダーターゲットへの書き込み終了待ち
			m_frameBuffer->WaitUntilFinishDrawingToRenderTarget(T_GRAPHICS.GetFrameBufferDX12(FrameBufferDX12Id::Scene));
		}

		// ポストエフェクト描画
		{
			postprocessingRenderer->Render(m_frameBuffer);
		}

		// 2D描画
		{
			T_TEXT.BeginDX12();

			UI.RenderDX12(rc);

			T_TEXT.EndDX12();
		}
	}
	DrawSceneGUI();
	T_GRAPHICS.GetImGUIRenderer()->RenderDX12(m_frameBuffer->GetCommandList());

	T_GRAPHICS.End();
}

/**************************************************************************//**
	@brief		キャラコントロール
	@param[in]    characterNumber
	@param[in]    modelType
	@param[in]    value
*//***************************************************************************/
void SceneCharacter_E4C::UpdateCurrentModel(int characterNumber, int modelType, int value)
{
}

/**************************************************************************//**
	@brief	ディバッグ描画
*//***************************************************************************/
void SceneCharacter_E4C::DrawSceneGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->Pos;
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	//if (ImGui::Begin("Scene##Debug", nullptr, ImGuiWindowFlags_None))
	//{
	//	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	//	{
	//		// カメラ
	//		DirectX::XMFLOAT3 eye = CameraManager::Instance().GetCamera()->GetEye();
	//		ImGui::DragFloat3("Eye", &eye.x, 0.01f, 100.0f);
	//		DirectX::XMFLOAT3 focus = CameraManager::Instance().GetCamera()->GetFocus();
	//		ImGui::DragFloat3("Fcous", &focus.x, 0.01f, 100.0f);
	//		DirectX::XMFLOAT3 up = CameraManager::Instance().GetCamera()->GetUp();
	//		ImGui::DragFloat3("Up", &up.x, 0.01f, 100.0f);

	//		CameraManager::Instance().GetCamera()->SetLookAt(eye, focus, up);
	//		cameraController->SyncCameraToController(CameraManager::Instance().GetCamera());
	//	}
	//}
	//ImGui::End();
}
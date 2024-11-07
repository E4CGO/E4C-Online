#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Network/HttpRequest.h"
#include "TAKOEngine/Rendering/ResourceManager.h"
#include "TAKOEngine/GUI/UIManager.h"
#include "TAKOEngine/Editor/Camera/CameraManager.h"

#include <imgui.h>
#include <string>

#include "Scene/SceneManager.h"
#include "SceneCharacter_E4C.h"
#include "SceneCharacter_E4CState.h"

#include "GameData.h"

// ディフォルト値
float SceneCharacter_E4C::m_time{ 0 };
const int SceneCharacter_E4C::m_maxCharacters{ 3 };

/**************************************************************************//**
	@brief	初期化
*//***************************************************************************/
void SceneCharacter_E4C::Initialize()
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


	//シャドウマップレンダラ
	shadowMapRenderer->Initialize();

	// フレームバッファマネージャー
	m_frameBuffer = T_GRAPHICS.GetFrameBufferManager();

	// モデル
	{
		m_sprites[0] = std::make_unique<SpriteDX12>(1, "Data/Sprites/UI/start.png");
		m_sprites[0] = std::make_unique<SpriteDX12>(1, "Data/Sprites/UI/exit.png");
	}

	m_previewCharacters.resize(m_maxCharacters);

	// 光
	LightManager::Instance().SetAmbientColor({ 0, 0, 0, 0 });
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

	// ステート
	stateMachine = std::make_unique<StateMachine<SceneCharacter_E4C>>();
	stateMachine->RegisterState(STATE::INIT, new SceneCharacter_E4CState::InitState(this));
	stateMachine->RegisterState(STATE::CHARACTERSELECTION, new SceneCharacter_E4CState::CharacterSelectionState(this));
	stateMachine->RegisterState(STATE::CHARACTERCREATION, new SceneCharacter_E4CState::CharacterCreationState(this));
	stateMachine->RegisterState(STATE::START, new SceneCharacter_E4CState::StartState(this));
	stateMachine->SetState(STATE::INIT);
}

/**************************************************************************//**
	@brief 終わり
*//***************************************************************************/
void SceneCharacter_E4C::Finalize()
{
	spritePreLoad.clear();
	UI.Clear();
	shadowMapRenderer->Clear();
	CameraManager::Instance().Clear();
}

/**************************************************************************//**
	@brief		 更新処理
	@param[in]    elapsedTime
*//***************************************************************************/
void SceneCharacter_E4C::Update(float elapsedTime)
{
	m_time += elapsedTime;

	for (auto& it : m_previewCharacters)
	{
		if (it != nullptr)
			it->Update(elapsedTime);
	}

	stateMachine->Update(elapsedTime);

#ifdef _DEBUG
	// カメラ更新
	cameraController->Update();
	cameraController->SyncContrllerToCamera(CameraManager::Instance().GetCamera());
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

	for (auto& it : m_previewCharacters)
	{
		if (it != nullptr) {
			if (it->GetMenuVisibility())
				it->Render(rc);
		}
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
			CameraManager::Instance().GetCamera());

		// レンダーコンテキスト設定
		RenderContextDX12 rc;
		rc.d3d_command_list = m_frameBuffer->GetCommandList();
		rc.scene_cbv_descriptor = scene_cbv_descriptor;

		// スプライト描画
		if (m_sprites[0] != nullptr)
		{
			m_sprites[0]->Begin(rc);
			m_sprites[0]->Draw(0, 0, 100, 100, 0, 1, 1, 1, 1);
			m_sprites[0]->End(m_frameBuffer->GetCommandList());
		}
	}
	TentacleLib::graphics.End();
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

	if (ImGui::Begin("Scene##Debug", nullptr, ImGuiWindowFlags_None))
	{
		if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// カメラ
			DirectX::XMFLOAT3 eye = CameraManager::Instance().GetCamera()->GetEye();
			ImGui::DragFloat3("Eye", &eye.x, 0.01f, 100.0f);
			DirectX::XMFLOAT3 focus = CameraManager::Instance().GetCamera()->GetFocus();
			ImGui::DragFloat3("Fcous", &focus.x, 0.01f, 100.0f);
			DirectX::XMFLOAT3 up = CameraManager::Instance().GetCamera()->GetUp();
			ImGui::DragFloat3("Up", &up.x, 0.01f, 100.0f);

			CameraManager::Instance().GetCamera()->SetLookAt(eye, focus, up);
			cameraController->SyncCameraToController(CameraManager::Instance().GetCamera());
		}
	}
	ImGui::End();
}
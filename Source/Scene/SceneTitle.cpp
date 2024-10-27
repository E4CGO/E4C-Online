#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Network/HttpRequest.h"
#include "TAKOEngine/Network/url.h"
#include "TAKOEngine/Rendering/ResourceManager.h"
#include "TAKOEngine/GUI/UIManager.h"
#include "TAKOEngine/Effects//EffectManager.h"

#include <imgui.h>
#include <string>

#include "Scene/SceneManager.h"
#include "Scene/SceneTitle.h"
#include "Scene/SceneGame.h"
#include "Scene/SceneTitleState.h"

#include "GameData.h"

//テスト用
float SceneTitle::time = { 0 };

void SceneTitle::Initialize()
{
	// Sprite Resource Preload
	for (auto& filename : spriteList)
	{
		spritePreLoad.insert(RESOURCE.LoadSpriteResource(filename));
	}

	//シャドウマップレンダラ
	shadowMapRenderer->Initialize();

	//スキニング
	m_skinning_pipeline = T_GRAPHICS.GetSkinningPipeline();

	// フレームバッファマネージャー
	m_framBuffer = T_GRAPHICS.GetFramBufferManager();

	// モデル
	{
		// 背景
		map = std::make_unique<ModelObject>("Data/Model/Map/TitleMap.glb");
		shadowMapRenderer->ModelRegister(map->GetModel().get());

		// キャラ
		barbarian = std::make_unique<ModelObject>("Data/Model/Character/Barbarian.glb");
		barbarian->SetAnimation(22, true, 0.0f);
		barbarian->SetPosition({ 0.28f, 0.0f, -3.15f });
		barbarian->SetAngle({ 0.0f, -0.38f, 0.0f });
		barbarian->GetModel()->FindNode("Barbarian_Round_Shield")->visible = false;
		barbarian->GetModel()->FindNode("Mug")->visible = false;
		barbarian->GetModel()->FindNode("1H_Axe")->visible = false;
		barbarian->GetModel()->FindNode("1H_Axe_Offhand")->visible = false;
		shadowMapRenderer->ModelRegister(barbarian->GetModel().get());

		knight = std::make_unique<ModelObject>("Data/Model/Character/Knight.glb");
		knight->SetAnimation(22, true, 0.0f);
		knight->SetPosition({ 1.08f, 0.0f, 2.12f });
		knight->SetAngle({ 0.0f, -1.22f, 0.0f });
		knight->GetModel()->FindNode("1H_Sword_Offhand")->visible = false;
		knight->GetModel()->FindNode("2H_Sword")->visible = false;
		knight->GetModel()->FindNode("Badge_Shield")->visible = false;
		knight->GetModel()->FindNode("Round_Shield")->visible = false;
		knight->GetModel()->FindNode("Spike_Shield")->visible = false;
		shadowMapRenderer->ModelRegister(knight->GetModel().get());

		rouge = std::make_unique<ModelObject>("Data/Model/Character/Rogue_Hooded.glb");
		rouge->SetAnimation(22, true, 0.0f);
		rouge->SetPosition({ -2.52f, 0.0f, -1.7f });
		rouge->SetAngle({ 0.0f, 0.55f, 0.0f });
		rouge->GetModel()->FindNode("1H_Crossbow")->visible = false;
		rouge->GetModel()->FindNode("Knife")->visible = false;
		rouge->GetModel()->FindNode("Knife_Offhand")->visible = false;
		rouge->GetModel()->FindNode("Throwable")->visible = false;
		shadowMapRenderer->ModelRegister(rouge->GetModel().get());

		//test = std::make_unique<ModelDX12>("Data/Model/Character/Barbarian.glb");
		test = std::make_unique<ModelDX12>("Data/Model/Enemy/Goblin.glb");
		test->PlayAnimation(0, true);

		float posX = 0;
		for (int i = 0; i < 3; ++i)
		{
			int id = test->AllocateInstancingIndex();
			if (id < 0) continue;

			DirectX::XMMATRIX m;
			m = DirectX::XMMatrixScaling(1, 1, 1);
			m *= DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(0));
			m *= DirectX::XMMatrixTranslation(posX, 0, 0);

			DirectX::XMFLOAT4X4 tm;
			DirectX::XMStoreFloat4x4(&tm, m);
			test->UpdateTransform(id, tm);

			posX += 2;
		}
		
		m_sprites[0] = std::make_unique<SpriteDX12>(1, "Data/Sprites/button_agree.png");

		// ルミナステクスチャ
		//m_sprites[1] = std::make_unique<SpriteDX12>(1, &T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Scene)->GetRenderTargetTexture());
	}

	// 光
	Light* dl = new Light(LightType::Directional);
	dl->SetDirection({ 0.0f, -0.503f, -0.864f });
	LightManager::Instance().Register(dl);
	shadowMapRenderer->SetShadowLight(dl);

	// カメラ設定
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),		// 画角
		SCREEN_W / SCREEN_H,					// 画面アスペクト比
		0.1f,									// ニアクリップ
		10000.0f								// ファークリップ
	);
	camera.SetLookAt(
		{ -5.661f, 2.5f, 5.584f },				// 視点
		{ 0.0f, 2.0, 0.0f },					// 注視点
		{ 0.036f, 0.999f, -0.035f }				// 上ベクトル
	);
	cameraController = std::make_unique<FreeCameraController>();
	cameraController->SyncCameraToController(camera);
	cameraController->SetEnable(false);

	// ステート
	stateMachine = std::make_unique<StateMachine<SceneTitle>>();
	stateMachine->RegisterState(STATE::TITLE, new SceneTitleState::TitleState(this));
	stateMachine->RegisterState(STATE::LOGIN_CHECK, new SceneTitleState::LoginCheckState(this));
	stateMachine->RegisterState(STATE::INPUT, new SceneTitleState::InputState(this));
	stateMachine->RegisterState(STATE::SETTING, new SceneTitleState::SettingState(this));
	stateMachine->RegisterState(STATE::LOGIN, new SceneTitleState::LoginState(this));
	stateMachine->SetState(STATE::TITLE);

	HttpRequest* xhr = new HttpRequest("https://drive.usercontent.google.com/download?id=1ojUCZZxOZdpU98Liv9xoxYNW9btwt60-");
	xhr->Send();
	if (xhr->GetThread())
	{
		xhr->GetThread()->join();
		if (xhr->GetState() == HttpRequest::STATE::RESPONSE)
		{
			HttpResponse response = xhr->GetResponse();
			std::string host = response.data["host"];
			GAME_DATA.SetOnline(response.data["online"]);
			Url::SetHost(host.c_str());
		}
	}
	delete xhr;
	if (GAME_DATA.IsOnline())
	{
		xhr = new HttpRequest(URL(Url::VERSION));
		xhr->Send();
		if (xhr->GetThread())
		{
			xhr->GetThread()->join();
			if (xhr->GetState() == HttpRequest::STATE::RESPONSE)
			{
				GAME_DATA.SetOnline(true);
			}
		}
		delete xhr;
	}

}

void SceneTitle::Finalize()
{
	spritePreLoad.clear();
	UI.Clear();
	shadowMapRenderer->Clear();
}

// 更新処理
void SceneTitle::Update(float elapsedTime)
{
	time += elapsedTime;

	stateMachine->Update(elapsedTime);

	map->Update(elapsedTime);
	barbarian->Update(elapsedTime);
	knight->Update(elapsedTime);
	rouge->Update(elapsedTime);

	if (test != nullptr && T_GRAPHICS.isDX12Active)
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(test_scale.x, test_scale.y, test_scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(
			DirectX::XMConvertToRadians(test_rotation.x),
			DirectX::XMConvertToRadians(test_rotation.y),
			DirectX::XMConvertToRadians(test_rotation.z));
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(test_position.x, test_position.y, test_position.z);
		DirectX::XMMATRIX TRANSFORM = S * R * T;
		DirectX::XMStoreFloat4x4(&test_transform, TRANSFORM);
		//test->SetTransformMatrix(test_transform);
		test->UpdateAnimation(elapsedTime);
		test->UpdateTransform();
	}

#ifdef _DEBUG
	// カメラ更新
	cameraController->Update();
	cameraController->SyncContrllerToCamera(camera);
#endif // _DEBUG

	UI.Update(elapsedTime);
}

// 描画処理
void SceneTitle::Render()
{
	T_TEXT.Begin();

	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->Clear(T_GRAPHICS.GetDeviceContext(), 0.2f, 0.2f, 0.2f, 1);
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->SetRenderTarget(T_GRAPHICS.GetDeviceContext());

	// 描画コンテキスト設定
	RenderContext rc;
	rc.camera = &camera;
	rc.deviceContext = T_GRAPHICS.GetDeviceContext();
	rc.renderState = T_GRAPHICS.GetRenderState();

	// ライトの情報を詰め込む
	LightManager::Instance().PushRenderContext(rc);

	//	シャドウマップ描画
	shadowMapRenderer->Render();
	rc.shadowMapData = shadowMapRenderer->GetShadowMapData();

	map->Render(rc);
	barbarian->Render(rc);
	knight->Render(rc);
	rouge->Render(rc);

	UI.Render(rc);

	T_TEXT.End();

#ifdef _DEBUG
	// DebugIMGUI
	//DrawSceneGUI();
	shadowMapRenderer->DrawDebugGUI();
#endif // _DEBUG
}

void SceneTitle::RenderDX12()
{
	T_GRAPHICS.BeginRender();
	{
		// シーン用定数バッファ更新
		const Descriptor* scene_cbv_descriptor = T_GRAPHICS.UpdateSceneConstantBuffer(
			Camera::Instance());

		// レンダーコンテキスト設定
		RenderContextDX12 rc;
		rc.d3d_command_list = m_framBuffer->GetCommandList();
		rc.scene_cbv_descriptor = scene_cbv_descriptor;

		//スキニング
		test->UpdateFrameResource(test_transform);
		m_skinning_pipeline->Compute(rc, test.get());

		m_framBuffer->WaitUntilToPossibleSetRenderTarget(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Scene));
		m_framBuffer->SetRenderTarget(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Scene));
		m_framBuffer->Clear(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Scene));

		// モデル描画
		ModelShaderDX12* shader = T_GRAPHICS.GetModelShaderDX12(ModelShaderDX12Id::ToonInstancing);
		if (test != nullptr)
		{
			shader->Render(rc, test.get());
		}

		m_framBuffer->WaitUntilFinishDrawingToRenderTarget(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Scene)); 

		// 画面に表示されるレンダリングターゲットに戻す
		m_framBuffer->SetRenderTarget(T_GRAPHICS.GetCurrentFrameBuffuerRTV(), T_GRAPHICS.GetCurrentFrameBuffuerDSV());

		//m_framBuffer->WaitUntilToPossibleSetRenderTarget(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Luminance));
		//m_framBuffer->SetRenderTarget(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Luminance));
		//m_framBuffer->Clear(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Luminance));

		// スプライト描画
		SpriteShaderDX12* sprite = T_GRAPHICS.GetSpriteShaderDX12(SpriteShaderDX12Id::LuminanceExtraction);
		if (m_sprites[0] != nullptr)
		{
			m_sprites[0]->Begin(rc);
			m_sprites[0]->Draw(0, 0, 100, 100, 0, 1, 1, 1, 1);
			sprite->Render(rc, m_sprites[0].get());
		}

		//m_framBuffer->WaitUntilFinishDrawingToRenderTarget(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Luminance));

		//EFFECTS.GetEffect(EffectManager::EFFECT_IDX::BOMB_EFFECT)->PlayDX12(DirectX::XMFLOAT3(0.f, 0.f, 0.f), 5.0f);

		//EFFECTS.RenderDX12(camera.GetView(), camera.GetProjection());

		// IMGUI描画処理
		{
			{
				ImVec2 displaySize = ImGui::GetIO().DisplaySize;
				ImVec2 pos = ImVec2{ 0,0 };
				float width = 210;
				float height = 460;
				ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y), ImGuiCond_Once);
				ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Once);

				if (ImGui::Begin("Debug"))
				{
					if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::DragFloat3("Position", &test_position.x, 1.0f);
						ImGui::DragFloat3("Rotation", &test_rotation.x, 1.0f);
						ImGui::DragFloat3("Scale", &test_scale.x, 1.01f);
					}
				}
				ImGui::End();
			}

			T_GRAPHICS.GetImGUIRenderer()->RenderDX12(m_framBuffer->GetCommandList());
		}	
	}
	T_GRAPHICS.End();
}

void SceneTitle::DrawSceneGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->Pos;
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Scene##Debug", nullptr, ImGuiWindowFlags_None))
	{
		if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// カメラ
			DirectX::XMFLOAT3 eye = camera.GetEye();
			ImGui::DragFloat3("Eye", &eye.x, 0.01f, 100.0f);
			DirectX::XMFLOAT3 focus = camera.GetFocus();
			ImGui::DragFloat3("Fcous", &focus.x, 0.01f, 100.0f);
			DirectX::XMFLOAT3 up = camera.GetUp();
			ImGui::DragFloat3("Up", &up.x, 0.01f, 100.0f);
		}

		float angleLimit = DirectX::XM_PI;
		// ナイト
		if (ImGui::CollapsingHeader("Knight", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DirectX::XMFLOAT3 position = knight->GetPosition();
			ImGui::DragFloat3("Position##Knight", &position.x, 0.01f, 100.0f);
			knight->SetPosition(position);
			DirectX::XMFLOAT3 angle = knight->GetAngle();
			ImGui::DragFloat3("Angle##Knight", &angle.x, 0.01f, -angleLimit, angleLimit);
			knight->SetAngle(angle);
		}

		// バーバリアン
		if (ImGui::CollapsingHeader("Barbarian", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DirectX::XMFLOAT3 position = barbarian->GetPosition();
			ImGui::DragFloat3("Position##Barbarian", &position.x, 0.01f, 100.0f);
			barbarian->SetPosition(position);
			DirectX::XMFLOAT3 angle = barbarian->GetAngle();
			ImGui::DragFloat3("Angle##Barbarian", &angle.x, 0.01f, -angleLimit, angleLimit);
			barbarian->SetAngle(angle);
		}

		// レンジャー
		if (ImGui::CollapsingHeader("Rouge", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DirectX::XMFLOAT3 position = rouge->GetPosition();
			ImGui::DragFloat3("Position##Rouge", &position.x, 0.01f, 100.0f);
			rouge->SetPosition(position);
			DirectX::XMFLOAT3 angle = rouge->GetAngle();
			ImGui::DragFloat3("Angle##Rouge", &angle.x, 0.01f, -angleLimit, angleLimit);
			rouge->SetAngle(angle);
		}
	}
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 500), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Mouse##Debug", nullptr, ImGuiWindowFlags_None))
	{
		if (ImGui::CollapsingHeader("Mouse", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DirectX::XMFLOAT2 mousePos = T_INPUT.GetMousePos();
			ImGui::InputFloat2("Position##MouseTittle", &mousePos.x);
			DirectX::XMFLOAT2 mouseWinPos = T_INPUT.GetMouseWinPos();
			ImGui::InputFloat2("Window Position##MouseTittle", &mouseWinPos.x);
			DirectX::XMFLOAT2 mouseDetla = { T_INPUT.GetMouseDeltaX(), T_INPUT.GetMouseDeltaY() };
			ImGui::InputFloat2("Detla##MouseTittle", &mouseDetla.x);
			DirectX::XMFLOAT2 mouseScroll = { T_INPUT.GetMouseScrollX(), T_INPUT.GetMouseScrollY() };
			ImGui::InputFloat2("Scroll##MouseTittle", &mouseScroll.x);
		}
	}
	ImGui::End();
}
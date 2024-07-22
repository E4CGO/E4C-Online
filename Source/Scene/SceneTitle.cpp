#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Network/HttpRequest.h"
#include "TAKOEngine/Network/url.h"
#include "TAKOEngine/Rendering/ResourceManager.h"
#include "TAKOEngine/GUI/UIManager.h"

#include <imgui.h>
#include <string>

#include "Scene/SceneManager.h"
#include "Scene/SceneTitle.h"
#include "Scene/SceneGame.h"
#include "Scene/SceneTitleState.h"

#include "GameData.h"

void SceneTitle::Initialize()
{
	// Sprite Resource Preload
	for (auto& filename : spriteList)
	{
		spritePreLoad.insert(RESOURCE.LoadSpriteResource(filename));
	}

	//�V���h�E�}�b�v�����_��
	shadowMapRenderer->Initialize();

	// ���f��
	{
		// �w�i
		map = std::make_unique<ModelObject>("Data/Model/Map/TitleMap.glb");
		shadowMapRenderer->ModelRegister(map->GetModel().get());

		// �L����
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

		test = std::make_unique<ModelDX12>("Data/Model/Character/Barbarian.glb");
		test->PlayAnimation(0, true);
	}

	// ��
	LightManager::Instance().SetAmbientColor({ 0, 0, 0, 0 });
	Light* dl = new Light(LightType::Directional);
	dl->SetDirection({ 0.0f, -0.503f, -0.864f });
	LightManager::Instance().Register(dl);
	shadowMapRenderer->SetShadowLight(dl);

	// �J�����ݒ�
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),							// ��p
		SCREEN_W / SCREEN_H,	// ��ʃA�X�y�N�g��
		0.1f,														// �j�A�N���b�v
		10000.0f													// �t�@�[�N���b�v
	);
	camera.SetLookAt(
		{ -5.661f, 2.5f, 5.584f },	// ���_
		{ 0.0f, 2.0, 0.0f },	// �����_
		{ 0.036f, 0.999f, -0.035f } // ��x�N�g��
	);
	cameraController = std::make_unique<FreeCameraController>();
	cameraController->SyncCameraToController(camera);
	cameraController->SetEnable(false);

	// �X�e�[�g
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

// �X�V����
void SceneTitle::Update(float elapsedTime)
{
	stateMachine->Update(elapsedTime);

	map->Update(elapsedTime);
	barbarian->Update(elapsedTime);
	knight->Update(elapsedTime);
	rouge->Update(elapsedTime);

	if (test != nullptr)
	{
		const DirectX::XMFLOAT4X4 w = { 1.f,0.f,0.f,0.f, 0.f,1.f,0.f,0.f, 0.f,0.f,1.f,0.f, 0.f,0.f,0.f,1.f };
		test->UpdateAnimation(elapsedTime);
		test->UpdateTransform(w);
	}

#ifdef _DEBUG
	// �J�����X�V
	cameraController->Update();
	cameraController->SyncContrllerToCamera(camera);
#endif // _DEBUG

	UI.Update(elapsedTime);
}

// �`�揈��
void SceneTitle::Render()
{
	T_TEXT.Begin();

	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->Clear(T_GRAPHICS.GetDeviceContext(), 0.2f, 0.2f, 0.2f, 1);
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->SetRenderTarget(T_GRAPHICS.GetDeviceContext());
	
	// �`��R���e�L�X�g�ݒ�
	RenderContext rc;
	rc.camera = &camera;
	rc.deviceContext = T_GRAPHICS.GetDeviceContext();
	rc.renderState   = T_GRAPHICS.GetRenderState();

	// ���C�g�̏����l�ߍ���
	LightManager::Instance().PushRenderContext(rc);

	//	�V���h�E�}�b�v�`��
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
	ID3D12GraphicsCommandList* d3d_command_list = TentacleLib::graphics.Begin();
	{
		Camera& camera = Camera::Instance();

		// �V�[���p�萔�o�b�t�@�X�V
		Descriptor* scene_cbv_descriptor = TentacleLib::graphics.UpdateSceneConstantBuffer(
			camera.GetView(),
			camera.GetProjection(),
			DirectX::XMFLOAT3(0, -1, 0)
		);

		// �����_�[�R���e�L�X�g�ݒ�
		RenderContextDX12 rc;
		rc.d3d_command_list = d3d_command_list;
		rc.scene_cbv_descriptor = scene_cbv_descriptor;

		// ���f���`��
		Shader* shader = TentacleLib::graphics.GetShader();
		shader->Begin(rc);
		if (test != nullptr)
		{
			shader->Draw(rc, test.get());
		}
		shader->End(rc);
	}
	TentacleLib::graphics.End();
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
			// �J����
			DirectX::XMFLOAT3 eye = camera.GetEye();
			ImGui::DragFloat3("Eye", &eye.x, 0.01f, 100.0f);
			DirectX::XMFLOAT3 focus = camera.GetFocus();
			ImGui::DragFloat3("Fcous", &focus.x, 0.01f, 100.0f);
			DirectX::XMFLOAT3 up = camera.GetUp();
			ImGui::DragFloat3("Up", &up.x, 0.01f, 100.0f);
		}

		float angleLimit = DirectX::XM_PI;
		// �i�C�g
		if (ImGui::CollapsingHeader("Knight", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DirectX::XMFLOAT3 position = knight->GetPosition();
			ImGui::DragFloat3("Position##Knight", &position.x, 0.01f, 100.0f);
			knight->SetPosition(position);
			DirectX::XMFLOAT3 angle = knight->GetAngle();
			ImGui::DragFloat3("Angle##Knight", &angle.x, 0.01f, -angleLimit, angleLimit);
			knight->SetAngle(angle);
		}

		// �o�[�o���A��
		if (ImGui::CollapsingHeader("Barbarian", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DirectX::XMFLOAT3 position = barbarian->GetPosition();
			ImGui::DragFloat3("Position##Barbarian", &position.x, 0.01f, 100.0f);
			barbarian->SetPosition(position);
			DirectX::XMFLOAT3 angle = barbarian->GetAngle();
			ImGui::DragFloat3("Angle##Barbarian", &angle.x, 0.01f, -angleLimit, angleLimit);
			barbarian->SetAngle(angle);
		}

		// �����W���[
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
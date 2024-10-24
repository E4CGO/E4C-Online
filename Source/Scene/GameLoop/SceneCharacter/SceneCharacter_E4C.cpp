#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Network/HttpRequest.h"
#include "TAKOEngine/Rendering/ResourceManager.h"
#include "TAKOEngine/GUI/UIManager.h"

#include <imgui.h>
#include <string>

#include "Scene/SceneManager.h"
#include "SceneCharacter_E4C.h"
#include "SceneCharacter_E4CState.h"

#include "GameData.h"

//テスト用
float SceneCharacter_E4C::time = { 0 };

void SceneCharacter_E4C::Initialize()
{
	// Sprite Resource Preload
	for (auto& filename : spriteList)
	{
		spritePreLoad.insert(RESOURCE.LoadSpriteResource(filename));
	}

	//シャドウマップレンダラ
	shadowMapRenderer->Initialize();

	// モデル
	{
		m_sprites[0] = std::make_unique<SpriteDX12>(1, "Data/Sprites/UI/start.png");
		m_sprites[0] = std::make_unique<SpriteDX12>(1, "Data/Sprites/UI/exit.png");
	}

	m_Characters_LEFT.resize(3);
	m_Characters_CENTER.resize(3);
	m_Characters_RIGHT.resize(3);

	m_character_BARB_HEAD = std::make_shared<ModelObject>("Data/Model/Character/BODY_BARB.glb");
	m_character_BARB_BODY = std::make_shared<ModelObject>("Data/Model/Character/HEAD_BARB.glb");
	m_character_BARB_WEAPON = std::make_shared<ModelObject>("Data/Model/Character/WEAPON_BARB.glb");

	m_character_MAGE_HEAD = std::make_shared<ModelObject>("Data/Model/Character/BODY_MAGE.glb");
	m_character_MAGE_BODY = std::make_shared<ModelObject>("Data/Model/Character/HEAD_MAGE.glb");
	m_character_MAGE_WEAPON = std::make_shared<ModelObject>("Data/Model/Character/WEAPON_MAGE.glb");

	// 光
	LightManager::Instance().SetAmbientColor({ 0, 0, 0, 0 });
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
	stateMachine = std::make_unique<StateMachine<SceneCharacter_E4C>>();
	stateMachine->RegisterState(STATE::INIT, new SceneCharacter_E4CState::InitState(this));
	stateMachine->RegisterState(STATE::CHARACTERSELECTION, new SceneCharacter_E4CState::CharacterSelectionState(this));
	stateMachine->RegisterState(STATE::CHARACTERCREATION, new SceneCharacter_E4CState::CharacterCreationState(this));
	stateMachine->RegisterState(STATE::START, new SceneCharacter_E4CState::StartState(this));
	stateMachine->SetState(STATE::INIT);
}

void SceneCharacter_E4C::Finalize()
{
	spritePreLoad.clear();
	UI.Clear();
	shadowMapRenderer->Clear();
}

// 更新処理
void SceneCharacter_E4C::Update(float elapsedTime)
{
	time += elapsedTime;

	stateMachine->Update(elapsedTime);

	if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[0].visible != 0)
	{
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[0].Character.bodyType != 0)
		{
			m_Characters_LEFT[0]->Update(elapsedTime);
		}
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[0].Character.headType != 0)
		{
			m_Characters_LEFT[1]->Update(elapsedTime);
		}
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[0].Character.weaponType != 0)
		{
			m_Characters_LEFT[2]->Update(elapsedTime);
		}
	}

	if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[1].visible != 0)
	{
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[1].Character.bodyType != 0)
		{
			m_Characters_CENTER[0]->Update(elapsedTime);
		}
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[1].Character.headType != 0)
		{
			m_Characters_CENTER[1]->Update(elapsedTime);
		}
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[1].Character.weaponType != 0)
		{
			m_Characters_CENTER[2]->Update(elapsedTime);
		}
	}

	if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[2].visible != 0)
	{
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[2].Character.bodyType != 0)
		{
			m_Characters_RIGHT[0]->Update(elapsedTime);
		}
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[2].Character.headType != 0)
		{
			m_Characters_RIGHT[1]->Update(elapsedTime);
		}
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[2].Character.weaponType != 0)
		{
			m_Characters_RIGHT[2]->Update(elapsedTime);
		}
	}

#ifdef _DEBUG
	// カメラ更新
	cameraController->Update();
	cameraController->SyncContrllerToCamera(camera);
#endif // _DEBUG

	UI.Update(elapsedTime);
}

// 描画処理
void SceneCharacter_E4C::Render()
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

	if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[0].visible != 0)
	{
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[0].Character.headType != 0)
		{
			m_Characters_LEFT[0]->Render(rc);
		}
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[0].Character.bodyType != 0)
		{
			m_Characters_LEFT[1]->Render(rc);
		}
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[0].Character.weaponType != 0)
		{
			m_Characters_LEFT[2]->Render(rc);
		}
	}

	if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[1].visible != 0)
	{
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[1].Character.headType != 0)
		{
			m_Characters_CENTER[0]->Render(rc);
		}
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[1].Character.bodyType != 0)
		{
			m_Characters_CENTER[1]->Render(rc);
		}
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[1].Character.weaponType != 0)
		{
			m_Characters_CENTER[2]->Render(rc);
		}
	}

	if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[2].visible != 0)
	{
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[2].Character.headType != 0)
		{
			m_Characters_RIGHT[0]->Render(rc);
		}
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[2].Character.bodyType != 0)
		{
			m_Characters_RIGHT[1]->Render(rc);
		}
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[2].Character.weaponType != 0)
		{
			m_Characters_RIGHT[2]->Render(rc);
		}
	}

	UI.Render(rc);

	T_TEXT.End();

#ifdef _DEBUG
	// DebugIMGUI
	//DrawSceneGUI();
	//shadowMapRenderer->DrawDebugGUI();
#endif // _DEBUG
}

void SceneCharacter_E4C::RenderDX12()
{
	ID3D12GraphicsCommandList* d3d_command_list = TentacleLib::graphics.Begin();
	{
		Camera& camera = Camera::Instance();

		// シーン用定数バッファ更新
		const Descriptor* scene_cbv_descriptor = TentacleLib::graphics.UpdateSceneConstantBuffer(
			Camera::Instance(),
			DirectX::XMFLOAT3(0, -1, 0));

		// レンダーコンテキスト設定
		RenderContextDX12 rc;
		rc.d3d_command_list = d3d_command_list;
		rc.scene_cbv_descriptor = scene_cbv_descriptor;

		// スプライト描画
		if (m_sprites[0] != nullptr)
		{
			m_sprites[0]->Begin(rc);
			m_sprites[0]->Draw(0, 0, 100, 100, 0, 1, 1, 1, 1);
			m_sprites[0]->End(d3d_command_list);
		}

		TentacleLib::graphics.End();
	}
}

void SceneCharacter_E4C::UpdateCurrentModel(int characterNumber, int modelType, int value)
{
	switch (characterNumber)
	{
	case 0:
		switch (modelType)
		{
		case 0:
			switch (value)
			{
			case 0:
				break;
			case 1:
				m_Characters_LEFT[0] = m_character_BARB_HEAD;
				break;
			case 2:
				m_Characters_LEFT[0] = m_character_MAGE_HEAD;
				break;
			default:
				break;
			}
			break;
		case 1:
			switch (value)
			{
			case 0:
				break;
			case 1:
				m_Characters_LEFT[1] = m_character_BARB_BODY;
				break;
			case 2:
				m_Characters_LEFT[1] = m_character_MAGE_BODY;
				break;
			default:
				break;
			}
			break;
		case 2:
			switch (value)
			{
			case 0:
				break;
			case 1:
				m_Characters_LEFT[2] = m_character_BARB_WEAPON;
				break;
			case 2:
				m_Characters_LEFT[2] = m_character_MAGE_WEAPON;
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		break;
	case 1:
		switch (modelType)
		{
		case 0:
			switch (value)
			{
			case 0:

				break;
			case 1:
				m_Characters_CENTER[0] = m_character_BARB_HEAD;
				break;
			case 2:
				m_Characters_CENTER[0] = m_character_MAGE_HEAD;
				break;
			default:
				break;
			}
			break;
		case 1:
			switch (value)
			{
			case 0:

				break;
			case 1:
				m_Characters_CENTER[1] = m_character_BARB_BODY;
				break;
			case 2:
				m_Characters_CENTER[1] = m_character_MAGE_BODY;
				break;
			default:
				break;
			}
			break;
		case 2:
			switch (value)
			{
			case 0:

				break;
			case 1:
				m_Characters_CENTER[2] = m_character_BARB_WEAPON;
				break;
			case 2:
				m_Characters_CENTER[2] = m_character_MAGE_WEAPON;
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	case 2:
		switch (modelType)
		{
		case 0:
			switch (value)
			{
			case 0:
				break;
			case 1:
				m_Characters_RIGHT[0] = m_character_BARB_HEAD;
				break;
			case 2:
				m_Characters_RIGHT[0] = m_character_MAGE_HEAD;
				break;
			default:
				break;
			}
			break;
		case 1:
			switch (value)
			{
			case 0:

				break;
			case 1:
				m_Characters_RIGHT[1] = m_character_BARB_BODY;
				break;
			case 2:
				m_Characters_RIGHT[1] = m_character_MAGE_BODY;
				break;
			default:
				break;
			}
			break;
		case 2:
			switch (value)
			{
			case 0:

				break;
			case 1:
				m_Characters_RIGHT[2] = m_character_BARB_WEAPON;
				break;
			case 2:
				m_Characters_RIGHT[2] = m_character_MAGE_WEAPON;
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
}

void SceneCharacter_E4C::DrawSceneGUI()
{
}
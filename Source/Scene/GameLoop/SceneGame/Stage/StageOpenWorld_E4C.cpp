#include "StageOpenWorld_E4C.h"

#include "GameObject/ModelObject.h"
#include "GameObject/Character/Player/Barbarian.h"
#include "GameObject/Props/Teleporter.h"

#include "Scene/Scene.h"
#include "Scene/Stage/StageManager.h"
#include "Scene/Stage/Stage.h"

#include "Map/MapTileManager.h"
#include "Map/MapTile.h"

#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"
#include "TAKOEngine/Tool/GLTFImporter.h"
#include "TAKOEngine/Tool/Timer.h"

static float timer = 0;

void StageOpenWorld_E4C::Initialize()
{
	Stage::Initialize(); // デフォルト

	stage_collision = new MapTile("Data/Model/Stage/Terrain_Collision.glb", 0.01f);
	stage_collision->Update(0);
	MAPTILES.Register(stage_collision);

	map = std::make_unique<gltf_model>(T_GRAPHICS.GetDevice(), "Data/Model/Stage/Terrain_Map.glb");

	PlayerCharacterData::CharacterInfo charaInfo = {
		true,			// visible
		"",				// save
		{				//Character
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}
	};

	player = std::make_unique<PlayerCharacter>(charaInfo);
	player->SetPosition({ 5,	10, 5 });
	player->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::State::Idle));

	//teleporter = std::make_unique<Teleporter>("Data/Model/Cube/testCubes.glb", 1.0);
	//teleporter->SetPosition({ 50, 0, 60 });

	std::array<DirectX::XMFLOAT3, 4 > positions = {
	DirectX::XMFLOAT3{ 10.0f, 10.0f, 5.0f},
	DirectX::XMFLOAT3{ 10.0f, 20.0f, 5.0f },
	DirectX::XMFLOAT3{ 5.0f, 10.0f, 5.0f },
	DirectX::XMFLOAT3{ 5.0f, 20.0f, 5.0f }
	};

	plane = std::make_unique<Plane>(T_GRAPHICS.GetDevice(), "Data/Sprites/gem.png", 1.0f, positions);

	positions = {
		DirectX::XMFLOAT3{ 15.0f, 15.0f, 5.0f},
		DirectX::XMFLOAT3{ 15.0f, 25.0f, 5.0f },
		DirectX::XMFLOAT3{ 25.0f, 15.0f, 5.0f },
		DirectX::XMFLOAT3{ 25.0f, 25.0f, 5.0f }
	};

	portal = std::make_unique<Plane>(T_GRAPHICS.GetDevice(), "", 1.0f, positions);
	portal.get()->SetShader(ModelShaderId::Portal);

	// 光
	LightManager::Instance().SetAmbientColor({ 0, 0, 0, 0 });
	Light* dl = new Light(LightType::Directional);
	dl->SetDirection({ 0.0f, -0.503f, -0.864f });
	LightManager::Instance().Register(dl);

	// カメラ設定
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),							// 画角
		T_GRAPHICS.GetScreenWidth() / T_GRAPHICS.GetScreenHeight(),	// 画面アスペクト比
		0.1f,														// ニアクリップ
		10000.0f													// ファークリップ
	);
	camera.SetLookAt(
		{ 0, 5.0f, 10.0f },	// 視点
		{ 0, 0, 0 },	// 注視点
		{ 0, 0.969f, -0.248f } // 上ベクトル
	);

	cameraController = std::make_unique<ThridPersonCameraController>();
	cameraController->SyncCameraToController(camera);
	cameraController->SetEnable(true);
	cameraController->SetPlayer(player.get());

	{
		HRESULT hr;

		D3D11_BUFFER_DESC buffer_desc{};
		buffer_desc.ByteWidth = (sizeof(CbScene) + 15) / 16 * 16;
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		hr = T_GRAPHICS.GetDevice()->CreateBuffer(&buffer_desc, nullptr, constant_buffers[1].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

void StageOpenWorld_E4C::Update(float elapsedTime)
{
	cameraController->Update(elapsedTime);
	cameraController->SyncContrllerToCamera(camera);

	player->Update(elapsedTime);
	//teleporter->Update(elapsedTime);
	plane->Update(elapsedTime);
	portal->Update(elapsedTime);

	//teleporter->CheckPlayer(player->GetPosition(), elapsedTime);

	//if (teleporter->GetPortalReady()) STAGES.stageNumber = 1;

	if (T_INPUT.KeyDown(VK_F2))
	{
		PlayerCharacterData::CharacterInfo charInfo = {
			true,			// visible
			"",				// save
			{				//Character
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1
			}
		};
		//player->
		player->LoadAppearance(charInfo.Character.pattern);
	}

	timer += elapsedTime;
}

void StageOpenWorld_E4C::Render()
{
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->Clear(T_GRAPHICS.GetDeviceContext(), 0.2f, 0.2f, 0.2f, 1);
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->SetRenderTarget(T_GRAPHICS.GetDeviceContext());

	// 描画コンテキスト設定
	RenderContext rc;
	rc.camera = &camera;
	rc.deviceContext = T_GRAPHICS.GetDeviceContext();
	rc.renderState = T_GRAPHICS.GetRenderState();

	rc.timerGlobal = timer;
	rc.timerTick = TentacleLib::Timer::Instance().Delta();

	// ライトの情報を詰め込む
	LightManager::Instance().PushRenderContext(rc);

	// 描画
	player->Render(rc);

	float time = 0;

	{
		static std::vector<gltf_model::node> animated_nodes{ map->nodes };

		//map->animate(0/*animation index*/, time, animated_nodes);

		const DirectX::XMFLOAT4X4 coordinate_system_transforms[]
		{
			{-1, 0, 0, 0,
			  0, 1, 0, 0,
			  0, 0, 1, 0,
			  0, 0, 0, 1}, // 0:RHS Y-UP

			{ 1, 0, 0, 0,
			  0, 1, 0, 0,
			  0, 0, 1, 0,
			  0, 0, 0, 1}, // 1:LHS Y-UP

			{-1, 0, 0, 0,
			  0, 0,-1, 0,
			  0, 1, 0, 0,
			  0, 0, 0, 1}, // 2:RHS Z-UP

			{ 1, 0, 0, 0,
			  0, 0, 1, 0,
			  0, 1, 0, 0,
			  0, 0, 0, 1}, //3:LHS Z - UP
		};

		float scale_factor = 1.0f;

		DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&coordinate_system_transforms[0]) * DirectX::XMMatrixScaling(scale_factor, scale_factor, scale_factor) };
		DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(1.0, 1.0f, 1.0f) };
		DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f) };
		DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f) };

		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&world, C * S * R * T);

		// シーン用定数バッファ更新
		CbScene cbScene{};
		DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.camera->GetView());
		DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.camera->GetProjection());
		DirectX::XMStoreFloat4x4(&cbScene.view_projection, V * P);

		const DirectX::XMFLOAT3& eye = rc.camera->GetEye();
		cbScene.camera_position.x = eye.x;
		cbScene.camera_position.y = eye.y;
		cbScene.camera_position.z = eye.z;

		// レンダーステート設定
		const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		rc.deviceContext->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Opaque), blend_factor, 0xFFFFFFFF);
		rc.deviceContext->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
		rc.deviceContext->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));

		rc.deviceContext->UpdateSubresource(constant_buffers[1].Get(), 0, 0, &cbScene, 0, 0);
		rc.deviceContext->VSSetConstantBuffers(1, 1, constant_buffers[1].GetAddressOf());
		rc.deviceContext->PSSetConstantBuffers(1, 1, constant_buffers[1].GetAddressOf());

		map->render(rc, world, animated_nodes);
	}

	//teleporter->Render(rc);
	plane->Render(rc);
	portal->Render(rc);
}

void StageOpenWorld_E4C::OnPhase()
{
}
﻿#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Network/HttpRequest.h"
#include "TAKOEngine/Rendering/ResourceManager.h"
#include "TAKOEngine/GUI/UIManager.h"

#include "TAKOEngine/Sound/Sound.h"
#include "TAKOEngine/Editor/Camera/CameraManager.h"
#include "TAKOEngine/Network/WinSock2Wrapper.h"
#include "TAKOEngine/Tool/Console.h"

#include <commdlg.h>
#include <imgui.h>
#include <string>
#include <algorithm>
#include <fstream>

#include "Scene/SceneManager.h"
#include "SceneRoomTest_E4C.h"

#include "Map/MapTile.h"
#include "Map/MapTileManager.h"

#include "GameData.h"

void SceneRoomTest_E4C::Initialize()
{
	// Sprite Resource Preload
	for (auto& filename : m_spriteList)
	{
		m_spritePreLoad.insert(RESOURCE.LoadSpriteResource(filename));
	}

	//シャドウマップレンダラ
	m_shadowMapRenderer->Initialize();

	// フレームバッファマネージャー
	m_frameBuffer = T_GRAPHICS.GetFrameBufferManager();

	// 光
	LightManager::Instance().SetAmbientColor({ 0.3f, 0.3f, 0.3f, 0 });
	Light* dl = new Light(LightType::Directional);
	dl->SetDirection({ 0.0f, -0.503f, -0.864f });
	LightManager::Instance().Register(dl);
	m_shadowMapRenderer->SetShadowLight(dl);

	CameraManager& cameramanager = CameraManager::Instance();
	Camera* mainCamera = new Camera();
	cameramanager.Register(mainCamera);
	cameramanager.SetCamera(0);

	// カメラ設定
	CameraManager::Instance().GetCamera()->SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),		// 画角
		SCREEN_W / SCREEN_H,					// 画面アスペクト比
		0.1f,									// ニアクリップ
		10000.0f								// ファークリップ
	);
	CameraManager::Instance().GetCamera()->SetLookAt(
		{ -5.661f, 2.5f, 5.584f },				// 視点
		{ 0.0f, 2.0, 0.0f },					// 注視点
		{ 0.036f, 0.999f, -0.035f }				// 上ベクトル
	);
	m_cameraController = std::make_unique<FreeCameraController>();
	m_cameraController->SyncCameraToController(CameraManager::Instance().GetCamera());
	m_cameraController->SetEnable(true);

	// ノードのデフォルト名
	// リサイズ
	nodeDefaultNames.resize(TileType::TILETYPE_COUNT);

	nodeDefaultNames.at(TileType::FLOOR_01A) = "Floor 01a";
	nodeDefaultNames.at(TileType::FLOOR_01B) = "Floor 01b";
	nodeDefaultNames.at(TileType::FLOOR_02A) = "Floor 02a";
	nodeDefaultNames.at(TileType::FLOOR_03A) = "Floor 03a";
	nodeDefaultNames.at(TileType::FLOOR_CLOUD_01A) = "FloorCloud 01a";
	nodeDefaultNames.at(TileType::WALL_01A) = "Wall 01a";
	nodeDefaultNames.at(TileType::WALL_01B) = "Wall 01b";
	nodeDefaultNames.at(TileType::WALL_02A) = "Wall 02a";
	nodeDefaultNames.at(TileType::WALL_02B) = "Wall 02b";
	nodeDefaultNames.at(TileType::WALL_03A) = "Wall 03a";
	nodeDefaultNames.at(TileType::WALL_04A) = "Wall 04a";
	nodeDefaultNames.at(TileType::WALL_CLOUD) = "WallCloud";
	nodeDefaultNames.at(TileType::ARCH_01A) = "Arch 01a";
	nodeDefaultNames.at(TileType::ARCH_ENTRANCE_01A) = "ArchEntrance 01a";
	nodeDefaultNames.at(TileType::ARCH_FLOOR_01A) = "ArchFloor 01a";
	nodeDefaultNames.at(TileType::STAIR_RAILING_01A) = "StairRailing 01a";
	nodeDefaultNames.at(TileType::STAIR_STEP_01A) = "StairStep 01a";
	nodeDefaultNames.at(TileType::CARAMEL_01) = "Caramel 01";
	nodeDefaultNames.at(TileType::CARAMEL_02) = "Caramel 02";
	nodeDefaultNames.at(TileType::CLOUD_01) = "Cloud 01";
	nodeDefaultNames.at(TileType::CLOUD_02) = "Cloud 02";
	nodeDefaultNames.at(TileType::CREAM_01) = "Cream 01";
	nodeDefaultNames.at(TileType::CREAM_02) = "Cream 02";
	nodeDefaultNames.at(TileType::LOLIPOP_01A) = "Lolipop 01a";
	nodeDefaultNames.at(TileType::LOLIPOP_01B) = "Lolipop 01b";
	nodeDefaultNames.at(TileType::LOLIPOP_02A) = "Lolipop 02a";
	nodeDefaultNames.at(TileType::STAR) = "Star";
	nodeDefaultNames.at(TileType::FIRE_HYDRANT) = "FireHydrant";
	nodeDefaultNames.at(TileType::PORTAL) = "Portal";
	nodeDefaultNames.at(TileType::SPAWNER) = "Spawner";
	nodeDefaultNames.at(TileType::CONNECTPOINT) = "ConnectPoint";
	nodeDefaultNames.at(TileType::FOUNTAIN) = "Fountain";
	nodeDefaultNames.at(TileType::STAIR_TO_NEXTFLOOR) = "StairToNextFloor";
	nodeDefaultNames.at(TileType::BOSSROOM) = "BossRoom";
	nodeDefaultNames.at(TileType::BARREL) = "Barrel";
	nodeDefaultNames.at(TileType::WALL_BRICK_01A) = "WallBrick 01a";
	nodeDefaultNames.at(TileType::CHANDELIER) = "Chandelier";
	nodeDefaultNames.at(TileType::FLOOR_BROWN) = "FloorBrown";
	nodeDefaultNames.at(TileType::FLOOR_CHECK) = "FloorCheck";
	nodeDefaultNames.at(TileType::FLOOR_GRAY) = "FloorGray";
	nodeDefaultNames.at(TileType::HEART_ARCH) = "HeartArch";
	nodeDefaultNames.at(TileType::PLASTIC_FLOWER) = "PlasticFlower";
	nodeDefaultNames.at(TileType::SET_01A) = "Set 01a";
	nodeDefaultNames.at(TileType::SET_01B) = "Set 01b";
	nodeDefaultNames.at(TileType::SET_01C) = "Set 01c";
	nodeDefaultNames.at(TileType::STAIR_STEP_01B) = "StairStep 01b";
	nodeDefaultNames.at(TileType::TOY_ARCH_01A) = "ToyArch 01a";
	nodeDefaultNames.at(TileType::TOY_ARCH_SUPPORT_01A) = "ToyArchSupport 01a";
	nodeDefaultNames.at(TileType::TOY_ARCH_TOP_01A) = "ToyArchTop 01a";
	nodeDefaultNames.at(TileType::WALL_PAPER) = "WallPaper";
	nodeDefaultNames.at(TileType::WALL_SQUARES) = "WallSquares";
	nodeDefaultNames.at(TileType::WELL) = "Well";

	//testModel = std::make_unique<ModelObject>("Data/Model/Stage/Terrain_Map.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR);
	//testModel->SetPosition({ 0.0f, 0.0f, 0.0f });
}

void SceneRoomTest_E4C::Finalize()
{
	Console::Instance().Close();

	m_spritePreLoad.clear();
	UI.Clear();
	NODES.Clear();
	m_shadowMapRenderer->Clear();
	CameraManager::Instance().Clear();
	LightManager::Instance().Clear();
}

void SceneRoomTest_E4C::Update(float elapsedTime)
{
	m_cameraController->SyncContrllerToCamera(CameraManager::Instance().GetCamera());
	m_cameraController->Update(elapsedTime);

	Camera* camera = CameraManager::Instance().GetCamera();

	NODES.Update(elapsedTime);

	CameraManager::Instance().GetCamera()->GetFocus();

	if (T_GRAPHICS.isDX12Active)
	{
		for (auto& it : NODES.GetAll())
		{
			T_GRAPHICS.GetShadowRenderer()->ModelRegister(it->GetModel().get());
			//T_GRAPHICS.GetShadowRenderer()->ModelRegister(it->GetModel().get());
		}

		T_GRAPHICS.GetShadowRenderer()->ModelRegister(testModel->GetModel().get());
	}

	// RayCast
	{
		// ImGUIウィンドウ上にマウスがある場合はキャストを行わない
		if (T_INPUT.KeyDown(VK_LBUTTON) &&
			!ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow))
		{
			// マウスカーソルの座標を取得
			DirectX::XMFLOAT3 screenPosition;
			screenPosition.x = T_INPUT.GetMouseWinPos().x;
			screenPosition.y = T_INPUT.GetMouseWinPos().y;

			T_GRAPHICS.GetDeviceContext();

			// レイの始点
			DirectX::XMFLOAT3 start;
			screenPosition.z = 0.0f;
			DirectX::XMStoreFloat3(&start, DirectX::XMVector3Unproject(
				DirectX::XMLoadFloat3(&screenPosition),
				T_GRAPHICS.GetViewPort().TopLeftX,
				T_GRAPHICS.GetViewPort().TopLeftY,
				T_GRAPHICS.GetViewPort().Width,
				T_GRAPHICS.GetViewPort().Height,
				0.0f,
				1.0f,
				DirectX::XMLoadFloat4x4(&camera->GetProjection()),
				DirectX::XMLoadFloat4x4(&camera->GetView()),
				DirectX::XMMatrixIdentity()));

			// レイの終点
			DirectX::XMFLOAT3 end;
			screenPosition.z = 1.0f;
			DirectX::XMStoreFloat3(&end, DirectX::XMVector3Unproject(
				DirectX::XMLoadFloat3(&screenPosition),
				T_GRAPHICS.GetViewPort().TopLeftX,
				T_GRAPHICS.GetViewPort().TopLeftY,
				T_GRAPHICS.GetViewPort().Width,
				T_GRAPHICS.GetViewPort().Height,
				0.0f,
				1.0f,
				DirectX::XMLoadFloat4x4(&camera->GetProjection()),
				DirectX::XMLoadFloat4x4(&camera->GetView()),
				DirectX::XMMatrixIdentity()));

			// レイキャスト
			Node* hitNode = NODES.RayCast(start, end);

			if (hitNode != nullptr)
			{
				ChangeSelectedNode(hitNode);
			}
		}
	}
}

void SceneRoomTest_E4C::Render()
{
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->Clear(T_GRAPHICS.GetDeviceContext(), 0.2f, 0.2f, 0.2f, 1);
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->SetRenderTarget(T_GRAPHICS.GetDeviceContext());

	// 描画コンテキスト設定
	RenderContext rc;
	rc.camera = CameraManager::Instance().GetCamera();
	rc.deviceContext = T_GRAPHICS.GetDeviceContext();
	rc.renderState = T_GRAPHICS.GetRenderState();

	// ライトの情報を詰め込む
	LightManager::Instance().PushRenderContext(rc);

	NODES.Render(rc);

	// AABBの描画
	// radiiは半径なので2倍して直径にしてから描画を行う
	DirectX::XMFLOAT3 diameter = {
	roomSetting.aabb.radii.x * 2.0f,
	roomSetting.aabb.radii.y * 2.0f,
	roomSetting.aabb.radii.z * 2.0f
	};
	T_GRAPHICS.GetDebugRenderer()->DrawCube(roomSetting.aabb.position, diameter, { 1.0f, 1.0f, 1.0f, 1.0f });

	//	シャドウマップ描画
	m_shadowMapRenderer->Render();
	rc.shadowMapData = m_shadowMapRenderer->GetShadowMapData();

	UI.Render(rc);

	// デバッグレンダラ描画実行
	T_GRAPHICS.GetDebugRenderer()->Render(T_GRAPHICS.GetDeviceContext(), CameraManager::Instance().GetCamera()->GetView(), CameraManager::Instance().GetCamera()->GetProjection());
	DrawDebugGUI();
}

void SceneRoomTest_E4C::RenderDX12()
{
	T_GRAPHICS.BeginRender();

	// シーン用定数バッファ更新
	const Descriptor* scene_cbv_descriptor = T_GRAPHICS.UpdateSceneConstantBuffer(
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

		// モデル描画
		NODES.RenderDX12(rc);

		//testModel->RenderDX12(rc);

		T_GRAPHICS.GetDebugRenderer()->DrawCylinder({ 0.0f, 0.0f, 0.0f }, 16.0f, 1.5f, { 1,0,0,1 });
		//T_GRAPHICS.GetDebugRenderer()->DrawCylinder(position, spawnerData.searchRadius, 1.5f, { 1,0,1,1 });
		// AABBの描画
		// radiiは半径なので2倍して直径にしてから描画を行う
		DirectX::XMFLOAT3 diameter = {
		roomSetting.aabb.radii.x * 2.0f,
		roomSetting.aabb.radii.y * 2.0f,
		roomSetting.aabb.radii.z * 2.0f
		};
		T_GRAPHICS.GetDebugRenderer()->DrawCube(roomSetting.aabb.position, diameter, { 1.0f, 1.0f, 1.0f, 1.0f });

		// レンダーターゲットへの書き込み終了待ち
		m_frameBuffer->WaitUntilFinishDrawingToRenderTarget(T_GRAPHICS.GetFrameBufferDX12(FrameBufferDX12Id::Scene));
	}

	// デバッグ描画
	{
	}

	// ポストエフェクト描画
	{
		//postprocessingRenderer->Render(m_frameBuffer);
	}

	DrawDebugGUI();
	T_GRAPHICS.GetImGUIRenderer()->RenderDX12(m_frameBuffer->GetCommandList());

	T_GRAPHICS.End();
}

/**************************************************************************//**
	@brief		部屋データを指定したjsonからロードする
	@param[in]	なし
	@return		なし
*//***************************************************************************/
void SceneRoomTest_E4C::LoadRoomData()
{
	// 現在のノードをクリアする
	NODES.Clear();

	// 現在のカレントディレクトリを保存しておく
	char currentDirectory[260];
	GetCurrentDirectoryA(260, currentDirectory);

	// ダイアログボックスの設定
	OPENFILENAMEA ofn;
	char filePath[260]{};

	// OPENFILENAMEA構造体の初期化
	ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "json File(*.json)\0*.json\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = filePath;
	ofn.nMaxFile = sizeof(filePath);
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrTitle = "Open File";
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;

	// ノードデータ一次保管用
	std::vector<IMPORT_DATA> importDatas;

	// ファイルから読み込み
	if (GetOpenFileNameA(&ofn) == TRUE)
	{
		Console::Instance().Log(filePath);

		nlohmann::json loadFile;
		std::ifstream ifs(filePath);

		if (ifs.is_open())
		{
			ifs >> loadFile;

			// 部屋の生成設定をロード
			roomSetting.weight = loadFile["RoomSetting"]["Weight"];
			roomSetting.aabb.position.x = loadFile["RoomSetting"]["AABB"]["Position"].at(0);
			roomSetting.aabb.position.y = loadFile["RoomSetting"]["AABB"]["Position"].at(1);
			roomSetting.aabb.position.z = loadFile["RoomSetting"]["AABB"]["Position"].at(2);
			roomSetting.aabb.radii.x = loadFile["RoomSetting"]["AABB"]["Radii"].at(0);
			roomSetting.aabb.radii.y = loadFile["RoomSetting"]["AABB"]["Radii"].at(1);
			roomSetting.aabb.radii.z = loadFile["RoomSetting"]["AABB"]["Radii"].at(2);

			// ノードデータロード
			for (const auto& nodeData : loadFile["NodeDatas"])
			{
				TileType tileType = nodeData["Type"];
				DirectX::XMFLOAT3 position = {
					nodeData["Position"].at(0),
					nodeData["Position"].at(1),
					nodeData["Position"].at(2)
				};
				DirectX::XMFLOAT3 angle = {
					nodeData["Angle"].at(0),
					nodeData["Angle"].at(1),
					nodeData["Angle"].at(2)
				};
				DirectX::XMFLOAT3 scale = {
					nodeData["Scale"].at(0),
					nodeData["Scale"].at(1),
					nodeData["Scale"].at(2),
				};

				importDatas.emplace_back(position, angle, scale, tileType);
			}

			ifs.close();
		}
	}

	// 元のカレントディレクトリに戻す
	SetCurrentDirectoryA(currentDirectory);

	for (const IMPORT_DATA& data : importDatas)
	{
		std::string nodeName;

		switch (data.type)
		{
		case TileType::SPAWNER:			AddSpawner("Spawner", data.position);									break;
		case TileType::CONNECTPOINT:	AddConnectPoint("ConnectPoint", data.position, data.angle);				break;
		default:						AddTileNode(nodeDefaultNames.at(data.type), data.type, data.position, data.angle, data.scale);	break;
		}
	}
}

void SceneRoomTest_E4C::LoadSpawnerData(const auto& nodeData)
{
	TileType tileType = nodeData["Type"];
	DirectX::XMFLOAT3 position = {
		nodeData["Position"].at(0),
		nodeData["Position"].at(1),
		nodeData["Position"].at(2)
	};
	DirectX::XMFLOAT3 angle = {
		nodeData["Angle"].at(0),
		nodeData["Angle"].at(1),
		nodeData["Angle"].at(2)
	};
	DirectX::XMFLOAT3 scale = {
		nodeData["Scale"].at(0),
		nodeData["Scale"].at(1),
		nodeData["Scale"].at(2),
	};

	SpawnerNode* newNode = new SpawnerNode(NODES.GetUniqueName("NewNode"));
	newNode->SetPosition(position);
	newNode->SetAngle(angle);
	newNode->SetScale(scale);
	NODES.Register(newNode);
}

/**************************************************************************//**
	@brief		部屋データをjsonにセーブする
	@param[in]	なし
	@return		なし
*//***************************************************************************/
void SceneRoomTest_E4C::SaveRoomData()
{
	// 現在のカレントディレクトリを保存しておく
	char currentDirectory[260];
	GetCurrentDirectoryA(260, currentDirectory);

	// ダイアログボックスの設定
	OPENFILENAMEA ofn;
	char filePath[260]{};

	// OPENFILENAMEA構造体の初期化
	ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "json file(*.json)\0*.json\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = filePath;
	ofn.nMaxFile = sizeof(filePath);
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrTitle = "Save File";
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_CREATEPROMPT;
	ofn.lpstrDefExt = "json";

	// ファイルに保存
	if (GetSaveFileNameA(&ofn) == TRUE)
	{
		Console::Instance().Log(filePath);
		nlohmann::json saveFile;

		// 部屋の生成設定
		saveFile["RoomSetting"] = {
			{ "Weight", roomSetting.weight },
		};

		saveFile["RoomSetting"]["AABB"] = {
			{ "Position", { roomSetting.aabb.position.x, roomSetting.aabb.position.y, roomSetting.aabb.position.z }},
			{ "Radii", { roomSetting.aabb.radii.x, roomSetting.aabb.radii.y, roomSetting.aabb.radii.z }}
		};

		// ノードデータ設定
		saveFile["NodeDatas"] = nlohmann::json::array();
		for (Node* tileNode : NODES.GetAll())
		{
			saveFile["NodeDatas"].push_back({
				{ "Type", tileNode->GetType() },
				{ "Position", { tileNode->GetPosition().x, tileNode->GetPosition().y, tileNode->GetPosition().z }},
				{ "Angle", { tileNode->GetAngle().x, tileNode->GetAngle().y, tileNode->GetAngle().z }},
				{ "Scale", { tileNode->GetScale().x, tileNode->GetScale().y, tileNode->GetScale().z }},
				});
		}

		std::ofstream ofs(filePath);
		if (ofs.is_open())
		{
			ofs << saveFile.dump(4);
			ofs.close();
		}
	}

	// 元のカレントディレクトリに戻す
	SetCurrentDirectoryA(currentDirectory);
}

void SceneRoomTest_E4C::DrawDebugGUI()
{
	ImVec2 screenSize = { TentacleLib::graphics.GetScreenWidth(), TentacleLib::graphics.GetScreenHeight() };
	ImVec2 windowSize = { 256.0f, 512.0f };

	//ImGui::SetNextWindowPos(ImVec2(32.0f, 32.0f), ImGuiCond_Once);
	//ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

	//ImGui::SetNextWindowPos(ImVec2((256.0f + 32.0f), 32.0f), ImGuiCond_Once);
	//ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

	// ひえらるき～
	if (ImGui::Begin("SceneRoomTest_E4C", nullptr, ImGuiWindowFlags_MenuBar))
	{
		// メニュー
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("ファイル")) {
				if (ImGui::MenuItem("json読み込み")) LoadRoomData();
				if (ImGui::MenuItem("json保存")) SaveRoomData();
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("ノード作成")) {
				if (ImGui::BeginMenu("タイルノード")) {
					for (int i = 0; i < (int)(TileType::TILETYPE_COUNT); i++)
					{
						std::string defaultName = GetDefaultName((TileType)i);

						if (ImGui::MenuItem(defaultName.c_str())) AddTileNode(defaultName, (TileType)i);
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("オブジェクトノード")) {
					if (ImGui::MenuItem("Spawner"))	AddSpawner();
					if (ImGui::MenuItem("ConnectPoint")) AddConnectPoint();
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
		}
		if (ImGui::MenuItem("ノード複製")) DuplicateNode();
		if (ImGui::BeginMenu("ノード削除")) {
			if (ImGui::MenuItem("選択中のノードを削除")) RemoveSelectedNode();
			if (ImGui::MenuItem("全てのノードを削除")) ClearNodes();
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	// 部屋の生成設定
	if (ImGui::TreeNodeEx("部屋の生成設定", ImGuiTreeNodeFlags_DefaultOpen)) {
		// Weight
		ImGui::InputInt("重み", &roomSetting.weight);
		// AABB
		ImGui::DragFloat3("AABB: Position", &roomSetting.aabb.position.x, 1.0f);
		ImGui::DragFloat3("AABB: Radii", &roomSetting.aabb.radii.x, 1.0f);

		ImGui::TreePop();
	}

	ImGui::Separator();

	// ノード
	if (ImGui::TreeNodeEx("ノード", ImGuiTreeNodeFlags_DefaultOpen)) {
		int index = 0;
		for (Node* node : NODES.GetAll()) {
			ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;
			if (selectionNode == node) nodeFlags |= ImGuiTreeNodeFlags_Selected;
			if (node) {
				ImGui::PushID(index);

				if (ImGui::TreeNodeEx(node->GetName().c_str(), nodeFlags)) {
					if (ImGui::IsItemClicked()) ChangeSelectedNode(node);
					if (ImGui::BeginPopupContextItem())
					{
						ImGui::SeparatorText("ノードメニュー");
						if (ImGui::Button("複製"))
						{
							DuplicateNode();
							ImGui::CloseCurrentPopup();
						}
						ImGui::SameLine();
						if (ImGui::Button("削除"))
						{
							ChangeSelectedNode(nullptr);
							NODES.Remove(node);
							ImGui::CloseCurrentPopup();
						}
						ImGui::EndPopup();
					}
				}
				ImGui::PopID();
			}
			index++;
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	ImGui::Separator();
	ImGui::End();

	// 選択したノードのデバッグGUIを表示する
	if (selectionNode != nullptr)
	{
		windowSize = { 256.0f, 256.0f };

		ImGui::SetNextWindowPos(ImVec2((screenSize.x - windowSize.x - 32.0f), 32.0f), ImGuiCond_Once);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

		if (ImGui::Begin("選択中のノード", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
		{
			selectionNode->DrawDebugGUI();
		}
		ImGui::End();
	}

	{
		ImGui::SetNextWindowPos(ImVec2(T_INPUT.GetMousePos().x, T_INPUT.GetMousePos().y), ImGuiCond_Once);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

		if (ImGui::BeginPopupContextVoid())
		{
			ImGui::SeparatorText("ノードメニュー");
			if (ImGui::Button("複製"))
			{
				DuplicateNode();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("削除"))
			{
				Node* removeNode = selectionNode;
				ChangeSelectedNode(nullptr);
				NODES.Remove(removeNode);
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
}

void SceneRoomTest_E4C::AddTileNode(
	std::string name, TileType type,
	DirectX::XMFLOAT3 position,
	DirectX::XMFLOAT3 angle,
	DirectX::XMFLOAT3 scale)
{
	FILE_DATA importData = DungeonData::Instance().GetModelFileDatas(type).at(0);

	switch (type) {
	case ns_RoomData::PORTAL:
	case ns_RoomData::SPAWNER:
	case ns_RoomData::CONNECTPOINT: return;	// ポータル、スポナー、接続点は別の関数で生成する
	default: break;
	}

	TileNode* newNode = new TileNode(NODES.GetUniqueName(name), type, importData.fileName.c_str(), importData.scale);
	newNode->SetPosition(position);
	newNode->SetAngle(angle);
	newNode->SetScale(scale);
	NODES.Register(newNode);

	// 追加したノードを選択
	ChangeSelectedNode(newNode);
}

void SceneRoomTest_E4C::AddSpawner(
	std::string name,
	DirectX::XMFLOAT3 position)
{
	SpawnerNode* newNode = new SpawnerNode(NODES.GetUniqueName(name));
	newNode->SetPosition(position);
	NODES.Register(newNode);

	// 追加したノードを選択させる
	ChangeSelectedNode(newNode);
}

void SceneRoomTest_E4C::AddConnectPoint(
	std::string name,
	DirectX::XMFLOAT3 position,
	DirectX::XMFLOAT3 angle)
{
	ConnectPointNode* newNode = new ConnectPointNode(NODES.GetUniqueName(name));
	newNode->SetPosition(position);
	newNode->SetAngle(angle);
	NODES.Register(newNode);

	// 追加したノードを選択させる
	ChangeSelectedNode(newNode);
}

void SceneRoomTest_E4C::DuplicateNode()
{
	if (selectionNode != nullptr)
	{
		Node* newNode = selectionNode->Duplicate();

		// 正規表現が用いられた名前に変換する
		newNode->SetName(NODES.GetUniqueName(newNode->GetName()));
		NODES.Register(newNode);

		// 複製したノードを選択する
		ChangeSelectedNode(newNode);
	}
}

void SceneRoomTest_E4C::RemoveSelectedNode()
{
	NODES.Remove(selectionNode);
	NODES.Update(0);
	selectionNode = nullptr;
}

void SceneRoomTest_E4C::ClearNodes()
{
	NODES.Clear();
	selectionNode = nullptr;
}

void SceneRoomTest_E4C::ChangeSelectedNode(Node* newNode)
{
	if (selectionNode != nullptr)
	{
		selectionNode->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	}
	selectionNode = newNode;
	if (newNode != nullptr)
	{
		newNode->SetColor({ 1.2f, 1.2f, 1.2f, 1.0f });
	}
}

void SceneRoomTest_E4C::CalcAABB()
{
	float minX, minY, minZ;
	float maxX, maxY, maxZ;
	minX = minY = minZ = FLT_MAX;
	maxX = maxY = maxZ = FLT_MIN;

	if (NODES.Count() == 0)
	{
		// ノードがないなら全て0
		roomSetting.aabb.position = { 0.0f, 0.0f, 0.0f };
		roomSetting.aabb.radii = { 0.0f, 0.0f, 0.0f };
		return;
	}

	for (Node* node : NODES.GetAll())
	{
		if (node->GetPosition().x < minX) minX = node->GetPosition().x;
		if (node->GetPosition().y < minY) minY = node->GetPosition().y;
		if (node->GetPosition().z < minZ) minZ = node->GetPosition().z;
		if (node->GetPosition().x > maxX) maxX = node->GetPosition().x;
		if (node->GetPosition().y > maxY) maxY = node->GetPosition().y;
		if (node->GetPosition().z > maxZ) maxZ = node->GetPosition().z;
	}

	float widthX = fabsf(maxX - minX) + 4.0f;
	float widthY = fabsf(maxY - minY) + 4.0f;
	float widthZ = fabsf(maxZ - minZ) + 4.0f;

	roomSetting.aabb.position = { (widthX * 0.5f), (widthY * 0.5f), (widthZ * 0.5f) };
	roomSetting.aabb.radii = { widthX, widthY, widthZ };
}

void SceneRoomTest_E4C::AddTemplate3x3Floor()
{
	for (int x = -1; x < 2; x++) {
		for (int z = -1; z < 2; z++) {
			AddTileNode(NODES.GetUniqueName("Floor01a"), TileType::FLOOR_01A, DirectX::XMFLOAT3((x * 4.0f), 0.0f, (z * 4.0f)));
		}
	}
}

Node* TileNode::Duplicate()
{
	FILE_DATA importData = DungeonData::Instance().GetModelFileDatas(type).at(0);

	TileNode* newNode = new TileNode(name, type, importData.fileName.c_str(), importData.scale);
	newNode->SetPosition(this->position);
	newNode->SetAngle(this->angle);
	newNode->SetScale(this->scale);

	return newNode;
}

void TileNode::DrawDebugGUI()
{
	// name
	{
		std::string inputName = name;
		if (ImGui::InputText("Name", inputName.data(), 256))
		{
			std::string newName = inputName.c_str();

			// 正規表現も行ってからセットする
			name = NODES.GetUniqueName(newName);
		}
	}

	// pos
	if (ImGui::Button("Reset##1")) position = { 0.0f, 0.0f, 0.0f };
	ImGui::SameLine();
	ImGui::DragFloat3("Position", &position.x, 0.1f);
	// angle
	DirectX::XMFLOAT3 debugAngle = angle;
	debugAngle.x = DirectX::XMConvertToDegrees(debugAngle.x);
	debugAngle.y = DirectX::XMConvertToDegrees(debugAngle.y);
	debugAngle.z = DirectX::XMConvertToDegrees(debugAngle.z);
	if (ImGui::Button("Reset##2")) debugAngle = { 0.0f, 0.0f, 0.0f };
	ImGui::SameLine();
	if (ImGui::DragFloat3("Angle", &debugAngle.x, 1.0f))
	{
		angle.x = DirectX::XMConvertToRadians(debugAngle.x);
		angle.y = DirectX::XMConvertToRadians(debugAngle.y);
		angle.z = DirectX::XMConvertToRadians(debugAngle.z);
	}
	// scale
	if (ImGui::Button("Reset##3")) scale = { 1.0f, 1.0f, 1.0f };
	ImGui::SameLine();
	ImGui::DragFloat3("Scale", &scale.x, 0.1f);
}

void SpawnerNode::Render(const RenderContext& rc)
{
	// 継承元のRender呼び出し
	ModelObject::Render(rc);

	T_GRAPHICS.GetDebugRenderer()->DrawCylinder(position, spawnerData.spawnRadius, 1.5f, { 1,0,0,1 });
	T_GRAPHICS.GetDebugRenderer()->DrawCylinder(position, spawnerData.searchRadius, 1.5f, { 1,0,1,1 });
}

Node* SpawnerNode::Duplicate()
{
	SpawnerNode* newNode = new SpawnerNode(name);
	newNode->SetPosition(this->position);
	newNode->SetAngle(this->angle);
	newNode->SetScale(this->scale);

	return newNode;
}

void SpawnerNode::DrawDebugGUI()
{
	ImGui::Text("Type: SPAWNER");

	// name
	std::string inputName = name;
	if (ImGui::InputText("Name", inputName.data(), 256)) {
		// 正規表現も行ってからセットする
		name = NODES.GetUniqueName(inputName.c_str());
	}

	// pos
	ImGui::DragFloat3("Position", &position.x, 0.1f);
	// angle
	DirectX::XMFLOAT3 debugAngle = angle;
	debugAngle.x = DirectX::XMConvertToDegrees(debugAngle.x);
	debugAngle.y = DirectX::XMConvertToDegrees(debugAngle.y);
	debugAngle.z = DirectX::XMConvertToDegrees(debugAngle.z);
	if (ImGui::DragFloat3("Angle", &debugAngle.x, 1.0f)) {
		angle.x = DirectX::XMConvertToRadians(debugAngle.x);
		angle.y = DirectX::XMConvertToRadians(debugAngle.y);
		angle.z = DirectX::XMConvertToRadians(debugAngle.z);
	}
	// scale
	ImGui::DragFloat3("Scale", &scale.x, 0.1f);

	// スポナーデータ
	if (ImGui::TreeNodeEx("SpawnerData", ImGuiTreeNodeFlags_DefaultOpen)) {
		// enemyType
		int enemyTypeInt = (int)spawnerData.enemyType;
		if (ImGui::InputInt("EnemyType", &enemyTypeInt)) {
			// 最大数を超えないように調整
			if (enemyTypeInt < 0) enemyTypeInt = 0;
			spawnerData.enemyType = (uint8_t)enemyTypeInt;
		}
		// searchRadius
		ImGui::SliderFloat("SearchRadius", &spawnerData.searchRadius, 0.0f, 30.0f);
		// spawnRadius
		ImGui::SliderFloat("SpawnRadius", &spawnerData.spawnRadius, 0.0f, 30.0f);
		// maxExistedEnemiesNum
		ImGui::SliderInt("MaxExistedEnemiesNum", &spawnerData.maxExistedEnemiesNum, 0, 30);
		// maxSpawnedEnemiesNum
		ImGui::SliderInt("MaxSpawnedEnemiesNum", &spawnerData.maxSpawnedEnemiesNum, -1, 30);
		// spawnTime
		ImGui::SliderFloat("SpawnTime", &spawnerData.spawnTime, 1.0f, 30.0f);

		ImGui::TreePop();
	}
}

std::string NodeManager::GetUniqueName(std::string name)
{
	// 使用されている番号を保存する配列
	std::vector<int> duplicateNumbers;

	int nextNumber = -1;

	// 正規表現が用いられていない名前にする
	{
		std::smatch match;
		if (std::regex_search(name, match, std::regex("\\((\\d+)\\)")))
		{
			name = match.prefix();
		}
	}

	// 既存のノード番号をセットに追加する
	for (Node* node : this->items)
	{
		// 同じ名前のノードがあれば0番も使用されているとして保存
		if (name == node->GetName())
		{
			duplicateNumbers.push_back(0);
		}

		std::string nodeName = node->GetName();
		std::smatch match;
		if (std::regex_search(nodeName, match, std::regex("\\((\\d+)\\)")))
		{
			// 正規表現が行われていない名前と同じ名前なら
			if (name == match.prefix())
			{
				// 使用されているとして番号を保存
				duplicateNumbers.emplace_back(std::stoi(match[1].str()));
			}
		}
	}

	// 配列を昇順に並べ替える
	std::sort(duplicateNumbers.begin(), duplicateNumbers.end());
	// 重複した番号を削除する
	duplicateNumbers.erase(std::unique(duplicateNumbers.begin(), duplicateNumbers.end()), duplicateNumbers.end());

	// 使用されている番号があるならば
	if (duplicateNumbers.size() != 0)
	{
		// 空いている番号を探す
		for (int searchNum = 0; int number : duplicateNumbers)
		{
			// searchNumとnumberが同じでないなら
			// 空いている番号
			if (searchNum != number)
			{
				nextNumber = searchNum;
				break;
			}
			searchNum++;
		}

		// 空いている番号がなかった場合
		if (nextNumber == -1)
		{
			// 最後のnumber+1したものが次の番号
			nextNumber = (duplicateNumbers.back() + 1);
		}
	}
	// 中身がないなら重複なしなのでそのまま返す
	else
	{
		return name;
	}

	// バグ修正用
	if (name.back() == '\0') {
		name.pop_back();
	}

	// 0番が空いている＝オリジナルが空いているのでそのまま返す
	if (nextNumber == 0) return name;

	// 数字をカッコで囲んで返す
	return name + "(" + std::to_string(nextNumber) + ")";
}

Node* NodeManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3 end)
{
	for (Node* node : items)
	{
		if (node->GetModels().size() == 0) continue;

		HitResultVector hit;
		if (Collision::IntersectRayVsModel(start, end, node->GetModel().get(), hit))
		{
			return node;
		}
	}
	return nullptr;
}

void ConnectPointNode::Render(const RenderContext& rc)
{
	// 継承元のRender呼び出し
	ModelObject::Render(rc);

	T_GRAPHICS.GetDebugRenderer()->SetSphere(position, 1.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
}

Node* ConnectPointNode::Duplicate()
{
	ConnectPointNode* newNode = new ConnectPointNode(name);
	newNode->SetPosition(this->position);
	newNode->SetAngle(this->angle);
	newNode->SetScale(this->scale);

	return newNode;
}

void ConnectPointNode::DrawDebugGUI()
{
	ImGui::Text("Type: CONNECTPOINT");

	// name
	{
		std::string inputName = name;
		if (ImGui::InputText("Name", inputName.data(), 256))
		{
			std::string newName = inputName.c_str();

			// 正規表現も行ってからセットする
			name = NODES.GetUniqueName(newName);
		}
	}

	// pos
	ImGui::DragFloat3("Position", &position.x, 0.1f);
	// angle
	DirectX::XMFLOAT3 debugAngle = angle;
	debugAngle.x = DirectX::XMConvertToDegrees(debugAngle.x);
	debugAngle.y = DirectX::XMConvertToDegrees(debugAngle.y);
	debugAngle.z = DirectX::XMConvertToDegrees(debugAngle.z);
	if (ImGui::DragFloat3("Angle", &debugAngle.x, 1.0f))
	{
		angle.x = DirectX::XMConvertToRadians(debugAngle.x);
		angle.y = DirectX::XMConvertToRadians(debugAngle.y);
		angle.z = DirectX::XMConvertToRadians(debugAngle.z);
	}
	// scale
	ImGui::DragFloat3("Scale", &scale.x, 0.1f);
}
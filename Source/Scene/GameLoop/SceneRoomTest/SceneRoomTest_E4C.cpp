#include "TAKOEngine/Runtime/tentacle_lib.h"
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
#include <commdlg.h>

#include "Scene/SceneManager.h"
#include "SceneRoomTest_E4C.h"

#include "Map/MapTile.h"
#include "Map/MapTileManager.h"
#include "Map/CrossRoom1.h"

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
	LightManager::Instance().SetAmbientColor({ 0, 0, 0, 0 });
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

	Console::Instance().Open();
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
	m_cameraController->Update();
	m_cameraController->SyncContrllerToCamera(CameraManager::Instance().GetCamera());

	NODES.Update(elapsedTime);
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
		CameraManager::Instance().GetCamera());

	// レンダーコンテキスト設定
	RenderContextDX12 rc;
	rc.d3d_command_list = m_frameBuffer->GetCommandList();
	rc.scene_cbv_descriptor = scene_cbv_descriptor;

	// 3Dモデル描画
	{
		m_frameBuffer->WaitUntilToPossibleSetRenderTarget(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Scene));
		m_frameBuffer->SetRenderTarget(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Scene));
		m_frameBuffer->Clear(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Scene));

		// モデル描画
		MAPTILES.RenderDX12(rc);

		// レンダーターゲットへの書き込み終了待ち
		m_frameBuffer->WaitUntilFinishDrawingToRenderTarget(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Scene));
	}

	// ポストエフェクト描画
	{
		//postprocessingRenderer->Render(m_frameBuffer);
	}

	// 2D描画
	{
	}

	T_GRAPHICS.End();
}

/**************************************************************************//**
	@brief		部屋データを指定したjsonからロードする
	@param[in]	なし
	@return		なし
*//***************************************************************************/
void SceneRoomTest_E4C::LoadRoomData()
{
	// ダイアログボックスの設定
	OPENFILENAMEA ofn;
	char filePath[260]{};

	// OPENFILENAMEA構造体の初期化
	ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "JSONファイル(*.json)\0*.json\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = filePath;
	ofn.nMaxFile = sizeof(filePath);
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrTitle = "ファイルを開く";
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;

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
			roomSetting.aabb.position.x = loadFile["RoomSetting"]["AABB"].at(0);
			roomSetting.aabb.position.y = loadFile["RoomSetting"]["AABB"].at(1);
			roomSetting.aabb.position.z = loadFile["RoomSetting"]["AABB"].at(2);

			// ノードデータロード
			for (const auto& nodeData : loadFile["NodeDatas"])
			{
				// タイプによって処理を分ける
				TileType tileType = nodeData["Type"];

				switch (tileType)
				{
				case PORTAL:			continue;
				case SPAWNER:			LoadSpawnerData(nodeData);	break;
				case CONNECTPOINT:		continue;
				case TILETYPE_COUNT:	continue;
				default:				LoadTileNodeData(nodeData);	break;
				}
			}

			ifs.close();
		}
	}
}

void SceneRoomTest_E4C::LoadTileNodeData(const auto& nodeData)
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

	FILE_DATA importData = DungeonData::Instance().GetModelFileDatas(tileType).at(0);

	TileNode* newNode = new TileNode(NODES.GetUniqueName("NewNode"), tileType, importData.fileName.c_str(), importData.scale);
	newNode->SetPosition(position);
	newNode->SetAngle(angle);
	newNode->SetScale(scale);
	NODES.Register(newNode);
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
	// ダイアログボックスの設定
	OPENFILENAMEA ofn;
	char filePath[260]{};

	// OPENFILENAMEA構造体の初期化
	ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "JSONファイル(*.json)\0*.json\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = filePath;
	ofn.nMaxFile = sizeof(filePath);
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrTitle = "ファイルを保存する";
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
			{ "AABB", { roomSetting.aabb.position.x, roomSetting.aabb.position.y, roomSetting.aabb.position.z }},
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
}

void SceneRoomTest_E4C::DrawDebugGUI()
{
	ImVec2 screenSize = { TentacleLib::graphics.GetScreenWidth(), TentacleLib::graphics.GetScreenHeight() };
	ImVec2 windowSize = { 256.0f, 512.0f };

	ImGui::SetNextWindowPos(ImVec2(32.0f, 32.0f), ImGuiCond_Once);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

	bool check = false;
	if (ImGui::Begin("SceneRoomTest_E4C", &check, ImGuiWindowFlags_MenuBar))
	{
		//if (ImGui::BeginMainMenuBar())
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Load File")) LoadRoomData();
				if (ImGui::MenuItem("Save File")) SaveRoomData();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2((256.0f + 32.0f), 32.0f), ImGuiCond_Once);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

	// ひえらるき～
	if (ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_MenuBar))
	{
		// メニュー
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("New")) {
				if (ImGui::BeginMenu("TileNode")) {
					if (ImGui::MenuItem("Floor01a"))		AddTileNode("Floor",	TileType::FLOOR_01A);
					if (ImGui::MenuItem("Wall01a"))			AddTileNode("Wall",		TileType::WALL_01A);
					if (ImGui::MenuItem("StairStep01a"))	AddTileNode("Stair",	TileType::STAIR_STEP_01A);
					if (ImGui::MenuItem("Portal"))			AddTileNode("Portal",	TileType::PORTAL);
					if (ImGui::MenuItem("ConnectPoint"))	AddTileNode("ConnectPoint",	TileType::CONNECTPOINT);
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("ObjectNode")) {
					if (ImGui::MenuItem("Spawner"))	AddSpawner();
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Template")) {
					if (ImGui::MenuItem("3x3 Floor")) AddTemplate3x3Floor();
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Duplicate")) DuplicateNode();
			if (ImGui::BeginMenu("Remove")) {
				if (ImGui::MenuItem("Remove Selected Node")) RemoveSelectedNode();
				if (ImGui::MenuItem("Clear All")) ClearNodes();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// 部屋の生成設定
		ImGui::InputInt("Weight", &roomSetting.weight);

		ImGui::Separator();

		// ノード
		if (ImGui::TreeNodeEx("Nodes", ImGuiTreeNodeFlags_DefaultOpen)) {
			int index = 0;
			for (Node* node : NODES.GetAll()) {
				ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;
				if (selectionNode == node) nodeFlags |= ImGuiTreeNodeFlags_Selected;
				if (node) {
					ImGui::PushID(index);
					if (ImGui::TreeNodeEx(node->GetName().c_str(), nodeFlags)) {
						if (ImGui::IsItemFocused()) selectionNode = node;
					}
					ImGui::PopID();
				}
				index++;
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
	}
	ImGui::End();

	// 選択したノードのデバッグGUIを表示する
	if (selectionNode != nullptr)
	{
		windowSize = { 256.0f, 256.0f };

		ImGui::SetNextWindowPos(ImVec2((screenSize.x - windowSize.x - 32.0f), 32.0f), ImGuiCond_Once);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

		if (ImGui::Begin("SelectedNode", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
		{
			selectionNode->DrawDebugGUI();
		}
		ImGui::End();
	}
}

void SceneRoomTest_E4C::AddTileNode(
	std::string name, TileType type,
	DirectX::XMFLOAT3 position,
	DirectX::XMFLOAT3 angle,
	DirectX::XMFLOAT3 scale)
{
	FILE_DATA importData = DungeonData::Instance().GetModelFileDatas(type).at(0);

	TileNode* newNode = new TileNode(NODES.GetUniqueName("NewNode"), type, importData.fileName.c_str(), importData.scale);
	newNode->SetPosition(position);
	newNode->SetAngle(angle);
	newNode->SetScale(scale);
	NODES.Register(newNode);

	// 追加したノードを選択
	selectionNode = newNode;
}

void SceneRoomTest_E4C::AddSpawner()
{
	SpawnerNode* newNode = new SpawnerNode(NODES.GetUniqueName("Spawner"));
	NODES.Register(newNode);

	// 追加したノードを選択させる
	selectionNode = newNode;
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
		selectionNode = newNode;
	}
}

void SceneRoomTest_E4C::RemoveSelectedNode()
{
	NODES.Remove(selectionNode);
}

void SceneRoomTest_E4C::ClearNodes()
{
	NODES.Clear();
	selectionNode = nullptr;
}

void SceneRoomTest_E4C::AddTemplate3x3Floor()
{
	for (int x = -1; x < 2; x++) {
		for (int z = -1; z < 2; z++) {
			TileNode* newNode = new TileNode(NODES.GetUniqueName("Floor01a"), TileType::FLOOR_01A, "Data/Model/DungeonAssets/SM_Floor_02a.glb");
			newNode->SetPosition(DirectX::XMFLOAT3((x), 0.0f, (z)));
			NODES.Register(newNode);
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
	// type
	{
		std::string typeText = "Type: ";

		switch (type)
		{
		case TileType::FLOOR_01A:		typeText += "FLOOR_01A";		break;
		case TileType::WALL_01A:		typeText += "WALL_01A";			break;
		case TileType::STAIR_STEP_01A:	typeText += "STAIR_STEP_01A";	break;
		case TileType::PORTAL:			typeText += "PORTAL";			break;
		case TileType::CONNECTPOINT:	typeText += "CONNECTPOINT";		break;
		}

		ImGui::Text(typeText.c_str());
	}

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
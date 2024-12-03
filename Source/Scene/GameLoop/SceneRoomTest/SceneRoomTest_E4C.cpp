#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Network/HttpRequest.h"
#include "TAKOEngine/Rendering/ResourceManager.h"
#include "TAKOEngine/GUI/UIManager.h"

#include "TAKOEngine/Sound/Sound.h"
#include "TAKOEngine/Editor/Camera/CameraManager.h"

#include <imgui.h>
#include <string>
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

	TileNode* testNode1 = new TileNode("");
	testNode1->SetName(GetUniqueName("node"));
	TILENODES.Register(testNode1);

	TileNode* testNode2 = new TileNode("");
	testNode2->SetName(GetUniqueName("node"));
	TILENODES.Register(testNode2);

	//std::string testStr1 = "node";
	//std::string testStr2 = "node(1)";
	//std::string testStr3 = "node(2)";
	//std::string testStr4 = "floor(1)";
	//std::string testStr5 = "floor";

	//testStr1 = GetUniqueName(testStr1);
	//testStr2 = GetUniqueName(testStr2);

	//int a = 0;
}

void SceneRoomTest_E4C::Finalize()
{
	m_spritePreLoad.clear();
	UI.Clear();
	TILENODES.Clear();
	m_shadowMapRenderer->Clear();
	CameraManager::Instance().Clear();
	LightManager::Instance().Clear();
}

void SceneRoomTest_E4C::Update(float elapsedTime)
{
	m_cameraController->Update();
	m_cameraController->SyncContrllerToCamera(CameraManager::Instance().GetCamera());

	TILENODES.Update(elapsedTime);
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

	TILENODES.Render(rc);

	//	シャドウマップ描画
	m_shadowMapRenderer->Render();
	rc.shadowMapData = m_shadowMapRenderer->GetShadowMapData();

	UI.Render(rc);

	// デバッグレンダラ描画実行
	T_GRAPHICS.GetDebugRenderer()->Render(T_GRAPHICS.GetDeviceContext(), CameraManager::Instance().GetCamera()->GetView(), CameraManager::Instance().GetCamera()->GetProjection());
	DebugRender();

	//T_TEXT.End();
}

/**************************************************************************//**
	@brief		部屋データをjsonからロードする
	@param[in]	なし
	@return		なし
*//***************************************************************************/
void SceneRoomTest_E4C::LoadRoomData()
{
	// 構造体セット
	//OPENFILENAMEA	ofn;
	//memset(&ofn, 0, sizeof(OPENFILENAMEA));
	//ofn.lStructSize = sizeof(ofn);
	//ofn.hwndOwner = NULL;
	//ofn.lpstrTitle = "a";
	////ofn.lStructSize = sizeof(OPENFILENAMEA);
	////ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	
	//::GetOpenFileNameA(&ofn);
}

/**************************************************************************//**
	@brief		部屋データをjsonにセーブする
	@param[in]	なし
	@return		なし
*//***************************************************************************/
void SceneRoomTest_E4C::SaveRoomData()
{
	nlohmann::json saveFile;

	nlohmann::json testJson;
	testJson["TestData"] = {
		{
			{"Yeah", 315},
			{"Salmon", true}
		}
	};

	saveFile.emplace_back(testJson);

	////saveFile["RoomData"]["RoomType"] = RoomType::SIMPLE_ROOM_1;
	////saveFile["RoomData"]["Weight"] = 0;

	//// 接続点データ
	//CONNECTPOINT_DATA newData[2];
	//newData[0].position = { 0, 0, 0 };
	//newData[1].position = { 10, 10, 10 };

	//saveFile["RoomData"] = {
	//	{"RoomType", RoomType::SIMPLE_ROOM_1},
	//	{"Weight", 1},
	//};

	//for (const CONNECTPOINT_DATA& data : newData)
	//{
	//	//saveFile["RoomData"]["ConnectPointDatas"].emplace_back(data);
	//}

	std::ofstream file("debug_TestRoom.json");
	file << saveFile;
	file.close();
}

/**************************************************************************//**
	@brief		正規表現を用いた重複しない名前を返す
	@param[in]	元の名前
	@return		重複しない名前
*//***************************************************************************/
std::string SceneRoomTest_E4C::GetUniqueName(std::string name)
{
	// ノード番号を格納するためのベクター
	std::vector<int> nodeNumbers;

	int nextNumber = -1;

	// 既存のノード番号をセットに追加する
	for (TileNode* node : TILENODES.GetAll())
	{
		// 同じ名前のノードがあれば0番をノード番号ベクターに保存
		if (name == node->GetName())
		{
			nodeNumbers.emplace_back(0);
		}

		std::string nodeName = node->GetName();
		std::smatch match;
		if (std::regex_search(nodeName, match, std::regex("\\((\\d+)\\)")))
		{
			// match[1]が空でない
			// ちゃんと正規表現になっている
			if (!match[1].str().empty())
			{
				// ノード番号ベクターに保存
				nodeNumbers.emplace_back(std::stoi(match[1].str()));
			}
		}
	}

	// ノード番号ベクターに中身があるなら
	if (nodeNumbers.size() != 0)
	{
		// 空いている番号を探す
		for (int searchNum = 1; int number : nodeNumbers)
		{
			// nodeNumbersと一致しないなら
			// numberが空いている番号となる
			if (searchNum != number)
			{
				nextNumber = number;
				break;
			}
			searchNum++;
		}

		// 空いている番号がなかった場合
		if (nextNumber == -1)
		{
			// 最後のnumber+1したものが次の番号
			nextNumber = (nodeNumbers.back() + 1);
		}
	}
	// 中身がないなら重複なしなのでそのまま返す
	else
	{
		return name;
	}

	return (name + "(" + std::to_string(nextNumber) + ")");
}

/**************************************************************************//**
	@brief		正規表現が用いられた名前から元の名前に変換し返す
	@param[in]	正規表現が用いられた名前
	@return		元の名前
*//***************************************************************************/
std::string SceneRoomTest_E4C::GetNameByRegEx(std::string regName)
{
	std::string replaceStr = "";
	return std::regex_replace(regName, std::regex(R"(\w+)\((\d+)\)"), replaceStr.c_str());
}

void SceneRoomTest_E4C::DebugRender()
{
	//if (ImGui::TreeNode("SceneRoomTest_E4C"))
	//{
	//	if (ImGui::Button("Save"))
	//	{
	//		nlohmann::json jsonFile;

	//		jsonFile["yeah"] = 1;

	//		std::ofstream file("testRoom.json");
	//		file << jsonFile;
	//		file.close();
	//	}

	//	ImGui::TreePop();
	//}

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
				if (ImGui::MenuItem("Open File"))
				{
					LoadRoomData();
				}
				if (ImGui::MenuItem("Save File"))
				{
					SaveRoomData();
				}

				ImGui::EndMenu();
			}

			//ImGui::EndMainMenuBar();
			ImGui::EndMenuBar();
		}
	}
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2((256.0f + 32.0f), 32.0f), ImGuiCond_Once);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

	// ひえらるき～
	if (ImGui::Begin("Hierarchy", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
	{
		std::vector<std::string> typeNames;
		typeNames.emplace_back("FLOOR");
		typeNames.emplace_back("WALL");
		typeNames.emplace_back("PILLAR");
		typeNames.emplace_back("STAIR");
		typeNames.emplace_back("SPAWNER");

		if (ImGui::BeginCombo("NextTileType", typeNames.at(selectionTileType).c_str()))
		{
			for (int i = 0; i < typeNames.size(); i++)
			{
				if (ImGui::Selectable(typeNames.at(i).c_str()))
				{
					selectionTileType = static_cast<TileType>(i);
				}
			}

			ImGui::EndCombo();
		}

		if (ImGui::Button("Add"))
		{
			std::string fileName;

			switch (selectionTileType)
			{
			case TileType::FLOOR:	fileName = "Data/Model/DungeonAssets/NewAssets/SM_Floor_01a.glb";			break;
			case TileType::WALL:	fileName = "Data/Model/DungeonAssets/NewAssets/SM_Wall_Pattern_01a.glb";	break;
			case TileType::PILLAR:	break;
			case TileType::STAIR:	fileName = "Data/Model/DungeonAssets/SLOPE.glb";	break;
			case TileType::SPAWNER:	break;
			}

			TileNode* newNode = new TileNode(fileName.c_str(), 1.0f);
			newNode->SetName("TileNode");
			newNode->SetType(selectionTileType);

			TILENODES.Register(newNode);

			// 追加したノードを選択させる
			selectionNode = newNode;
		}

		ImGui::SameLine();
		if (ImGui::Button("Remove"))
		{
			TILENODES.Remove(selectionNode);
			selectionNode = nullptr;
		}

		ImGui::SameLine();
		if (ImGui::Button("Duplicate"))
		{
			if (selectionNode != nullptr)
			{
				std::string fileName;

				switch (selectionNode->GetType())
				{
				case TileType::FLOOR:	fileName = "Data/Model/DungeonAssets/NewAssets/SM_Floor_01a.glb";			break;
				case TileType::WALL:	fileName = "Data/Model/DungeonAssets/NewAssets/SM_Wall_Pattern_01a.glb";	break;
				case TileType::PILLAR:	break;
				case TileType::STAIR:	fileName = "Data/Model/DungeonAssets/SLOPE.glb";	break;
				case TileType::SPAWNER:	break;
				}

				TileNode* newNode = new TileNode(fileName.c_str(), 1.0f);
				newNode->SetName(GetUniqueName(selectionNode->GetName()));
				newNode->SetType(selectionNode->GetType());
				newNode->SetPosition(selectionNode->GetPosition());
				newNode->SetAngle(selectionNode->GetAngle());
				newNode->SetScale(selectionNode->GetScale());
				TILENODES.Register(newNode);

				// 複製したノードを選択させる
				selectionNode = newNode;
			}
		}

		if (ImGui::Button("Clear All"))
		{
			for (TileNode* node : TILENODES.GetAll())
			{
				TILENODES.Remove(node);
				selectionNode = nullptr;
			}
		}

		if (ImGui::TreeNodeEx("Nodes", ImGuiTreeNodeFlags_DefaultOpen))
		{
			int index = 0;

			for (TileNode* node : TILENODES.GetAll())
			{
				ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;

				if (selectionNode == node)
				{
					nodeFlags |= ImGuiTreeNodeFlags_Selected;
				}

				ImGui::PushID(index);
				if (ImGui::TreeNodeEx(node->GetName().c_str(), nodeFlags))
				{
					if (ImGui::IsItemFocused()) selectionNode = node;
				}
				ImGui::PopID();
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
			std::string debugName = selectionNode->GetName();
			if (ImGui::InputText("Name", debugName.data(), 32))
			{
				selectionNode->SetName(debugName);
			}
		
			// pos
			DirectX::XMFLOAT3 debugTilePos = selectionNode->GetPosition();
			ImGui::DragFloat3("Position", &debugTilePos.x, 0.1f);
			selectionNode->SetPosition(debugTilePos);
			// angle
			DirectX::XMFLOAT3 debugTileAngle = selectionNode->GetAngle();
			debugTileAngle.x = DirectX::XMConvertToDegrees(debugTileAngle.x);
			debugTileAngle.y = DirectX::XMConvertToDegrees(debugTileAngle.y);
			debugTileAngle.z = DirectX::XMConvertToDegrees(debugTileAngle.z);
			ImGui::DragFloat3("Angle", &debugTileAngle.x);
			debugTileAngle.x = DirectX::XMConvertToRadians(debugTileAngle.x);
			debugTileAngle.y = DirectX::XMConvertToRadians(debugTileAngle.y);
			debugTileAngle.z = DirectX::XMConvertToRadians(debugTileAngle.z);
			selectionNode->SetAngle(debugTileAngle);
			// scale
			DirectX::XMFLOAT3 debugTileScale = selectionNode->GetScale();
			ImGui::DragFloat3("Scale", &debugTileScale.x, 0.1f);
			selectionNode->SetScale(debugTileScale);
		}
		ImGui::End();
	}
}
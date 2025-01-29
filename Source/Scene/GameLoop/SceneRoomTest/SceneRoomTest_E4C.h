#pragma once

#include <unordered_set>
#include <regex>

#include "TAKOEngine/Rendering/Model/ModelDX12.h"
#include "TAKOEngine/Rendering/SpriteDX12.h"
#include "TAKOEngine/Tool/GLTFImporter.h"
#include "TAKOEngine/Editor/Camera/FreeCameraController.h"
#include "TAKOEngine/AI/StateMachine.h"
#include "TAKOEngine/Tool/Singleton.h"

#include "Scene/Scene.h"

#include "GameObject/ObjectManager.h"
#include "GameObject/ModelObject.h"
#include "Map/DungeonData.h"
#include "Map/MapTile.h"
#include "Map/RoomBase.h"

using namespace ns_RoomData;

// ノード基底クラス
class Node : public ModelObject
{
public:
	Node(std::string name, TileType type, const char* fileName = "", float scaling = 1.0f) :
		ModelObject(fileName, scaling, RENDER_MODE::DX11), name(name), type(type)
	{
		if (T_GRAPHICS.isDX12Active)
		{
			SetShader(fileName, ModelShaderDX12Id::Toon);
		}
		else
		{

		}

		UpdateTransform();
	}
	~Node() = default;

	// 名前
	void SetName(const std::string newName) { this->name = newName; }
	const std::string GetName() const { return this->name; }

	// タイプ
	void SetType(const TileType newType) { this->type = newType; }
	const TileType GetType() const { return this->type; }

	// 自身を複製する
	virtual Node* Duplicate() { return nullptr; }

	// jsonへ保存
	virtual void SaveToJson(nlohmann::json_abi_v3_11_3::json& saveFile)
	{
		saveFile["NodeDatas"].push_back({
			{ "Type", type },
			{ "Position", { position.x, position.y, position.z }},
			{ "Angle", { angle.x, angle.y, angle.z }},
			{ "Scale", { scale.x, scale.y, scale.z }},
			});
	}

protected:
	std::string name;
	TileType type;
};


// ノード管理クラス
class NodeManager : public ObjectManager<Node>, public Singleton<NodeManager>
{
	friend class Singleton<NodeManager>;
public:
	// 正規表現を用いて重複しない名前を探す
	std::string GetUniqueName(std::string name);
	// レイキャスト
	Node* RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3 end);
};
#define NODES NodeManager::Instance()


// タイルノード
class TileNode : public Node
{
public:
	TileNode(std::string name, TileType type = TileType::FLOOR_01A, const char* fileName = "", float scaling = 1.0f) :
		Node(name, type, fileName, scaling) {};
	~TileNode() = default;

	Node* Duplicate() override;

	// デバッグGUI描画
	void DrawDebugGUI() override;
};


// スポナーノード
class SpawnerNode : public Node
{
public:
	SpawnerNode(std::string name) :
		Node(name, TileType::SPAWNER) {}

	void Render(const RenderContext& rc) override;

	Node* Duplicate() override;

	// デバッグGUI描画
	void DrawDebugGUI() override;

	// jsonへ保存
	void SaveToJson(nlohmann::json_abi_v3_11_3::json& saveFile) override
	{
		nlohmann::json spawnerJsonData;
		spawnerJsonData = {
			{ "EnemyType", spawnerData.enemyType },
			{ "SearchRadius", spawnerData.searchRadius }
		};

		saveFile["NodeDatas"].push_back({
			{ "Type", type },
			{ "Position", { position.x, position.y, position.z }},
			{ "Angle", { angle.x, angle.y, angle.z }},
			{ "Scale", { scale.x, scale.y, scale.z }},
			{ "SpawnerData", spawnerJsonData }
			});
	}

	// スポナーデータのゲッター、セッター
	SPAWNER_DATA GetSpawnerData() { return spawnerData; }
	void SetSpawnerData(SPAWNER_DATA newData) { spawnerData = newData; }

protected:
	SPAWNER_DATA spawnerData;
};

// 接続点ノード
class ConnectPointNode : public Node
{
public:
	ConnectPointNode(std::string name) :
		Node(name, TileType::CONNECTPOINT) {}

	void Render(const RenderContext& rc) override;

	Node* Duplicate() override;

	// デバッグGUI描画
	void DrawDebugGUI() override;
};

// 次の部屋へ行くためのノード
class StairToNextFloorNode : public Node
{
public:
	StairToNextFloorNode(std::string name) :
		Node(name, TileType::STAIR_TO_NEXTFLOOR) {}

	void Render(const RenderContext& rc) override;

	Node* Duplicate() override;

	// デバッグGUI描画
	void DrawDebugGUI() override;

	// jsonへ保存
	void SaveToJson(nlohmann::json_abi_v3_11_3::json& saveFile) override
	{
		nlohmann::json teleporterJsonData;
		teleporterJsonData = {
			{ "PortalTime", portalTime },
			{ "InteractionDistance", interactionDistance }
		};

		saveFile["NodeDatas"].push_back({
			{ "Type", type },
			{ "Position", { position.x, position.y, position.z }},
			{ "Angle", { angle.x, angle.y, angle.z }},
			{ "Scale", { scale.x, scale.y, scale.z }},
			{ "TeleporterData", teleporterJsonData }
			});
	}

	float GetPortalTime() { return portalTime; }
	void SetPortalTime(float newPortalTime) { portalTime = newPortalTime; }

	float GetInteractionDistance() { return interactionDistance; }
	void SetInteractionDistance(float newDistance) { interactionDistance = newDistance; }

protected:
	float portalTime = 3.0f;
	float interactionDistance = 5.0f;
};


class SceneRoomTest_E4C : public Scene
{
public:
	SceneRoomTest_E4C() = default;
	~SceneRoomTest_E4C() = default;

	// 初期化
	void Initialize() override;
	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;
	// 描画処理
	void Render() override;
	void RenderDX12() override;

	// データ一次保管用
	struct IMPORT_DATA
	{
		DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 angle = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };

		TileType type = TileType::FLOOR_01A;
	};

	// 部屋データを指定したjsonからロードする
	void LoadRoomData();
	// スポナーのロード
	void LoadSpawnerData(const auto& nodeData);

	// 部屋データをjsonにセーブする
	void SaveRoomData();

	// デバッグ描画
	void DrawDebugGUI();

	// TileNode追加
	void AddTileNode(
		std::string name = "Node", TileType type = TileType::FLOOR_01A,
		DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f },
		DirectX::XMFLOAT3 angle = { 0.0f, 0.0f, 0.0f },
		DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f });
	// Spawner追加
	void AddSpawner(
		std::string name = "Spawner",
		DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f });
	// ConnectPoint追加
	void AddConnectPoint(
		std::string name = "ConnectPoint",
		DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f },
		DirectX::XMFLOAT3 angle = { 0.0f, 0.0f, 0.0f });
	// StairToNextFloor追加
	void AddStairToNextFloor(
		std::string name = "StairToNextFloor",
		DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f });

	// ノード複製
	void DuplicateNode();
	// ノード削除
	void RemoveSelectedNode();
	// ノード全削除
	void ClearNodes();
	// 選択ノードを変更する
	void ChangeSelectedNode(Node* newNode);

	// ノードの座標からAABBを算出する
	void CalcAABB();

	// テンプレート
	// 3x3 Floor
	void AddTemplate3x3Floor();

	const std::string GetDefaultName(int index) const { return nodeDefaultNames.at(index); }

private:
	std::unique_ptr<myRenderer::shadow::ShadowMapRender> m_shadowMapRenderer = std::make_unique<myRenderer::shadow::ShadowMapRender>();
	FrameBufferManager* m_frameBuffer;
	std::unique_ptr<FreeCameraController> m_cameraController;

	DungeonData::RoomGenerateSetting roomSetting;	// 部屋の生成設定
	Node* selectionNode = nullptr;					// 現在選択しているノード

	// Sprite Preload
	std::unordered_set<const char*> m_spriteList = {
		"",											// マスク
		// Setting UI
		"Data/Sprites/UI/start.png",
		"Data/Sprites/UI/exit.png"
	};
	std::unordered_set<std::shared_ptr<Sprite>> m_spritePreLoad;

	std::vector<std::string> nodeDefaultNames;

	std::unique_ptr<ModelObject> testModel;
};
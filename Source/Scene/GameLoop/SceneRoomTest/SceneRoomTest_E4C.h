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
	Node(std::string name, const char* fileName = "", float scaling = 1.0f) :
		ModelObject(fileName, scaling), name(name) {}
	~Node() = default;

	void SetName(const std::string newName) { this->name = newName; }
	const std::string GetName() const { return this->name; }

	// 自身を複製する
	virtual Node* Duplicate() { return nullptr; }

protected:
	std::string name;
};


// ノード管理クラス
class NodeManager : public ObjectManager<Node>, public Singleton<NodeManager>
{
	friend class Singleton<NodeManager>;

public:
	// 正規表現を用いて重複しない名前を探す
	std::string GetUniqueName(std::string name);
};
#define NODES NodeManager::Instance()


// MapTileノード
class TileNode : public Node
{
public:
	TileNode(std::string name, TileType type = TileType::FLOOR, const char* fileName = "", float scaling = 1.0f) :
		Node(name, fileName, scaling), type(type) {};
	~TileNode() = default;

	void SetType(const TileType newType) { this->type = newType; }
	const TileType GetType() const { return this->type; }

	Node* Duplicate() override;

	// デバッグGUI描画
	void DrawDebugGUI() override;

private:
	TileType type;
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

	// 部屋データを指定したjsonからロードする
	void OpenRoomData();
	// 部屋データをjsonにセーブする
	void SaveRoomData();

	// デバッグ描画
	void DebugRender();

private:
	std::unique_ptr<myRenderer::shadow::ShadowMapRender> m_shadowMapRenderer = std::make_unique<myRenderer::shadow::ShadowMapRender>();
	FrameBufferManager* m_frameBuffer;
	std::unique_ptr<FreeCameraController> m_cameraController;

	Node* selectionNode = nullptr;					// 現在選択しているノード
	TileType selectionTileType = TileType::FLOOR;	// 次に新規作成するタイルのタイプ

	//std::vector<TILE_DATA> connectPointDatas;	// 接続点データ

	// Sprite Preload
	std::unordered_set<const char*> m_spriteList = {
		"",											// マスク
		// Setting UI
		"Data/Sprites/UI/start.png",
		"Data/Sprites/UI/exit.png"
	};
	std::unordered_set<std::shared_ptr<Sprite>> m_spritePreLoad;
};
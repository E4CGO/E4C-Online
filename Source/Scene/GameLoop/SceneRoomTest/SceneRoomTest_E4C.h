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

// 部屋テストシーンでのみ使う
class TileNode : public MapTile
{
public:
	TileNode(const char* filename, float scaling = 1.0f) :
		MapTile(filename, scaling, nullptr) {}
	~TileNode() = default;

	void SetName(std::string newName) { this->name = newName; }
	std::string GetName() { return name; }

	void SetType(TileType newType) { this->type = newType; }
	TileType GetType() { return type; }

private:
	std::string name = "";
	TileType type = TileType::FLOOR;
};

class TileNodeManager : public ObjectManager<TileNode>, public Singleton<TileNodeManager>
{
	friend class Singleton<TileNodeManager>;
//private:
//	TileNodeManager() = default;
//	~TileNodeManager() {}
};

#define TILENODES TileNodeManager::Instance()

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

	// 部屋データをjsonからロードする
	void LoadRoomData();
	// 部屋データをjsonにセーブする
	void SaveRoomData();

	// 次ノードの重複しない名前を探す
	std::string GetUniqueName(std::string name);

	// 正規表現された文字列から元の文字列に変換する
	std::string GetNameByRegEx(std::string regName);

	// デバッグ描画
	void DebugRender();

private:
	std::unique_ptr<myRenderer::shadow::ShadowMapRender> m_shadowMapRenderer = std::make_unique<myRenderer::shadow::ShadowMapRender>();
	FrameBufferManager* m_frameBuffer;
	std::unique_ptr<FreeCameraController> m_cameraController;

	//ModelObject* selectionObject = nullptr;
	TileNode* selectionNode = nullptr;
	TileType selectionTileType = TileType::FLOOR;

	// Sprite Preload
	std::unordered_set<const char*> m_spriteList = {
		"",											// マスク
		// Setting UI
		"Data/Sprites/UI/start.png",
		"Data/Sprites/UI/exit.png"
	};
	std::unordered_set<std::shared_ptr<Sprite>> m_spritePreLoad;

	//std::regex pattern;
};
#pragma once

#include "TAKOEngine/AI/BaseState.h"

#include <vector>

#include "Map/MapTileManager.h"
#include "Map/MapTile.h"

#include "Map/RoomBase.h"
#include "Map/SimpleRoom1.h"

#include "Scene/SceneDungeon.h"
#include "UI/Widget/WidgetText.h"

namespace SceneDungeonState
{
	enum DungeonState
	{
		GENERATE = 0,
		GAME,
	};

	// 方向
	enum Direction
	{
		LEFT = 0,
		RIGHT,
		TOP,
		BOTTOM,
	};

	// ダンジョン生成ステート
	class GenerateState : public HierarchicalState<SceneDungeon>
	{
	public:
		// コンストラクタ
		GenerateState(SceneDungeon* scene) : HierarchicalState<SceneDungeon>(scene) {};

		// デストラクタ
		~GenerateState();

		// ステートに入った時のメソッド
		virtual void Enter() override;

		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;

		// ステートから出ていくときのメソッド
		void Exit() override;

		// 生成設定のゲッター、セッター
		//DUNGEON_SETTINGS GetDungeonSettings() { return m_DungeonSettings; }
		//void SetDungeonSettings(DUNGEON_SETTINGS newSetting) { m_DungeonSettings = newSetting; }

	private:
		// 生成設定を元にマップデータの初期化を行う
		//void InitializeMapData();

		// 座標の指定した方角が隅であるかどうかを調べる
		//bool IsCorner(MAP_COORD coord, Direction direction);

		// 部屋の指定した方角が隅であるかどうかを調べる
		//bool IsCorner(ROOM_DATA roomData, Direction direction);

		// 接続点データから部屋を生成できるかを調べ、
		// 生成できる最大サイズの部屋データを算出する
		//bool CheckCreateNewRoom(
		//	CONNECTION_POINT connectionPoint,
		//	ROOM_DATA& maxRoomData);

		// 部屋をダンジョンタイル配列に書き込む
		//void WriteRoomData(ROOM_DATA data);

		// 部屋に接続点を追加する
		//void AddConnectionPoint(ROOM_DATA data);

		// 部屋を生成する
		void CreateRoom();

		// 部屋データを元にモデルを配置する
		//void PlaceRoom(ROOM_DATA roomData);



		//int m_TileScale = 4.0f;	// タイルのスケール

		RoomBase* rootRoom;	// 最初の部屋

		//DUNGEON_SETTINGS m_DungeonSettings;			// ダンジョンの生成設定
		//std::vector<std::vector<FloorType>> m_Map;	// ダンジョンタイル保存用配列
	};

	// ゲームステート
	class GameState : public HierarchicalState<SceneDungeon>
	{
	public:
		// コンストラクタ
		GameState(SceneDungeon* scene) : HierarchicalState<SceneDungeon>(scene) {};

		// デストラクタ
		~GameState() {}

		// ステートに入った時のメソッド
		virtual void Enter() override;

		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;

		// ステートから出ていくときのメソッド
		void Exit() override;
	};
}
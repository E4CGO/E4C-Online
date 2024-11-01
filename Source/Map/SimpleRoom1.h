#pragma once

#include "RoomBase.h"

class SimpleRoom1 : public RoomBase
{
public:
	// コンストラクタ（配列から生成を行う）
	SimpleRoom1(
		RoomBase* parent,
		int pointIndex,
		std::vector<int> roomTree,
		int& treeIndex);

	// コンストラクタ（乱数を用いて生成を行う）
	SimpleRoom1(
		RoomBase* parent,
		int pointIndex);

	void Update(float elapsedTime) override;

	void PlaceMapTile() override;

	int DrawDebugGUI(int i = 0) override;

private:
	std::vector<MapTile*> tiles;
};
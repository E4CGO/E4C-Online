#pragma once

#include "RoomBase.h"

class Passage1 : public RoomBase
{
public:
	Passage1(
		RoomBase* parent,
		int pointIndex,
		std::vector<int> roomTree,
		int& treeIndex);

	Passage1(
		RoomBase* parent,
		int pointIndex);

	void LoadMapTileData() override;

	void PlaceMapTile() override;
};
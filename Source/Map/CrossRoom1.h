#pragma once

#include "RoomBase.h"

class CrossRoom1 : public RoomBase
{
public:
	CrossRoom1(
		RoomBase* parent,
		int pointIndex,
		std::vector<int> roomTree,
		int& treeIndex);

	CrossRoom1(
		RoomBase* parent,
		int pointIndex);

	void LoadMapTileData() override;

	void PlaceMapTile() override;
};
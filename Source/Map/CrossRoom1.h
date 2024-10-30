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

	void PlaceMapTile() override;

	int DrawDebugGUI(int i = 0) override;
};
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

	void PlaceMapTile() override;

	int DrawDebugGUI(int i = 0) override;
};
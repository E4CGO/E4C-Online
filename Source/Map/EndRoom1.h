#pragma once

#include "RoomBase.h"

class EndRoom1 : public RoomBase
{
public:
	EndRoom1(
		RoomBase* parent,
		int pointIndex,
		std::vector<int> roomTree,
		int& treeIndex);

	EndRoom1(
		RoomBase* parent,
		int pointIndex);

	void PlaceMapTile() override;

	void PlaceExit() override;

	int DrawDebugGUI(int i = 0) override;
};
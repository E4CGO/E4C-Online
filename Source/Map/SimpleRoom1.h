#pragma once

#include "RoomBase.h"

class SimpleRoom1 : public RoomBase
{
public:
	SimpleRoom1(
		RoomBase* parent, int pointIndex,
		std::vector<AABB>& roomAABBs,
		bool isAutoGeneration,
		std::vector<uint8_t> roomOrder, int& orderIndex);

	void LoadMapData() override;
};
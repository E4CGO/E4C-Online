#pragma once

#include "RoomBase.h"

class CrossRoom1 : public RoomBase
{
public:
	CrossRoom1(
		RoomBase* parent, const int pointIndex,
		std::vector<AABB>& roomAABBs,
		const bool isAutoGeneration,
		std::vector<uint8_t>& roomOrder, int& orderIndex);

	void LoadMapData() override;
};
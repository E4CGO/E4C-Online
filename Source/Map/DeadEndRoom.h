#pragma once

#include "RoomBase.h"

class DeadEndRoom : public RoomBase
{
public:
	DeadEndRoom(
		RoomBase* parent, const int pointIndex,
		std::vector<AABB>& roomAABBs,
		const bool isAutoGeneration,
		const std::vector<uint8_t> roomOrder, int& orderIndex);

	void LoadMapData() override;
};
#pragma once

#include "RoomBase.h"

// CrossRoomのスポナーありバージョン
class CrossRoom2 : public RoomBase
{
public:
	CrossRoom2(
		RoomBase* parent, const int pointIndex,
		std::vector<AABB>& roomAABBs,
		const bool isAutoGeneration,
		std::vector<uint8_t>& roomOrder, int& orderIndex);

	void LoadMapData() override;
};
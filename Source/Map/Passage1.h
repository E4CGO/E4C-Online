﻿#pragma once

#include "RoomBase.h"

class Passage1 : public RoomBase
{
public:
	Passage1(
		RoomBase* parent, const int pointIndex,
		std::vector<AABB>& roomAABBs,
		const bool isAutoGeneration,
		std::vector<uint8_t>& roomOrder, int& orderIndex);

	void LoadMapData() override;
};
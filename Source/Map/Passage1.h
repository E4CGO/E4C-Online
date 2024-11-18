#pragma once

#include "RoomBase.h"

class Passage1 : public RoomBase
{
public:
	Passage1(
		RoomBase* parent,
		int pointIndex);

	void LoadMapData() override;
};
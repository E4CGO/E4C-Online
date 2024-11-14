#pragma once

#include "RoomBase.h"

class SimpleRoom1 : public RoomBase
{
public:
	SimpleRoom1(
		RoomBase* parent,
		int pointIndex);

	void LoadMapData() override;
};
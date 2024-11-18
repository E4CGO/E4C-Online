#pragma once

#include "RoomBase.h"

class EndRoom1 : public RoomBase
{
public:
	EndRoom1(
		RoomBase* parent,
		int pointIndex);

	void LoadMapData() override;
};
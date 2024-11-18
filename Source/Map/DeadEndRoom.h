#pragma once

#include "RoomBase.h"

class DeadEndRoom : public RoomBase
{
public:
	DeadEndRoom(
		RoomBase* parent,
		int pointIndex);

	void LoadMapData() override;
};
#pragma once

#include "RoomBase.h"

class CrossRoom1 : public RoomBase
{
public:
	CrossRoom1(
		RoomBase* parent,
		int pointIndex);

	void LoadMapData() override;
};
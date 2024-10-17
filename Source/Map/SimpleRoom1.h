#pragma once

#include "RoomBase.h"

class SimpleRoom1 : public RoomBase
{
public:
	SimpleRoom1(RoomBase* parent = nullptr, DirectX::XMFLOAT3 moveValue = { 0.0f, 0.0f, 0.0f });

	void PlaceMapTile() override;
};
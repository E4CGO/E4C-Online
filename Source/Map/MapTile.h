#pragma once

#include "GameObject/ModelObject.h"
#include "Map/RoomBase.h"

class MapTile : public ModelObject
{
public:
	MapTile(const char* filename, float scaling = 1.0f, RoomBase* parent = nullptr);
	~MapTile() = default;

	void UpdateTransform() override;

	void Update(float elapsedTime) override;

protected:
	RoomBase* parent = nullptr;
};
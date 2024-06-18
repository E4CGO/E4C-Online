#pragma once

#include "GameObject/ModelObject.h"

class MapTile : public ModelObject
{
public:
	MapTile(const char* filename, float scaling = 1.0f);
	~MapTile() = default;
protected:
};

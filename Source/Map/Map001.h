#pragma once

#include "MapTile.h"

class Map001 : public MapTile
{
public:
	Map001();
	~Map001() = default;

	virtual void Update(float elapsedTime) override;
private:
	std::unique_ptr<Model> mapCollider;
};

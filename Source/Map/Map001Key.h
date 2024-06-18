#pragma once

#include "GameObject/ModelObject.h"

class Map001Key : public ModelObject
{
public:
	Map001Key();
	~Map001Key() {}

	// XVˆ—
	virtual void Update(float elapsedTime) override;
	// •`‰æˆ—
	virtual void Render(const RenderContext& rc) override;
};

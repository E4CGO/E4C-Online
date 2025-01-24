#pragma once

#include "GameObject/ModelObject.h"

class Map001Key : public ModelObject
{
public:
	Map001Key();
	~Map001Key() {}

	// 更新処理
	virtual void Update(float elapsedTime) override;
	// 描画処理
	virtual void Render(const RenderContext& rc) override;
};

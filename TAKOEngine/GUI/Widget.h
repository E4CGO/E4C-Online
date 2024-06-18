#pragma once

#include <DirectXMath.h>

#include "TAKOEngine/Rendering/RenderContext.h"

class Widget
{
public:
	Widget() {};
	virtual ~Widget() {};

	virtual void Update(float elapsedTime) {};
	virtual void Render(const RenderContext& rc) = 0;
public:
	// アクセス
	virtual void SetPosition(const DirectX::XMFLOAT2& position) { this->position = position; }
	const DirectX::XMFLOAT2& GetPosition() const { return position; }
	virtual void SetSize(const DirectX::XMFLOAT2& size) { this->size = size; }
	const DirectX::XMFLOAT2& GetSize() const { return size; }
protected:
	DirectX::XMFLOAT2 position = {};
	DirectX::XMFLOAT2 size = {};
};

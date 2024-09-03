#pragma once

#include <DirectXMath.h>

struct CbScene
{
	DirectX::XMFLOAT4X4	view_projection;
	DirectX::XMFLOAT4	light_direction;
	DirectX::XMFLOAT4 camera_position;
};

struct CbLambertMaterial
{
	DirectX::XMFLOAT4	color;
};
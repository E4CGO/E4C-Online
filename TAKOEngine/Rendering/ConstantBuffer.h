#pragma once

#include <DirectXMath.h>

#include "RenderContext.h"

//TODO::SetCbScene
struct CbScene
{
	DirectX::XMFLOAT4X4	view_projection;
	DirectX::XMFLOAT4   camera_position;

	//ライト情報
	DirectX::XMFLOAT4				ambientLightColor;
	DirectionalLightData				directionalLightData;
	PointLightData					pointLightData[PointLightMax];			// 点光源情報
	SpotLightData					spotLightData[SpotLightMax];			// スポットライト情報
	int						pointLightCount = 0;				// 点光源数
	int						spotLightCount = 0;				// スポットライト数
};

struct CbLambertMaterial
{
	DirectX::XMFLOAT4	color;
};

//! @file ConstantBuffer.h
//! @note

#ifndef __INCLUDED_CONSTANT_BUFFER_H__
#define __INCLUDED_CONSTANT_BUFFER_H__

#include <DirectXMath.h>

//TODO::SetCbScene
struct CbScene
{
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
	DirectX::XMFLOAT4   camera_position;
	DirectX::XMFLOAT4X4	light_view_projection;

	//ライト情報
	DirectX::XMFLOAT4		ambientLightColor;
	DirectionalLightData	directionalLightData;
	PointLightData			pointLightData[PointLightMax];	// 点光源情報
	SpotLightData			spotLightData[SpotLightMax];	// スポットライト情報
	int						pointLightCount = 0;			// 点光源数
	int						spotLightCount = 0;				// スポットライト数

	float					timerGlobal;
	float					timerTick;

	// 影情報
	float shadowBias;
	DirectX::XMFLOAT3 shadowColor;

	// 草情報
	float grass_height_factor;
	float grass_width_factor;
	float grass_curvature;
	float grass_withered_factor;
	float grass_height_variance;
	float perlin_noise_distribution_factor;
	float tesselation_max_subdivision;
	float tesselation_max_distance;
	DirectX::XMFLOAT4 grass_specular_color;

	DirectX::XMFLOAT4 avatar_position;
	float wind_frequency;
	float wind_strength;
};

struct CbLambertMaterial
{
	DirectX::XMFLOAT4	color;
};

#endif //!__INCLUDED_CONSTANT_BUFFER_H__

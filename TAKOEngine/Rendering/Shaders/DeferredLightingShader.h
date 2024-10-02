#pragma once

#include "SpriteShader.h"

class DeferredLightingShader : public SpriteShader
{
public:
	DeferredLightingShader(ID3D11Device* device);
	~DeferredLightingShader() override = default;

	void Draw(const RenderContext& rc, const Sprite* sprite) override;
	void End(const RenderContext& rc) override;

private:
	void UpdateConstantBuffer(const RenderContext& rc) override;

	struct CbDeferred
	{
		DirectX::XMFLOAT4		cameraPosition;
		DirectX::XMFLOAT4X4		viewProjection;
		DirectX::XMFLOAT4		ambientLightColor;
		DirectionalLightData	directionalLightData;
		PointLightData			pointLightData[PointLightMax];	// 点光源
		SpotLightData			spotLightData[SpotLightMax];	// スポットライト
		int						pointLightCount;				// 点光源数
		int						spotLightCount;					// スポットライト数
		DirectX::XMFLOAT2		dummy;
	};
};
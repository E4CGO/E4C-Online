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
		PointLightData			pointLightData[PointLightMax];	// �_����
		SpotLightData			spotLightData[SpotLightMax];	// �X�|�b�g���C�g
		int						pointLightCount;				// �_������
		int						spotLightCount;					// �X�|�b�g���C�g��
		DirectX::XMFLOAT2		dummy;
	};
};
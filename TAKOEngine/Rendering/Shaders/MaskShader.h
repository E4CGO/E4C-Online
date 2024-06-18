#pragma once
#include "SpriteShader.h"
class MaskShader : public SpriteShader
{
public:
	MaskShader(ID3D11Device* device);
private:
	void UpdateConstantBuffer(const RenderContext& rc) override;
private:
	// 閾値を送るための定数バッファ用構造体
	struct CbMask
	{
		float			  dissolveThreshold;	// ディゾルブ量
		float			  edgeThreshold; 		// 縁の閾値
		DirectX::XMFLOAT2 dummy;
		DirectX::XMFLOAT4 edgeColor;			// 縁の色
	};
};

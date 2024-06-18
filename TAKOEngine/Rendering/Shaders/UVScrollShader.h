#pragma once
#include "SpriteShader.h"
class UVScrollShader : public SpriteShader
{
public:

	UVScrollShader(ID3D11Device* device);
private:
	void UpdateConstantBuffer(const RenderContext& rc) override;
private:
	struct CbScroll
	{
		DirectX::XMFLOAT2 uvScrollValue;	// UVスクロル値
		DirectX::XMFLOAT2 dummy;			// ダミー
	};
};

#pragma once
#include "SpriteShader.h"

class FinalpassShader : public SpriteShader
{
public:
	FinalpassShader(ID3D11Device* device);

	void Draw(const RenderContext& rc, const Sprite* sprite) override;
private:
	void UpdateConstantBuffer(const RenderContext& rc) override;
private:
	struct CBFinalpass
	{
		//	色調補正
		float	hueShift;	// 色相調整
		float	saturation;	// 彩度調整
		float	brightness;	// 明度調整
		float	dummy;
	};
};

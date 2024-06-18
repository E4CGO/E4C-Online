#pragma once
#include "SpriteShader.h"
class ColorGradingShader : public SpriteShader
{
public:
	ColorGradingShader(ID3D11Device* device);
private:
	void UpdateConstantBuffer(const RenderContext& rc) override;
private:
	struct CBColorGrading
	{
		float	hueShift;	// F‘Š’²®
		float	saturation;	// Ê“x’²®
		float	brightness;	// –¾“x’²®
		float	dummy;
	};
};

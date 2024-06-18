#include "ColorGrading.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture0.Sample(sampler0, pin.texcoord) * pin.color;
	// RGB > HSV‚É•ÏŠ·
    color.rgb = RGB2HSV(color.rgb);

	// F‘Š’²®
    color.r += hueShift;
    
	// Ê“x’²®
    color.g *= saturation;

	// –¾“x’²®
    color.b *= brightness;

	// HSV > RGB‚É•ÏŠ·
    color.rgb = HSV2RGB(color.rgb);

    return color;
}

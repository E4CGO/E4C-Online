#include "FilterFunctions.hlsli"
#include "ColorGradingDX12.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture0.Sample(sampler0, pin.texcoord) * pin.color;
	
    // RGB > HSVに変換
    color.rgb = RGB2HSV(color.rgb);

	// 色相調整
    color.r += hueShift;
    
	// 彩度調整
    color.g *= saturation;

	// 明度調整
    color.b *= brightness;

	// HSV > RGBに変換
    color.rgb = HSV2RGB(color.rgb);

    return color;
}

#include "FilterFunctions.hlsli"
#include "LuminanceExtractionDX12.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture0.Sample(sampler0, pin.texcoord) * pin.color;

	// RGB > 輝度値に変換
    float luminance = RGB2Luminance(color.rgb);
    
	// 閾値との差を算出
    float contribution = max(0, luminance - threshold);

	// 出力する色を補正する
    contribution /= luminance;
    color.rgb *= contribution * intensity;
    
    clip(color.a - 0.2);
    
    return color;
}
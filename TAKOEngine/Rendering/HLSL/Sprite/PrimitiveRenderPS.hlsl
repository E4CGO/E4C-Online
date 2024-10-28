#include "PrimitiveRender.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);
// ピクセルシェーダーエントリポイント
float4 main(VS_OUT pin) : SV_TARGET
{
   // float4 color = texture0.Sample(sampler0, pin.texcoord)*pin.color;
    return pin.color;
    //return color;
}

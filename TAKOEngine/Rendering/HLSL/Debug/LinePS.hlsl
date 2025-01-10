
#include "Line.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);
float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture0.Sample(sampler0, pin.texcoord)*pin.color;
    
    //if (color.r <= 0.1 && color.b <= 0.1 && color.g <= 0.1)
    //    discard;
    if (color.a < 0.1f)
        discard;
        return color;
}

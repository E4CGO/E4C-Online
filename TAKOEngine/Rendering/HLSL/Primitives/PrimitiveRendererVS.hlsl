#include "PrimitiveRenderer.hlsli"

VS_OUT main(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD)
{
    VS_OUT vout;
    vout.position = mul(position, viewProjection);
    vout.color = color;
    return vout;
}
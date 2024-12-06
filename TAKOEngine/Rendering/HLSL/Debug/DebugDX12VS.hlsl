#include "DebugDX12.hlsli"

[RootSignature(ROOT_SIG)]
VS_OUT main(float4 pos : POSITION)
{
    VS_OUT vout;
    vout.position = mul(pos, worldViewProjection);
    vout.color    = color;

    return vout;
}
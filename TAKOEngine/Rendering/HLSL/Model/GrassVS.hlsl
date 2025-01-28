#include "Grass.hlsli"

[RootSignature(ROOT_SIG)]
VS_OUT main(float4 position : POSITION)
{
    VS_OUT vout;
    vout.position = position;
    return vout;
}
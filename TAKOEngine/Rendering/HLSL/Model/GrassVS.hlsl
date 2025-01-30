#include "Grass.hlsli"

[RootSignature(ROOT_SIG)]
VS_OUT main(VS_IN vin)
{
    VS_OUT vout;
    vout.position = mul(vin.position, world_transform);
    vin.normal.w = 0;
    vout.normal = normalize(mul(vin.normal, world_transform));
    
    float sigma = vin.tangent.w;
    vin.tangent.w = 0;
    vout.tangent = normalize(mul(vin.tangent, world_transform));
    vout.tangent.w = sigma;
    
    return vout;
}
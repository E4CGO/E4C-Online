#include "ShadowMapDX12.hlsli"

[RootSignature(ROOT_SIG)]
float4 main( float4 pos : POSITION ) : SV_POSITION
{
    return mul(pos, mul(world_transform, lightViewProjection));
}
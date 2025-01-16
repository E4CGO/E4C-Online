#include "HitParticle.hlsli"

[RootSignature(ROOT_SIG)]
VS_OUT main(uint vertex_id : SV_VERTEXID)
{
    VS_OUT vout;
    vout.vertex_id = vertex_id;
    
    return vout;
}
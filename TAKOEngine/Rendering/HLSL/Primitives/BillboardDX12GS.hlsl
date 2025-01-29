#include "BillboardDX12.hlsli"

[maxvertexcount(4)]
void main(
	point GS_INPUT input[1] : SV_POSITION,
	inout TriangleStream<PSInput> output
)
{
    const float2 corners[] =
    {
        float2(-1.0f, -1.0f),
        float2(-1.0f, +1.0f),
        float2(+1.0f, -1.0f),
        float2(+1.0f, +1.0f),
    };
        
    const float2 texcoord[] =
    {
        float2(0.0f, 1.0f),
        float2(0.0f, 0.0f),
        float2(1.0f, 1.0f),
        float2(1.0f, 0.0f),
    };
    
    const float aspect = 1920.0 / 1080.0;
    float2 particle_scale = float2(WorldMatrix[0][0], WorldMatrix[1][1] * aspect);
    
    [unroll]
    for (uint vert_index = 0; vert_index < 4; ++vert_index)
    {
        PSInput ps;
        float4x4 viewProjection = mul(view, Projection);
        ps.position = mul(input[0].Position, viewProjection);
        ps.position.xy += corners[vert_index] * particle_scale;
        
        ps.color = input[0].Color;
        ps.texcoord = texcoord[vert_index];
        
        output.Append(ps);
        
    }
    
    output.RestartStrip();
    
}
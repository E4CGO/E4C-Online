#include "Billboard.hlsli"

[maxvertexcount(4)]
void main(
	point GSInput input[1],
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
    
    const float aspect = 1280.0 / 720.0;
    float2 particle_scale = float2(2.0f, 2.0f * aspect);
    
    [unroll]
    for (uint vert_index = 0; vert_index < 4; ++vert_index)
    {
        PSInput ps;
        
        ps.position = mul(input[0].Position, ViewProjection);
        ps.position.xy += corners[vert_index] * particle_scale;
        
        ps.color = input[0].Color;
        ps.texcoord = texcoord[vert_index];
        
        output.Append(ps);
        
    }
    
    output.RestartStrip();
    
}
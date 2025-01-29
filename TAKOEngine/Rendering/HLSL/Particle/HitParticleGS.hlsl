#include "HitParticle.hlsli"

StructuredBuffer<particle> particle_buffer : register(t0);

[maxvertexcount(4)]
void main(point VS_OUT input[1], inout TriangleStream<GS_OUT> output)
{
    const float2 corners[] =
    {
		float2(-1.0f, -1.0f),
		float2(-1.0f, +1.0f),
		float2(+1.0f, -1.0f),
		float2(+1.0f, +1.0f),
    };

    const float2 texcoords[] =
    {
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f),
    };
	
    particle p = particle_buffer[input[0].vertex_id];
    if (p.age < 0.0f)
    {
        return;
    }
	
    const float aspect_ratio = 1280.0f / 720.0f;
    float2 size = p.size;
    
    float3 view_space_velocity = mul(float4(p.velocity, 0.0f), view).xyz;
    float4 view_space_pos      = mul(float4(p.position, 1.0f), view);
    
    [unroll]
    for (uint vertex_index = 0; vertex_index < 4; vertex_index++)
    {
        GS_OUT element;
        
        // クリップ空間への変換
        float4x4 viewProjection = mul(view, Projection); 
        element.position = mul(float4(p.position.xyz, 1), viewProjection);
        
        // コーナー位置をビルボードとして設定する
        float2 corner;
        corner = corners[vertex_index];
        
        float2 streak = max(0, dot(normalize(-view_space_velocity), normalize(float3(corner, 0.0f)))) * -view_space_velocity.xy;
        float streak_factor = 15.0f;
        corner = corner + streak_factor * streak;
        
        element.position.xy += corner * float2(size.x, size.y * aspect_ratio);
        element.color = p.color;
        element.texcoord = texcoords[vertex_index];
        
        output.Append(element);
    }
    output.RestartStrip();
}
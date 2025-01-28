#include "Grass.hlsli" 

float random(in float2 st)
{
    return frac(sin(dot(st.xy, float2(12.9898, 78.233))) * 43758.5453123);
}

float noise(in float2 st)
{
    float2 i = floor(st);
    float2 f = frac(st);

    float a = random(i);
    float b = random(i + float2(1.0, 0.0));
    float c = random(i + float2(0.0, 1.0));
    float d = random(i + float2(1.0, 1.0));

    float2 u = f * f * (3.0 - 2.0 * f);

    return lerp(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

float4x4 angle_axis(float angle, float3 axis)
{
    float c, s;
    sincos(angle, s, c);

    float t = 1 - c;
    float x = axis.x;
    float y = axis.y;

    float z = axis.z;
    return float4x4(
		t * x * x + c, t * x * y - s * z, t * x * z + s * y, 0,
		t * x * y + s * z, t * y * y + c, t * y * z - s * x, 0,
		t * x * z - s * y, t * y * z + s * x, t * z * z + c, 0,
		0, 0, 0, 1
		);
}

[maxvertexcount(3)]
void main(triangle DS_OUT input[3], inout TriangleStream<GS_OUT> output)
{
    float4 positions[3];
    positions[0] = mul(input[0].position, world_transform);
    positions[1] = mul(input[1].position, world_transform);
    positions[2] = mul(input[2].position, world_transform);

    float4 midpoint_position = (positions[0] + positions[1] + positions[2]) / 3;
    float4 midpoint_normal = float4(normalize(cross(positions[2].xyz - positions[0].xyz, positions[1].xyz - positions[0].xyz)), 0);
    float4 midpoint_tangent = float4(1, 0, 0, 0);
    
    const float noise_factor = noise(midpoint_position.xy * noise_seed_multiplier);
    float grass_blade_height = lerp(m_grass_blade_height * 0.2, m_grass_blade_height, noise_factor); 
    float grass_blade_width = lerp(m_grass_blade_width * 0.2, m_grass_blade_width, noise_factor);
    midpoint_position.xz += (noise_factor * 2.0 - 1.0) * 0.5;
    float4x4 R = angle_axis(noise_factor * 3.14159265358979, midpoint_normal.xyz);
    midpoint_tangent = mul(midpoint_tangent, R);

    GS_OUT element;
    element.normal = midpoint_normal;

    float4x4 viewProjection = mul(view, Projection);
    
    element.position = midpoint_position + midpoint_normal * grass_blade_height;
    element.sv_position = mul(element.position, viewProjection);
    element.texcoord = float2(0.5, 0.0);
    output.Append(element);

    element.position = midpoint_position + midpoint_tangent * grass_blade_width;
    element.sv_position = mul(element.position, viewProjection);
    element.texcoord = float2(0.0, 1.0);
    output.Append(element);

    element.position = midpoint_position - midpoint_tangent * grass_blade_width;
    element.sv_position = mul(element.position, viewProjection);
    element.texcoord = float2(1.0, 1.0);
    output.Append(element);

    output.RestartStrip();
}
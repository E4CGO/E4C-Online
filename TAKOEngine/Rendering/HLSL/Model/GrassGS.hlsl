#include "Grass.hlsli" 

Texture2D distortion_texture : register(t1);
Texture2D densityMap : register(t2);

SamplerState sampler0 : register(s0);

float random(in float2 st)
{
    return frac(sin(dot(st.xy, float2(12.9898, 78.233))) * 43758.5453123);
}

float4 mod289(float4 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}
float4 perm(float4 x)
{
    return mod289(((x * 34.0) + 1.0) * x);
}
float noise(float3 p)
{
    float3 a = floor(p);
    float3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    float4 b = a.xxyy + float4(0.0, 1.0, 0.0, 1.0);
    float4 k1 = perm(b.xyxy);
    float4 k2 = perm(k1.xyxy + b.zzww);

    float4 c = k2 + a.zzzz;
    float4 k3 = perm(c);
    float4 k4 = perm(c + 1.0);

    float4 o1 = frac(k3 * (1.0 / 41.0));
    float4 o2 = frac(k4 * (1.0 / 41.0));

    float4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    float2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
}

#define PI 3.141592653
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
		0, 0, 0, 1);
}

#define BLADE_SEGMENTS 5
[maxvertexcount(BLADE_SEGMENTS * 2 + 1)]
void main(triangle DS_OUTPUT input[3], inout TriangleStream<GS_OUT> output)
{
    float2 uv = float2((input[0].position.x / 150.0f + 0.5f), (input[0].position.z / -150.0f + 0.5f));
    uv = clamp(uv, 0.0f, 1.0f);
    float density = densityMap.SampleLevel(sampler0, uv, 0).g;
    
    if (density < 0.5f)
    {
        return;
    }
    
    const float perlin_noise = noise(input[0].position.xyz * perlin_noise_distribution_factor);
    const float grass_blade_height = grass_height_factor + (perlin_noise * 2.0 - 1.0) * grass_height_variance * density;
    const float grass_blade_width = grass_width_factor * density;
    const float4 withered_color = float4(perlin_noise * grass_withered_factor, 0.0, 0.0, 1.0);
    
    const float s = random(input[1].position.xy);
    const float t = random(input[2].position.yz);
    float4 midpoint_position = lerp(input[0].position, lerp(input[1].position, input[2].position, s), t);
    midpoint_position.w = 1;

    float4 midpoint_normal = normalize((input[0].normal + input[1].normal + input[2].normal) / 3);
    float4 midpoint_tangent = normalize((input[0].tangent + input[1].tangent + input[2].tangent) / 3);

    const float random_facing = random(input[0].position.zx);
    const row_major float4x4 R = angle_axis(random_facing * PI, midpoint_normal.xyz);
    midpoint_tangent = mul(midpoint_tangent, R);

    const float2 distortion_texcoord = midpoint_position.xz + wind_frequency * timerGlobal;
    const float4 distortion = distortion_texture.SampleLevel(sampler0, distortion_texcoord * 0.001, 0) * 2 - 1;
    const float wind_avatar_bending_angle = distortion.y * PI * 0.5 * wind_strength;
    const float3 wind_bending_axis = normalize(float3(distortion.x, 0, distortion.z));
    const float4x4 W = angle_axis(wind_avatar_bending_angle, wind_bending_axis);

    const float4 avatar_offset = avatar_position - midpoint_position;
    const float avatar_distance = length(avatar_offset);
    const float impact_radius = 5;
    const float avatar_bending_angle = smoothstep(impact_radius, 0, avatar_distance);
    const float3 avatar_bending_axis = normalize(cross(midpoint_normal.xyz, normalize(avatar_offset).xyz));
    const row_major float4x4 A = angle_axis(avatar_bending_angle * PI * 0.2, avatar_bending_axis);
    const row_major float4x4 B = mul(W, A);

    const float random_curvature = random(input[0].position.xy * 0.01);
    float curvature = PI * 0.5 * (random_curvature * 2.0 - 1.0) * grass_curvature;
    const row_major float4x4 C = angle_axis(curvature / BLADE_SEGMENTS, midpoint_tangent.xyz);
    float4 segment_normal = midpoint_normal;

    GS_OUT element;
    for (int i = 0; i < BLADE_SEGMENTS; i++)
    {
        float4x4 viewProjection = mul(view, Projection);
        
        float t = i / (float) BLADE_SEGMENTS;
        float segment_height = grass_blade_height * t;
        float segment_width = grass_blade_width * (1 - t);

        element.normal = segment_normal;
        element.color = withered_color;

        element.position = midpoint_position + segment_normal * segment_height + midpoint_tangent * segment_width;
        element.position = mul(element.position - midpoint_position, B) + midpoint_position;
        element.sv_position = mul(element.position, viewProjection);
        element.texcoord = float2(0, 1 - t);
        output.Append(element);

        element.position = midpoint_position + segment_normal * segment_height - midpoint_tangent * segment_width;
        element.position = mul(element.position - midpoint_position, B) + midpoint_position;
        element.sv_position = mul(element.position, viewProjection);
        element.texcoord = float2(1, 1 - t);
        output.Append(element);

        segment_normal = mul(segment_normal, C);
    }
    float4x4 viewProjection = mul(view, Projection);
    element.position = midpoint_position + segment_normal * grass_blade_height;
    element.position = mul(element.position - midpoint_position, B) + midpoint_position;
    element.sv_position = mul(element.position, viewProjection);
    element.normal = segment_normal;
    element.color = withered_color;
    element.texcoord = float2(0.5, 0);
    output.Append(element);

    output.RestartStrip();
}
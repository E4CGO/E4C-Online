#include "../Lighting/ShadowMap.hlsli"

struct Default
{
    float4 ka;
    float4 kd;
    float4 ks;
    float shiness;
    float3 dummy;
};

struct Phong
{
    float4 ka;
    float4 kd;
    float4 ks;
    float shiness;
    float3 dummy;
};

struct Toon
{
    float4 ka;
    float4 kd;
    float4 ks;
    float shiness;
    float3 dummy;
};

struct ShaderData
{
    Default defaultShader;
    Phong   phongShader;
    Toon    toonShader;
};
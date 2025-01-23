#include "../Primitives/PlaneDX12.hlsli"

Texture2D spriteTexture : register(t0);
SamplerState spriteSampler : register(s0);

static float uv_factor = 0.3f;
static float timeMulti = 0.2f;
static float alpha = 5.7;
static bool ready = false;

// ピクセルシェーダーエントリポイント
// Modified From: https://www.shadertoy.com/view/WtfXRj
float rand(float2 p)
{
    return frac(sin(dot(p, float2(127.1, 311.7))) * 43758.5453123);
}

float noise(float2 p)
{
    float2 i = floor(p);
    float2 f = frac(p);
    float2 u = f * f * (3.0 - 2.0 * f);
    float a = rand(i + float2(0, 0));
    float b = rand(i + float2(1, 0));
    float c = rand(i + float2(0, 1));
    float d = rand(i + float2(1, 1));
    return float(a + (b - a) * u.x + (c - a) * u.y + (a - b - c + d) * u.x * u.y) / 4.;
}

float mirror(float t, float r)
{
    t = frac(t + r);
    return 2. * abs(t - 0.5);
}

float radialNoise(float t, float d)
{
    const float2x2 m2 = float2x2(0.90, 0.44, -0.44, 0.90);
    const float SCALE = 45.;
    d = pow(d, 0.01);
    float doff = -timerGlobal * timeMulti;
    //float doff = -test_TIME * 0.07;
    float2 p = float2(mirror(t, 0.1), d + doff);
    float f1 = noise(p * SCALE);
    p = 2.1 * float2(mirror(t, 0.4), d + doff);
    float f2 = noise(p * SCALE);
    p = 3.7 * float2(mirror(t, 0.8), d + doff);
    float f3 = noise(p * SCALE);
    p = 5.8 * float2(mirror(t, 0.0), d + doff);
    float f4 = noise(p * SCALE);
    return pow((f1 + 0.5 * f2 + 0.25 * f3 + 0.125 * f4) * 3., 1.);
}
float3 colorize(float x)
{
    x = clamp(x, 0, 1.0);
    float3 c = lerp(float3(0, 0, 1.1), float3(0, 1, 1), x);
    c = lerp(c, float3(1, 1, 1), x * 4. - 3.0) * x;
    c = max(c, 0);
    c = lerp(c, float3(1.0, .25, 1.0), smoothstep(1.0, 0.2, x) * smoothstep(0.15, 0.6, x));
    return c;
}

float Update(float2 uv)
{
    float d = dot(uv, uv);
    float t = atan2(uv.y, uv.x) / 6.28318530718;
    float v = radialNoise(t, d);
    return v;
}

float4 main(VS_OUT pin) : SV_TARGET
{
    float v;
    float angle = timerGlobal * 0.8f;
    
    float U = (pin.texcoord.x * 2.0 - 1.0) + uv_factor * sin(angle) * sin(angle);
    float V = (pin.texcoord.y * 2.0 - 1.0) + uv_factor * sin(angle) * cos(angle);
    float2 uv = float2(U, V);
    
    v = Update(uv);
    
    if (ready)
    {
        float rotate = timerGlobal * 0.8f;
        uv = pin.texcoord.xy * 2.0 - 1.0;
        v = Update(uv);
        
        v = v * 3.0 - 0.6;
        // 明滅させる
        v = lerp(0.0, v, 0.8 * smoothstep(0.0, 0.1, dot(uv, uv))) * lerp(-cos(rotate * 2.5), 0.2, 2) /*明滅*/;
    }
    else
    {
        v = v * 2.5 - 1.4;
        // 明滅させない
        v = lerp(0.0, v, 0.8 * smoothstep(0.0, 0.1, dot(uv, uv)));
    }
    
    float4 color = float4(colorize(v), alpha);
    
// imgui(material_color) の色を反映する
#if 0
    // モノクロにする
    color.rgb = (color.r + color.g + color.b) * 0.3333f;
    // 色を反映
    color = color * pin.color;
#endif
    
    return color;
}

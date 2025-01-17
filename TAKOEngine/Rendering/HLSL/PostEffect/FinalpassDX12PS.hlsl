#include "FilterFunctions.hlsli"
#include "FinalpassDX12.hlsli"

//シーンテクスチャ
Texture2D sceneTexture : register(t0);
SamplerState sceneSampler : register(s0);

// ブルームテクスチャ
Texture2D bloomTexture : register(t1);

static const float width = 1920.0f;
static const float height = 1080.0f;

#define FXAA_REDUCE_MIN (1.0/128.0)
#define FXAA_REDUCE_MUL (1.0/8.0)
#define FXAA_SPAN_MAX 8.0
#define FXAA_SUBPIX_SHIFT (1.0 / 4.0)
float3 fxaa(Texture2D tex, float4 pos_pos, float2 rcp_frame)
{
    float3 sampled_nw = tex.Sample(sceneSampler, pos_pos.zw).xyz;
    float3 sampled_ne = tex.Sample(sceneSampler, pos_pos.zw, int2(1, 0)).xyz;
    float3 sampled_sw = tex.Sample(sceneSampler, pos_pos.zw, int2(0, 1)).xyz;
    float3 sampled_se = tex.Sample(sceneSampler, pos_pos.zw, int2(1, 1)).xyz;
    float3 sampled_m = tex.Sample(sceneSampler, pos_pos.xy).xyz;

    float3 luma = float3(0.299, 0.587, 0.114);
    float luma_nw = dot(sampled_nw, luma);
    float luma_ne = dot(sampled_ne, luma);
    float luma_sw = dot(sampled_sw, luma);
    float luma_se = dot(sampled_se, luma);
    float luma_m = dot(sampled_m, luma);

    float luma_min = min(luma_m, min(min(luma_nw, luma_ne), min(luma_sw, luma_se)));
    float luma_max = max(luma_m, max(max(luma_nw, luma_ne), max(luma_sw, luma_se)));

    float2 dir;
    dir.x = -((luma_nw + luma_ne) - (luma_sw + luma_se));
    dir.y = ((luma_nw + luma_sw) - (luma_ne + luma_se));

    float dir_reduce = max((luma_nw + luma_ne + luma_sw + luma_se) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    float rcp_dir_min = 1.0 / (min(abs(dir.x), abs(dir.y)) + dir_reduce);
    dir = min(float2(FXAA_SPAN_MAX, FXAA_SPAN_MAX), max(float2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcp_dir_min)) * rcp_frame;

    float3 rgb_a = (1.0 / 2.0) * (
		tex.Sample(sceneSampler, pos_pos.xy + dir * (1.0 / 3.0 - 0.5)).xyz +
		tex.Sample(sceneSampler, pos_pos.xy + dir * (2.0 / 3.0 - 0.5)).xyz);
    float3 rgb_b = rgb_a * (1.0 / 2.0) + (1.0 / 4.0) * (
		tex.Sample(sceneSampler, pos_pos.xy + dir * (0.0 / 3.0 - 0.5)).xyz +
		tex.Sample(sceneSampler, pos_pos.xy + dir * (3.0 / 3.0 - 0.5)).xyz);
    float luma_b = dot(rgb_b, luma);

    return ((luma_b < luma_min) || (luma_b > luma_max)) ? rgb_a : rgb_b;
}

float4 main(VS_OUT pin) : SV_TARGET
{
    // FXAA(Fast approximate anti-aliasing)
    float2 rcp_frame = float2(1.0 / width, 1.0 / height);
    float4 pos_pos;
    pos_pos.xy = pin.texcoord.xy;
    pos_pos.zw = pin.texcoord.xy - (rcp_frame * (0.5 + FXAA_SUBPIX_SHIFT));

    float4 color = 1.0;
    color.rgb = fxaa(sceneTexture, pos_pos, rcp_frame);

	// ブルームテクスチャを加算する
    //color.rgb += bloomTexture.Sample(sceneSampler, pin.texcoord).rgb;
    
    // 色調補正処理
	{
		// RGB > HSVに変換
        color.rgb = RGB2HSV(color.rgb);

        // 色相調整
        color.r += hueShift;

        // 彩度調整
        color.g *= saturation;

        // 明度調整
        color.b *= brightness;
        
		// HSV > RGBに変換
        color.rgb = HSV2RGB(color.rgb);
    }

    return color;
}
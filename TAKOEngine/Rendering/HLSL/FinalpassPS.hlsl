#include "Finalpass.hlsli"

// シーンテクスチャ
Texture2D sceneTexture : register(t0);
SamplerState sceneSampler : register(s0);

// ブルームテクスチャ
Texture2D bloomTexture : register(t1);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = sceneTexture.Sample(sceneSampler, pin.texcoord) * pin.color;

	// ブルームテクスチャを加算する
    color.rgb += bloomTexture.Sample(sceneSampler, pin.texcoord).rgb;
    
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

#include "Portal.hlsli"

Texture2D spriteTexture : register(t0);
SamplerState spriteSampler : register(s0);

// ピクセルシェーダーエントリポイント
float4 main(VS_OUT pin) : SV_TARGET
{
    float t = timerGlobal * 0.2;
    //float3 iResolution = _PORTAL_iResolution; // 描画範囲
    
    //float2 uv = (ps_in.position.xy - 0.5 * iResolution.xy) / iResolution.y;  // スクリーンの位置を計算して出力
    float2 uv = pin.texcoord.xy - float2(0.5, 0.5); // オブジェクトの位置を計算して出力
    
    float2 st = float2(length(uv) * 2.0, atan2(uv.x, uv.y));
    
    st.y += st.x * 1.1;
        
    float x = fbm3d(float3((st.y), cos(st.y), pow(st.x, 0.3) + t * 0.1), 3);
    float y = fbm3d(float3(cos(1.0 - st.y), sin(1.0 - st.y), pow(st.x, 0.5) + t * 0.1), 4);
    float r = fbm3d(float3(x, y, st.x + t * 0.3), 5);
    
    r = fbm3d(float3(r - x, r - y, r + t * 0.3), 6);
    
    float c = (r + st.x * 5.0) / 6.0;
    
    float4 color = float4(smoothstep(0.3, 0.4, c), smoothstep(0.4, 0.55, c), smoothstep(0.2, 0.55, c), 1.0);
    if (color.r <= 0.5 && color.g <= 0.5 && color.b <= 0.5)
        discard; //黒色は無視する 
    if (color.r >= 0.7 && color.g >= 0.7 && color.b >= 0.7)
        discard; //白色は無視する 
    return color;
}
#include "Billboard.hlsli"

Texture2D spriteTexture : register(t0);
SamplerState spriteSampler : register(s0);

// ピクセルシェーダーエントリポイント
float4 main(PSInput pin) : SV_TARGET
{
    return spriteTexture.Sample(spriteSampler, pin.texcoord) * pin.color;
}
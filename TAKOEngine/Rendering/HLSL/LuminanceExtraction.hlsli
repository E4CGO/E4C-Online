#include "FilterFunctions.hlsli"
#include "Sprite.hlsli"

cbuffer CBLuminanceExtraction : register(b0)
{
    float threshold; // 高輝度抽出のための閾値
    float intensity; // ブルームの強度
    float2 dummy;
};

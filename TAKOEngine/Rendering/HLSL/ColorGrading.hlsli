#include "FilterFunctions.hlsli"
#include "Sprite.hlsli"

//	スプライト描画単位で指定するパラメーター
cbuffer CBColorGrading : register(b0)
{
    float hueShift; // 色相調整
    float saturation; // 彩度調整
    float brightness; // 明度調整
    float dummy;
};

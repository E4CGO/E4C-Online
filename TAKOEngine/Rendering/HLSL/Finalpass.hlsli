#include "FilterFunctions.hlsli"
#include "Sprite.hlsli"

cbuffer CBFinalpass : register(b0)
{
	//	色調補正
    float hueShift; // 色相調整
    float saturation; // 彩度調整
    float brightness; // 明度調整
    float dummy;
};

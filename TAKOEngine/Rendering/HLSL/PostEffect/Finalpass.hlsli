#include "FilterFunctions.hlsli"
#include "../Sprite/Sprite.hlsli"

cbuffer CBFinalpass : register(b0)
{
	//	�F���␳
    float hueShift; // �F������
    float saturation; // �ʓx����
    float brightness; // ���x����
    float dummy;
};

#pragma once
#include <DirectXMath.h>

class Mathf
{
public:
	// 線形補完
	static float Lerp(float a, float b, float t);
	// 指定範囲のランダム値を計算する
	static float RandomRange(float min, float max);
	// 誤差比較
	static bool cmpf(float a, float b, float epsilon = 0.005f);
	static bool cmpfloat3(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, float epsilon = 0.005f);
};
﻿#include "Mathf.h"
#include <stdlib.h>
#include <math.h>
#include "XMFLOAT.h"

float Mathf::Lerp(float a, float b, float t)
{
	return a * (1.0f - t) + (b * t);
}
DirectX::XMFLOAT3 Mathf::Lerp(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, float t)
{
	return {
		Lerp(a.x, b.x, t),
		Lerp(a.y, b.y, t),
		Lerp(a.z, b.z, t),
	};
}
float Mathf::LerpRadian(float a, float b, float t)
{
	float difference = b - a;

	// 差を -π から π の範囲に正規化
	if (difference > DirectX::XM_PI) {
		difference -= 2 * DirectX::XM_PI;
	}
	else if (difference < -DirectX::XM_PI) {
		difference += 2 * DirectX::XM_PI;
	}

	// 線形補間
	return a + t * difference;
}

float Mathf::RandomRange(float max, float min)
{
	return min + static_cast<float>(rand() * (max - min) / RAND_MAX);
}

float Mathf::CalcDistFloat3XZ(DirectX::XMFLOAT3 originPosition, DirectX::XMFLOAT3 targetPosition)
{
	float vx = targetPosition.x - originPosition.x;
	float vz = targetPosition.z - originPosition.z;
	return sqrtf(vx * vx + vz * vz);
}

float Mathf::CalcDistFloat3(DirectX::XMFLOAT3 originPosition, DirectX::XMFLOAT3 targetPosition)
{
	float vx = targetPosition.x - originPosition.x;
	float vy = targetPosition.y - originPosition.y;
	float vz = targetPosition.z - originPosition.z;
	return sqrtf(vx * vx + vy * vy + vz * vz);
}

// 誤差比較
bool Mathf::cmpf(float a, float b, float epsilon)
{
	return fabs(a - b) < epsilon;
}

bool Mathf::cmpfloat3(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, float epsilon)
{
	return XMFLOAT3Length(b - a) < epsilon;
}
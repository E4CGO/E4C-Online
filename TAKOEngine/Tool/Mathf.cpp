#include "Mathf.h"
#include <stdlib.h>
#include <math.h>
#include "XMFLOAT.h"

float Mathf::Lerp(float a, float b, float t)
{
	return a * (1.0f - t) + (b * t);
}

float Mathf::RandomRange(float max, float min)
{
	return min + static_cast<float>(rand() * (max - min) / RAND_MAX);
}

// åÎç∑î‰är
bool Mathf::cmpf(float a, float b, float epsilon)
{
	return fabs(a - b) < epsilon;
}

bool Mathf::cmpfloat3(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, float epsilon)
{
	return XMFLOAT3Length(b - a) < epsilon;
}
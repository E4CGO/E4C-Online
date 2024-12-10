//! @file Mathf.h
//! @note

#ifndef __INCLUDED_TOOL_MATH__
#define __INCLUDED_TOOL_MATH__

#include <DirectXMath.h>

/**************************************************************************//**
	@class	Mathf
	@brief	Float数学計算用
	@par	[説明]
*//***************************************************************************/
class Mathf
{
public:
	// 線形補完
	static float Lerp(float a, float b, float t);
	static DirectX::XMFLOAT3 Lerp(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, float t);
	static float LerpRadian(float a, float b, float t);
	// 指定範囲のランダム値を計算する
	static float RandomRange(float min, float max);
	static float CalcDistFloat3XZ(DirectX::XMFLOAT3 originPosition, DirectX::XMFLOAT3 targetPosition);
	static float CalcDistFloat3(DirectX::XMFLOAT3 originPosition, DirectX::XMFLOAT3 targetPosition);
	// 誤差比較
	static bool cmpf(float a, float b, float epsilon = 0.005f);
	static bool cmpfloat3(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, float epsilon = 0.005f);
};

#endif // !__INCLUDED_TOOL_MATH__
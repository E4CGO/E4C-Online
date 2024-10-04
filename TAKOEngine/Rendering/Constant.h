#pragma once

#include <d3d12.h>

// バッファカウント
static const UINT BufferCount = 2;

static const UINT SkinningCSThreadNum = 8;

// 低速 n 切り上げ
template<class T>
inline T RoundUp(const T& n, const T& v)
{
	return n * (((v)+n) / n);
}

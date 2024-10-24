//! @file Constant.h
//! @note

#ifndef __GRAPHICS_CONSTANT_H__
#define __GRAPHICS_CONSTANT_H__

#include <d3d12.h>

// バッファカウント
static const UINT BufferCount = 2;

static const UINT SkinningCSThreadNum = 8;

//インスタンシング最大数
static constexpr int InstancingMax = 512;

// 低速 n 切り上げ
template<class T>
inline T RoundUp(const T& n, const T& v)
{
	return n * (((v)+n) / n);
}

#endif // !__GRAPHICS_CONSTANT_H__
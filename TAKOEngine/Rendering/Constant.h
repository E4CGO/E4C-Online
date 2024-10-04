#pragma once

#include <d3d12.h>

// �o�b�t�@�J�E���g
static const UINT BufferCount = 2;

static const UINT SkinningCSThreadNum = 8;

// �ᑬ n �؂�グ
template<class T>
inline T RoundUp(const T& n, const T& v)
{
	return n * (((v)+n) / n);
}

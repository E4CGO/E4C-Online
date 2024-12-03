//! @file FrameBufferTexture.cpp
//! @note

#include "FrameBufferTexture.h"

//******************************************************************
// @class     初期化
// @param[in] texture　ID3D12Resource*
// @return    なし
//******************************************************************
void FrameBufferTexture::InitFromD3DResource(ID3D12Resource* texture)
{
	m_texture = texture;
	m_textureDesc = m_texture->GetDesc();
}
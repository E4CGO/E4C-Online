//! @file FrameBufferTexture.cpp
//! @note

#include "FrameBufferTexture.h"

void FrameBufferTexture::InitFromD3DResource(ID3D12Resource* texture)
{
	m_texture = texture;
	m_textureDesc = m_texture->GetDesc();
}
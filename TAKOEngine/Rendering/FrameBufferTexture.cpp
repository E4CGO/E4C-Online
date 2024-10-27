//! @file FrameBufferTexture.cpp
//! @note

#include "Misc.h"
#include "FrameBufferTexture.h"

FrameBufferTexture::~FrameBufferTexture()
{
	if (m_texture) m_texture->Release();
}

void FrameBufferTexture::InitFromD3DResource(ID3D12Resource* texture)
{
	if (m_texture) m_texture->Release();

	m_texture = texture;
	m_texture->AddRef();
	m_textureDesc = m_texture->GetDesc();
}
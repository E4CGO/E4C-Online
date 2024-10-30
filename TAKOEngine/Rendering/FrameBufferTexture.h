//! @file FrameBufferTexture.h
//! @note 

#ifndef __GRAHICS_FRAME_BUFFER_TEXTURE_H__
#define __GRAHICS_FRAME_BUFFER_TEXTURE_H__

#include <wrl.h>
#include <d3d12.h>

class FrameBufferTexture
{
public:
	FrameBufferTexture() = default;
	~FrameBufferTexture() = default;

	void InitFromD3DResource(ID3D12Resource* texture);

	ID3D12Resource* Get() const
	{
		return m_texture.Get();
	}

	DXGI_FORMAT GetFormat() const
	{
		return m_textureDesc.Format;
	}

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_texture = nullptr;
	D3D12_RESOURCE_DESC m_textureDesc = {};        
};

#endif // !__GRAHICS_FRAME_BUFFER_TEXTURE_H__

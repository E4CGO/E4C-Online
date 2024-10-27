//! @file FrameBufferTexture.h
//! @note 

#ifndef __GRAHICS_FRAME_BUFFER_TEXTURE_H__
#define __GRAHICS_FRAME_BUFFER_TEXTURE_H__

#include <wrl.h>
#include <d3d12.h>

class FrameBufferTexture
{
public:
	FrameBufferTexture() {};
	~FrameBufferTexture();

	void InitFromD3DResource(ID3D12Resource* texture);

	// テクスチャのリソースを取得
	ID3D12Resource* Get()
	{
		return m_texture;
	}

	// テクスチャのフォーマットを取得
	DXGI_FORMAT GetFormat() const
	{
		return m_textureDesc.Format;
	}

private:
	ID3D12Resource*     m_texture = nullptr;  //テクスチャ
	D3D12_RESOURCE_DESC m_textureDesc = {};        //テクスチャ情報    
};

#endif // !__GRAHICS_FRAME_BUFFER_TEXTURE_H__
//! @file FrameBufferDX12.h
//! @note

#ifndef __GRAHICS_FRAME_BUFFERDX12_H__
#define __GRAHICS_FRAME_BUFFERDX12_H__

#include <dxgi1_6.h>
#include "Descriptor.h"
#include "FrameBufferTexture.h"

//*******************************************************
// @class FrameBufferDX12
// @brief レンダータゲット作成
// @par   
//*******************************************************
class FrameBufferDX12
{
public:
	FrameBufferDX12(ID3D12Device* device, const wchar_t* resourceName[2], UINT wight, UINT height, DXGI_FORMAT rtv_format = DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT dsv_format = DXGI_FORMAT_D32_FLOAT);
	~FrameBufferDX12();

	// デプスステンシルバッファが存在しているか判定
	bool IsExsitDepthStencilBuffer() const
	{
		return d3d_dsv_resource;
	}

	// レンダーターゲットのクリアカラー
	const float* GetRTVClearColor() const
	{
		return m_rtvClearColor;
	}
	
	// 深度ステンシルのクリアカラー
	float GetDSVClearValue() const
	{
		return m_dsvClearValue;
	}

	// レンダーターゲットとなるテクスチャを取得
	FrameBufferTexture* GetRenderTargetTexture()
	{
		return m_renderTargetTexture.get();
	}

	// レンダーターゲットのフォーマットを取得
	DXGI_FORMAT GetRTVBufferFormat() const { return m_renderTargetTexture->GetFormat(); }

	//テクスチャ幅取得
	inline float GetWidth() const { return m_width; }

	//テクスチャの高さ取得
	inline float GetHeight() const { return m_height; }

	// レンダーターゲットのディスクリプタ取得
	const Descriptor* GetRTVDescriptor() { return rtv_descriptor; }

	// 深度ステンシルバッファのディスクリプタ取得
	const Descriptor* GetDSVDescriptor() { return dsv_descriptor; }

private:
	// レンダーターゲットとなるテクスチャを作成する
	void CreateRenderTargetTexture(ID3D12Device* device, const wchar_t* resourceName, UINT wight, UINT height, DXGI_FORMAT format);

	// 深度ステンシルバッファとなるテクスチャを作成
	void CreateDepthStencilTexture(ID3D12Device* device, const wchar_t* resourceName, UINT wight, UINT height, DXGI_FORMAT format);

	// ディスクリプタヒープを取得
	void CreateDescriptorHeap();

	// ディスクリプタを作成する
	void CreateDescriptor(ID3D12Device* device);

private:
	std::unique_ptr<FrameBufferTexture> m_renderTargetTexture;
	std::unique_ptr<FrameBufferTexture> m_depthStencilTexture;
	Microsoft::WRL::ComPtr<ID3D12Resource> d3d_rtv_resource;
	Microsoft::WRL::ComPtr<ID3D12Resource> d3d_dsv_resource;
	const Descriptor* rtv_descriptor = nullptr;
	const Descriptor* dsv_descriptor = nullptr;

	float m_width  = 0;	//レンダリングターゲットの幅
	float m_height = 0;	//レンダリングターゲットの高さ
	float m_rtvClearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };	//レンダリングターゲットビューのクリアカラー
	float m_dsvClearValue = 1.0f;							//DSVのクリアカラー
};

#endif // !__GRAHICS_FRAME_BUFFERDX12_H__
//! @file FrameBuffer.h
//! @note

#ifndef __GRAHICS_FRAME_BUFFER_H__
#define __GRAHICS_FRAME_BUFFER_H__

#include <wrl.h>
#include <d3d11.h>

//*******************************************************
// @class FrameBuffer
// @brief レンダータゲット作成&取得
// @par   
//*******************************************************
class FrameBuffer
{
public:
	FrameBuffer(ID3D11Device* device, IDXGISwapChain* swapchain, DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT);
	FrameBuffer(ID3D11Device* device, UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

	// カラーマップ取得
	ID3D11ShaderResourceView* GetColorMap() const { return colorMap.Get(); }
	const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetShaderResourceView() const { return colorMap; }

	// クリア
	void Clear(ID3D11DeviceContext* dc, float r, float g, float b, float a);

	//レンダーターゲット設定	
	void SetRenderTarget(ID3D11DeviceContext* dc);

	//G-Buffer用レンダーターゲット設定	
	void SetRenderTargets(ID3D11DeviceContext* dc, int number, ID3D11RenderTargetView* rtv[]);

	//ビューポート設定
	void SetViewport(ID3D11DeviceContext* dc, UINT width, UINT height);

	// レンダーターゲットビュー取得
	ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }

	// デプスステンシルビュー取得
	ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }

	//ビューポート取得
	const D3D11_VIEWPORT& GetViewport() { return viewport; }

	//テクスチャ幅取得
	inline int GetWidth() const { return texture2dDesc.Width; }

	//テクスチャの高さ取得
	inline int GetHeight() const { return texture2dDesc.Height; }

	enum class DeferredRTV
	{
		Normal,    
		Position,
	};

private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	D3D11_VIEWPORT viewport;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> colorMap;
	D3D11_TEXTURE2D_DESC texture2dDesc;
};

#endif // !__GRAHICS_FRAME_BUFFER_H__
#pragma once

#include <wrl.h>
#include <d3d11.h>

class FrameBuffer
{
public:
	FrameBuffer(ID3D11Device* device, IDXGISwapChain* swapchain);
	FrameBuffer(ID3D11Device* device, UINT width, UINT height);

	// カラーマップ取得
	ID3D11ShaderResourceView* GetColorMap() const { return colorMap.Get(); }
	const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetShaderResourceView() const { return colorMap; }

	// クリア
	void Clear(ID3D11DeviceContext* dc, float r, float g, float b, float a);

	void SetRenderTargets(ID3D11DeviceContext* dc);

	// レンダーターゲットビュー取得
	ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }

	// デプスステンシルビュー取得
	ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }

	const D3D11_VIEWPORT& GetViewport() { return viewport; }
private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	D3D11_VIEWPORT viewport;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> colorMap;
	D3D11_TEXTURE2D_DESC texture2dDesc;
};

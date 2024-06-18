#include "Misc.h"
#include "FrameBuffer.h"

// コンストラクタ
FrameBuffer::FrameBuffer(ID3D11Device* device, IDXGISwapChain* swapchain)
{
	HRESULT hr = S_OK;
	UINT width, height;

	// レンダーテーゲットビューの生成
	{
		// スワップチェーンからバックバッファテクスチャを取得する。
		// ※スワップチェーンに内包されているバックバッファテクスチャは'色'を書き込むテクスチャ
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		hr = swapchain->GetBuffer(
			0,
			__uuidof(ID3D11Texture2D),
			reinterpret_cast<void**>(texture2d.GetAddressOf())
		);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// バックバッファテクスチャへの書き込みの窓口となるレンダーターゲットビューを生成する。
		hr = device->CreateRenderTargetView(texture2d.Get(), nullptr, renderTargetView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// バックバッファテクスチャからサイズ情報を取得
		D3D11_TEXTURE2D_DESC texture2dDesc;
		texture2d->GetDesc(&texture2dDesc);

		width = texture2dDesc.Width;
		height = texture2dDesc.Height;
	}

	// 深度ステンシルビューの作成
	{
		// 深度ステンシル情報を書き込むためのテクスチャを作成する
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		texture2dDesc = {};
		texture2dDesc.Width = width;
		texture2dDesc.Height = height;
		texture2dDesc.MipLevels = 1;
		texture2dDesc.ArraySize = 1;
		texture2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texture2dDesc.SampleDesc.Count = 1;
		texture2dDesc.SampleDesc.Quality = 0;
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texture2dDesc.CPUAccessFlags = 0;
		texture2dDesc.MiscFlags = 0;
		hr = device->CreateTexture2D(&texture2dDesc, nullptr, texture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// 深度ステンシルテクスチャへの書き込むに窓口になる深度ステンシルビューを作成する
		hr = device->CreateDepthStencilView(texture2d.Get(), nullptr, depthStencilView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// ビューポート
	{
		viewport.Width = static_cast<float>(width);
		viewport.Height = static_cast<float>(height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
	}
}

// コンストラクタ
FrameBuffer::FrameBuffer(ID3D11Device* device, UINT width, UINT height)
{
	HRESULT hr = S_OK;
	// レンダーターゲット
	{
		// テクスチャ生成
		Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetBuffer;
		D3D11_TEXTURE2D_DESC texture2dDesc{};
		texture2dDesc.Width = width;
		texture2dDesc.Height = height;
		texture2dDesc.MipLevels = 1;
		texture2dDesc.ArraySize = 1;
		texture2dDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//texture2dDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		texture2dDesc.SampleDesc.Count = 1;
		texture2dDesc.SampleDesc.Quality = 0;
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		texture2dDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texture2dDesc.CPUAccessFlags = 0;
		texture2dDesc.MiscFlags = 0;
		hr = device->CreateTexture2D(&texture2dDesc, 0, renderTargetBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// レンダーターゲットビュー生成
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetviewDesc{};
		renderTargetviewDesc.Format = texture2dDesc.Format;
		renderTargetviewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		hr = device->CreateRenderTargetView(renderTargetBuffer.Get(), &renderTargetviewDesc, renderTargetView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// シェーダーリソースビュー生成
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
		shaderResourceViewDesc.Format = texture2dDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(renderTargetBuffer.Get(), &shaderResourceViewDesc, colorMap.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// デプスステンシル
	{
		// テクスチャ生成
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
		D3D11_TEXTURE2D_DESC texture2dDesc{};
		texture2dDesc.Width = width;
		texture2dDesc.Height = height;
		texture2dDesc.MipLevels = 1;
		texture2dDesc.ArraySize = 1;
		texture2dDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		texture2dDesc.SampleDesc.Count = 1;
		texture2dDesc.SampleDesc.Quality = 0;
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texture2dDesc.CPUAccessFlags = 0;
		texture2dDesc.MiscFlags = 0;
		hr = device->CreateTexture2D(&texture2dDesc, 0, depthStencilBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// デプスステンシルビュー生成
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{ };
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Flags = 0;
		hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	// ビューポート
	{
		viewport.Width = static_cast<float>(width);
		viewport.Height = static_cast<float>(height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
	}
}

void FrameBuffer::Clear(ID3D11DeviceContext* dc, float r, float g, float b, float a)
{
	float color[4]{ r,g,b,a };
	dc->ClearRenderTargetView(renderTargetView.Get(), color);
	dc->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void FrameBuffer::SetRenderTargets(ID3D11DeviceContext* dc)
{
	dc->RSSetViewports(1, &viewport);
	dc->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
}
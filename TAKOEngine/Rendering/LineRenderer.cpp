#include <stdio.h>
#include <memory>
#include "Misc.h"
#include "LineRenderer.h"
#include "GpuResourceUtils.h"

LineRenderer::LineRenderer(ID3D11Device* device, UINT vertexCount)
	: capacity(vertexCount)
{
	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	// 頂点シェーダー
	GpuResourceUtils::LoadVertexShader(
		device,
		"Data/Shader/LineVS.cso",
		inputElementDesc,
		_countof(inputElementDesc),
		inputLayout.GetAddressOf(),
		vertexShader.GetAddressOf()
	);

	// ピクセルシェーダー
	GpuResourceUtils::LoadPixelShader(
		device,
		"Data/Shader/LinePS.cso",
		pixelShader.GetAddressOf()
	);

	// 定数バッファ
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(ConstantBuffer),
		constantBuffer.GetAddressOf()
	);

	// ブレンドステート
	{
		D3D11_BLEND_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = false;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = device->CreateBlendState(&desc, blendState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// 深度ステンシルステート
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		HRESULT hr = device->CreateDepthStencilState(&desc, depthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// ラスタライザーステート
	{
		D3D11_RASTERIZER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.FrontCounterClockwise = true;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = true;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.AntialiasedLineEnable = false;

		HRESULT hr = device->CreateRasterizerState(&desc, rasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// 頂点バッファ
	{
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = sizeof(Vertex) * vertexCount;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		HRESULT hr = device->CreateBuffer(&desc, nullptr, vertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

// 描画開始
void LineRenderer::Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	// シェーダー設定
	context->VSSetShader(vertexShader.Get(), nullptr, 0);
	context->PSSetShader(pixelShader.Get(), nullptr, 0);
	context->IASetInputLayout(inputLayout.Get());

	// 定数バッファ設定
	context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
	//context->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	// レンダーステート設定
	const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	context->OMSetBlendState(blendState.Get(), blendFactor, 0xFFFFFFFF);
	context->OMSetDepthStencilState(depthStencilState.Get(), 0);
	context->RSSetState(rasterizerState.Get());

	// プリミティブ設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	// 定数バッファ更新
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX VP = V * P;
	ConstantBuffer data;
	DirectX::XMStoreFloat4x4(&data.wvp, VP);
	context->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);

	// 描画
	UINT totalVertexCount = static_cast<UINT>(vertices.size());
	UINT start = 0;
	UINT count = (totalVertexCount < capacity) ? totalVertexCount : capacity;

	while (start < totalVertexCount)
	{
		D3D11_MAPPED_SUBRESOURCE mappedVB;
		HRESULT hr = context->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVB);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		memcpy(mappedVB.pData, &vertices[start], sizeof(Vertex) * count);

		context->Unmap(vertexBuffer.Get(), 0);

		context->Draw(count, 0);

		start += count;
		if ((start + count) > totalVertexCount)
		{
			count = totalVertexCount - start;
		}
	}
	vertices.clear();
}

// 頂点追加
void LineRenderer::AddVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color)
{
	Vertex v;
	v.position = position;
	v.color = color;
	vertices.emplace_back(v);
}
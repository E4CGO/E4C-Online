//! @file PrimitiveRender.cpp
//! @note 

#include "PrimitiveRender.h"
#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/GpuResourceUtils.h"

/**************************************************************************//**
    @brief    コンストラク
    @param[in]    device
*//***************************************************************************/
PrimitiveRender::PrimitiveRender(ID3D11Device* device):SpriteShader(device, "Data/Shader/PrimitiveRenderVS.cso", "Data/Shader/PrimitiveRenderPS.cso")
{
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"TEXCOORD",  0,DXGI_FORMAT_R32G32_FLOAT,        0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	// 定数バッファ
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(CbScene),
		constantBuffer.GetAddressOf());

	// 頂点バッファ
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = sizeof(Vertex) * VertexCapacity;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	HRESULT hr = device->CreateBuffer(&desc, nullptr, vertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	//GpuResourceUtils::LoadTexture(device, "Data/Sprite/trail.png", texture.ReleaseAndGetAddressOf(), &textureDesc);
}
/**************************************************************************//**
     @brief    頂点追加
    @param[in]    position
    @param[in]    color
    @param[in]    texcoord
*//***************************************************************************/
void PrimitiveRender::AddVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT2& texcoord)
{
	Vertex& v = vertices.emplace_back();
	v.position = position;
	v.color = color;
	v.texcoord = texcoord;
}
/**************************************************************************//**
     @brief    描画実行
    @param[in]    dc
    @param[in]    view
    @param[in]    projection
    @param[in]    primitiveTopology
*//***************************************************************************/
void PrimitiveRender::Render(
	ID3D11DeviceContext* dc,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection,
	D3D11_PRIMITIVE_TOPOLOGY primitiveTopology)
{
	// シェーダー設定
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(), nullptr, 0);
	dc->PSSetShaderResources(0, 1, texture.GetAddressOf());
	dc->IASetInputLayout(inputLayout.Get());
	// 定数バッファ設定
	dc->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());




	// 頂点バッファ設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	dc->IASetPrimitiveTopology(primitiveTopology);
	dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	// 定数バッファ更新
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX VP = V * P;
	CbScene cbScene;
	DirectX::XMStoreFloat4x4(&cbScene.viewProjection, VP);
	dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &cbScene, 0, 0);

	// 描画
	UINT totalVertexCount = static_cast<UINT>(vertices.size());
	UINT start = 0;
	UINT count = (totalVertexCount < VertexCapacity) ? totalVertexCount : VertexCapacity;

	while (start < totalVertexCount)
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		HRESULT hr = dc->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		memcpy(mappedSubresource.pData, &vertices[start], sizeof(Vertex) * count);

		dc->Unmap(vertexBuffer.Get(), 0);

		dc->Draw(count, 0);

		start += count;
		if ((start + count) > totalVertexCount)
		{
			count = totalVertexCount - start;
		}
	}

	vertices.clear();
}

//! @file SpriteShader.cpp
//! @note 

#include "TAKOEngine/Rendering/GpuResourceUtils.h"
#include "SpriteShader.h"

//**********************************************************************
// @brief     コンストラクタ
// @param[in] device  ID3D11Device*
// @param[in] vs      VertexShaderの名前
// @param[in] ps      PixelShaderの名前
// @return    なし
//**********************************************************************
SpriteShader::SpriteShader(ID3D11Device* device, const char* vs, const char* ps)
{
	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// 頂点シェーダー
	GpuResourceUtils::LoadVertexShader(
		device,
		vs,
		inputElementDesc,
		_countof(inputElementDesc),
		inputLayout.GetAddressOf(),
		vertexShader.GetAddressOf()
	);

	// ピクセルシェーダー
	GpuResourceUtils::LoadPixelShader(
		device,
		ps,
		pixelShader.GetAddressOf()
	);
}

//**********************************************************************
// @brief     描画開始
// @param[in] rc  レンダーコンストラクタ
// @return    なし
//**********************************************************************
void SpriteShader::Begin(const RenderContext& rc)
{
	rc.deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	rc.deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);
	rc.deviceContext->IASetInputLayout(inputLayout.Get());

	rc.deviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	rc.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D11Buffer* constantBuffers[] =
	{
		constantBuffer.Get(),
	};
	rc.deviceContext->VSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
	rc.deviceContext->PSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);

	const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	rc.deviceContext->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Transparency), blend_factor, 0xFFFFFFFF);
	rc.deviceContext->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	rc.deviceContext->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullBack));

	// サンプラステート
	ID3D11SamplerState* samplerStates[] =
	{
		rc.renderState->GetSamplerState(SamplerState::LinearClamp)
	};
	rc.deviceContext->PSSetSamplers(0, _countof(samplerStates), samplerStates);
}

//**********************************************************************
// @brief     描画終了
// @param[in] rc  レンダーコンストラクタ
// @return    なし
//**********************************************************************
void SpriteShader::End(const RenderContext& rc)
{
}

//**********************************************************************
// @brief     描画
// @param[in] rc  レンダーコンストラクタ
// @param[in] sprite   描画対象のスプライトデータを指すポインタ
// @return    なし
//**********************************************************************
void SpriteShader::Draw(const RenderContext& rc, const Sprite* sprite)
{
	UpdateConstantBuffer(rc);

	UINT stride = sizeof(Sprite::Vertex);
	UINT offset = 0;

	rc.deviceContext->IASetVertexBuffers(0, 1, sprite->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	rc.deviceContext->PSSetShaderResources(0, 1, sprite->GetShaderResourceView().GetAddressOf());
	rc.deviceContext->Draw(4, 0);
}
//! @file RenderState.cpp
//! @note

#include "Misc.h"
#include "RenderState.h"
#include "Graphics.h"

//***********************************************************
// @brief       コンストラクタ
// @param[in]   device   ID3D11Device*
// @return      なし
//***********************************************************
RenderState::RenderState(ID3D11Device* device)
{
	// SamplerState
	{
		// ポイントサンプリング&テクスチャ繰り返しあり
		{
			D3D11_SAMPLER_DESC desc;
			desc.MipLODBias = 0.0f;
			desc.MaxAnisotropy = 1;
			desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			desc.MinLOD = -D3D11_FLOAT32_MAX;
			desc.MaxLOD = D3D11_FLOAT32_MAX;
			desc.BorderColor[0] = 1.0f;
			desc.BorderColor[1] = 1.0f;
			desc.BorderColor[2] = 1.0f;
			desc.BorderColor[3] = 1.0f;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

			HRESULT hr = device->CreateSamplerState(&desc, samplerStates[static_cast<int>(SamplerState::PointWrap)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
		// ポイントサンプリング&テクスチャ繰り返しなし
		{
			D3D11_SAMPLER_DESC desc;
			desc.MipLODBias = 0.0f;
			desc.MaxAnisotropy = 1;
			desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			desc.MinLOD = -D3D11_FLOAT32_MAX;
			desc.MaxLOD = D3D11_FLOAT32_MAX;
			desc.BorderColor[0] = 1.0f;
			desc.BorderColor[1] = 1.0f;
			desc.BorderColor[2] = 1.0f;
			desc.BorderColor[3] = 1.0f;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

			HRESULT hr = device->CreateSamplerState(&desc, samplerStates[static_cast<int>(SamplerState::PointClamp)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
		// リニアサンプリング&テクスチャ繰り返しあり
		{
			D3D11_SAMPLER_DESC desc;
			desc.MipLODBias = 0.0f;
			desc.MaxAnisotropy = D3D11_DEFAULT_MAX_ANISOTROPY;
			desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			desc.MinLOD = -D3D11_FLOAT32_MAX;
			desc.MaxLOD = D3D11_FLOAT32_MAX;
			desc.BorderColor[0] = 1.0f;
			desc.BorderColor[1] = 1.0f;
			desc.BorderColor[2] = 1.0f;
			desc.BorderColor[3] = 1.0f;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

			HRESULT hr = device->CreateSamplerState(&desc, samplerStates[static_cast<int>(SamplerState::LinearWrap)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}

		// リニアサンプリング&テクスチャ繰り返しなし
		{
			D3D11_SAMPLER_DESC desc;
			desc.MipLODBias = 0.0f;
			desc.MaxAnisotropy = 1;
			desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			desc.MinLOD = -D3D11_FLOAT32_MAX;
			desc.MaxLOD = D3D11_FLOAT32_MAX;
			desc.BorderColor[0] = 1.0f;
			desc.BorderColor[1] = 1.0f;
			desc.BorderColor[2] = 1.0f;
			desc.BorderColor[3] = 1.0f;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

			HRESULT hr = device->CreateSamplerState(&desc, samplerStates[static_cast<int>(SamplerState::LinearClamp)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}

		// リニアサンプリング&テクスチャ ボーダー
		{
			D3D11_SAMPLER_DESC desc;
			desc.MipLODBias = 0.0f;
			desc.MaxAnisotropy = 1;
			desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			desc.MinLOD = -D3D11_FLOAT32_MAX;
			desc.MaxLOD = D3D11_FLOAT32_MAX;
			desc.BorderColor[0] = 0.0f;
			desc.BorderColor[1] = 1.0f;
			desc.BorderColor[2] = 0.0f;
			desc.BorderColor[3] = 0.0f;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

			HRESULT hr = device->CreateSamplerState(&desc, samplerStates[static_cast<int>(SamplerState::LinearBorder)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}

		// リニアサンプリングミラーリング&テクスチャ ボーダー
		{
			D3D11_SAMPLER_DESC desc;
			desc.MipLODBias = 0.0f;
			desc.MaxAnisotropy = 1;
			desc.ComparisonFunc = D3D11_COMPARISON_LESS;
			desc.MinLOD = -D3D11_FLOAT32_MAX;
			desc.MaxLOD = D3D11_FLOAT32_MAX;
			desc.BorderColor[0] = 0.0f;
			desc.BorderColor[1] = 0.0f;
			desc.BorderColor[2] = 0.0f;
			desc.BorderColor[3] = 0.0f;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

			HRESULT hr = device->CreateSamplerState(&desc, samplerStates[static_cast<int>(SamplerState::LinearMirror)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}

		// リニアサンプリング&テクスチャ ボーダー
		{
			D3D11_SAMPLER_DESC desc;
			desc.MipLODBias = 0.0f;
			desc.MaxAnisotropy = 8;
			desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			desc.MinLOD = -D3D11_FLOAT32_MAX;
			desc.MaxLOD = D3D11_FLOAT32_MAX;
			desc.BorderColor[0] = 0.0f;
			desc.BorderColor[1] = 1.0f;
			desc.BorderColor[2] = 0.0f;
			desc.BorderColor[3] = 0.0f;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.Filter = D3D11_FILTER_ANISOTROPIC;

			HRESULT hr = device->CreateSamplerState(&desc, samplerStates[static_cast<int>(SamplerState::AnisotropicWrap)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}

		//シャドウマップ用サンプラーステート
		{
			D3D11_SAMPLER_DESC desc;
			desc.MipLODBias = 0.0f;
			desc.MaxAnisotropy = 1;
			desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			desc.MinLOD = -D3D11_FLOAT32_MAX;
			desc.MaxLOD = D3D11_FLOAT32_MAX;
			desc.BorderColor[0] = D3D11_FLOAT32_MAX;
			desc.BorderColor[1] = D3D11_FLOAT32_MAX;
			desc.BorderColor[2] = D3D11_FLOAT32_MAX;
			desc.BorderColor[3] = D3D11_FLOAT32_MAX;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

			HRESULT hr = device->CreateSamplerState(&desc, samplerStates[static_cast<int>(SamplerState::ShadowMap)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
	}

	// デプスステート
	{
		// 深度テストあり&深度書き込みあり
		{
			D3D11_DEPTH_STENCIL_DESC desc{};
			desc.DepthEnable = true;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			HRESULT hr = device->CreateDepthStencilState(&desc, depthStencilStates[static_cast<int>(DepthState::TestAndWrite)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}

		// 深度テストあり&深度書き込みなし
		{
			D3D11_DEPTH_STENCIL_DESC desc{};
			desc.DepthEnable = true;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			HRESULT hr = device->CreateDepthStencilState(&desc, depthStencilStates[static_cast<int>(DepthState::TestOnly)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}

		// 深度テストなし&深度書き込みあり
		{
			D3D11_DEPTH_STENCIL_DESC desc{};
			desc.DepthEnable = false;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
			HRESULT hr = device->CreateDepthStencilState(&desc, depthStencilStates[static_cast<int>(DepthState::WriteOnly)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
		// 深度テストなし&深度書き込みなし
		{
			D3D11_DEPTH_STENCIL_DESC desc{};
			desc.DepthEnable = false;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
			HRESULT hr = device->CreateDepthStencilState(&desc, depthStencilStates[static_cast<int>(DepthState::NoTestNoWrite)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
	}

	// ブレンドステート
	{
		// 合成なし
		{
			D3D11_BLEND_DESC desc{};
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
			HRESULT hr = device->CreateBlendState(&desc, blendStates[static_cast<int>(BlendState::Opaque)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
		// 通常合成
		{
			D3D11_BLEND_DESC desc{};
			desc.AlphaToCoverageEnable = false;
			desc.IndependentBlendEnable = false;
			desc.RenderTarget[0].BlendEnable = true;
			desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			HRESULT hr = device->CreateBlendState(&desc, blendStates[static_cast<int>(BlendState::Transparency)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
		// 通常合成(OIT)
		{
			D3D11_BLEND_DESC desc{};
			desc.AlphaToCoverageEnable = true;
			desc.IndependentBlendEnable = false;
			desc.RenderTarget[0].BlendEnable = true;
			desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			HRESULT hr = device->CreateBlendState(&desc, blendStates[static_cast<int>(BlendState::OIT)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
		// 加算合成
		{
			D3D11_BLEND_DESC desc{};
			desc.AlphaToCoverageEnable = false;
			desc.IndependentBlendEnable = false;
			desc.RenderTarget[0].BlendEnable = true;
			desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			HRESULT hr = device->CreateBlendState(&desc, blendStates[static_cast<int>(BlendState::Additive)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
		// 減算合成
		{
			D3D11_BLEND_DESC desc{};
			desc.AlphaToCoverageEnable = false;
			desc.IndependentBlendEnable = false;
			desc.RenderTarget[0].BlendEnable = true;
			desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
			desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			HRESULT hr = device->CreateBlendState(&desc, blendStates[static_cast<int>(BlendState::Subtraction)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
		// 乗算合成
		{
			D3D11_BLEND_DESC desc{};
			desc.AlphaToCoverageEnable = false;
			desc.IndependentBlendEnable = false;
			desc.RenderTarget[0].BlendEnable = true;
			desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
			desc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
			desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			HRESULT hr = device->CreateBlendState(&desc, blendStates[static_cast<int>(BlendState::Multiply)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
	}

	// ラスタライザステート
	{
		// ベタ塗り&カリングなし
		{
			D3D11_RASTERIZER_DESC desc{};
			desc.FrontCounterClockwise = false;
			desc.DepthBias = 0;
			desc.DepthBiasClamp = 0;
			desc.SlopeScaledDepthBias = 0;
			desc.DepthClipEnable = true;
			desc.ScissorEnable = false;
			desc.MultisampleEnable = true;
			desc.FillMode = D3D11_FILL_SOLID;
			desc.CullMode = D3D11_CULL_NONE;
			desc.AntialiasedLineEnable = false;
			HRESULT hr = device->CreateRasterizerState(&desc, rasterizerStates[static_cast<int>(RasterizerState::SolidCullNone)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}

		// ベタ塗り&裏面カリング
		{
			D3D11_RASTERIZER_DESC desc{};
			desc.FrontCounterClockwise = false;
			desc.DepthBias = 0;
			desc.DepthBiasClamp = 0;
			desc.SlopeScaledDepthBias = 0;
			desc.DepthClipEnable = true;
			desc.ScissorEnable = false;
			desc.MultisampleEnable = true;
			desc.FillMode = D3D11_FILL_SOLID;
			desc.CullMode = D3D11_CULL_BACK;
			desc.AntialiasedLineEnable = false;
			HRESULT hr = device->CreateRasterizerState(&desc, rasterizerStates[static_cast<int>(RasterizerState::SolidCullBack)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}

		// ベタ塗り&表面カリング
		{
			D3D11_RASTERIZER_DESC desc{};
			desc.FrontCounterClockwise = false;
			desc.DepthBias = 0;
			desc.DepthBiasClamp = 0;
			desc.SlopeScaledDepthBias = 0;
			desc.DepthClipEnable = true;
			desc.ScissorEnable = false;
			desc.MultisampleEnable = true;
			desc.FillMode = D3D11_FILL_SOLID;
			desc.CullMode = D3D11_CULL_FRONT;
			desc.AntialiasedLineEnable = false;
			HRESULT hr = device->CreateRasterizerState(&desc, rasterizerStates[static_cast<int>(RasterizerState::SolidCullFront)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}

		// ワイヤーフレーム&カリングなし
		{
			D3D11_RASTERIZER_DESC desc{};
			desc.FrontCounterClockwise = false;
			desc.DepthBias = 0;
			desc.DepthBiasClamp = 0;
			desc.SlopeScaledDepthBias = 0;
			desc.DepthClipEnable = true;
			desc.ScissorEnable = false;
			desc.MultisampleEnable = true;
			desc.FillMode = D3D11_FILL_WIREFRAME;
			desc.CullMode = D3D11_CULL_NONE;
			desc.AntialiasedLineEnable = false;
			HRESULT hr = device->CreateRasterizerState(&desc, rasterizerStates[static_cast<int>(RasterizerState::WireCullnone)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
		// ワイヤーフレーム&裏面カリング
		{
			D3D11_RASTERIZER_DESC desc{};
			desc.FrontCounterClockwise = false;
			desc.DepthBias = 0;
			desc.DepthBiasClamp = 0;
			desc.SlopeScaledDepthBias = 0;
			desc.DepthClipEnable = true;
			desc.ScissorEnable = false;
			desc.MultisampleEnable = true;
			desc.FillMode = D3D11_FILL_WIREFRAME;
			desc.CullMode = D3D11_CULL_BACK;
			desc.AntialiasedLineEnable = false;
			HRESULT hr = device->CreateRasterizerState(&desc, rasterizerStates[static_cast<int>(RasterizerState::WireCullBack)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
	}
}

//***********************************************************
// @brief       コンストラクタ
// @param[in]   なし
// @return      なし
//***********************************************************
SamplerManager::SamplerManager(SamplerState state)
{
	ID3D12Device* device = Graphics::Instance().GetDeviceDX12();

	//サンプラーの種類ごとに作成
	D3D12_SAMPLER_DESC samplerDesc = {};

	switch (static_cast<int>(state))
	{
	case 0:
		//! ポイントサンプリング&テクスチャ繰り返しあり  PointWrap
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		samplerDesc.MinLOD = 0.0f;
		samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc.BorderColor[0] = 1.0f;
		samplerDesc.BorderColor[1] = 1.0f;
		samplerDesc.BorderColor[2] = 1.0f;
		samplerDesc.BorderColor[3] = 1.0f;
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		break;

	case 1:
		//! ポイントサンプリング＆テクスチャ繰り返しなし    PointClamp
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		samplerDesc.MinLOD = 0.0f;
		samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc.BorderColor[0] = 1.0f;
		samplerDesc.BorderColor[1] = 1.0f;
		samplerDesc.BorderColor[2] = 1.0f;
		samplerDesc.BorderColor[3] = 1.0f;
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		break;

	case 2:
		//! リニアサンプリング＆テクスチャ繰り返しあり   LinearWrap
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		samplerDesc.MinLOD = 0.0f;
		samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc.BorderColor[0] = 1.0f;
		samplerDesc.BorderColor[1] = 1.0f;
		samplerDesc.BorderColor[2] = 1.0f;
		samplerDesc.BorderColor[3] = 1.0f;
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		break;

	case 3:
		//! リニアサンプリング＆テクスチャ繰り返しなし   LinearClamp
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		samplerDesc.MinLOD = 0.0f;
		samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc.BorderColor[0] = 1.0f;
		samplerDesc.BorderColor[1] = 1.0f;
		samplerDesc.BorderColor[2] = 1.0f;
		samplerDesc.BorderColor[3] = 1.0f;
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		break;

	case 4:
		//! リニアサンプリング&テクスチャボーダー   LinearBorder
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		samplerDesc.MinLOD = 0.0f;
		samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc.BorderColor[0] = 0.0f;
		samplerDesc.BorderColor[1] = 1.0f;
		samplerDesc.BorderColor[2] = 0.0f;
		samplerDesc.BorderColor[3] = 0.0f;
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		break;

	case 5:
		//! リニアサンプリング&テクスチャボーダー   LinearBorder
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS;
		samplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		samplerDesc.BorderColor[0] = 0.0f;
		samplerDesc.BorderColor[1] = 0.0f;
		samplerDesc.BorderColor[2] = 0.0f;
		samplerDesc.BorderColor[3] = 0.0f;
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		break;

	case 6:
		//! 異方サンプリング  AnisotropicWrap
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 8;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		samplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		samplerDesc.BorderColor[0] = 0.0f;
		samplerDesc.BorderColor[1] = 1.0f;
		samplerDesc.BorderColor[2] = 0.0f;
		samplerDesc.BorderColor[3] = 0.0f;
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;
		break;

	case 7:
		//! シャドウマップ用サンプラーステート   ShadowMap
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		samplerDesc.MinLOD = 0.0f;
		samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc.BorderColor[0] = D3D12_FLOAT32_MAX;
		samplerDesc.BorderColor[1] = D3D12_FLOAT32_MAX;
		samplerDesc.BorderColor[2] = D3D12_FLOAT32_MAX;
		samplerDesc.BorderColor[3] = D3D12_FLOAT32_MAX;
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		break;

	default:
		break;
	}

	CreateSampler(device, state, samplerDesc);
}

//***********************************************************
// @brief       デストラクタ
// @param[in]   なし
// @return      なし
//***********************************************************
SamplerManager::~SamplerManager()
{
}

//***********************************************************
// @brief       サンプラーを作成する内部関数
// @param[in]   device       ID3D11Device*
// @param[in]   type         サンプラータイプを選ぶ
// @param[in]   samplerDesc  作成したサンプラーを設定
// @return      なし
//***********************************************************
void SamplerManager::CreateSampler(ID3D12Device* device, SamplerState type, D3D12_SAMPLER_DESC& samplerDesc)
{
	Graphics& graphics = Graphics::Instance();

	//ディスクリプタを取得
	m_sampler_descriptor = graphics.GetSamplerDescriptorHeap()->PopDescriptor();

	// サンプラーを作成してヒープにバインド
	device->CreateSampler(&samplerDesc, m_sampler_descriptor->GetCpuHandle());
}

//***********************************************************
// @brief       コンストラクタ
// @param[in]   なし
// @return      なし
//***********************************************************
RenderStateDX12::RenderStateDX12()
{
#pragma region ブレンドステート

	//合成なし
	{
		D3D12_BLEND_DESC desc{};
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = false;
		desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		blendStates[static_cast<int>(BlendState::Opaque)] = desc;
	}

	//通常合成
	{
		D3D12_BLEND_DESC desc{};
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		blendStates[static_cast<int>(BlendState::Transparency)] = desc;
	}

	//通常合成(OIT)
	{
		D3D12_BLEND_DESC desc{};
		desc.AlphaToCoverageEnable = true;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		blendStates[static_cast<int>(BlendState::OIT)] = desc;
	}

	//加算合成
	{
		D3D12_BLEND_DESC desc{};
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		blendStates[static_cast<int>(BlendState::Additive)] = desc;
	}

	//減算合成
	{
		D3D12_BLEND_DESC desc{};
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		blendStates[static_cast<int>(BlendState::Subtraction)] = desc;
	}

	//乗算合成
	{
		D3D12_BLEND_DESC desc{};
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
		desc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
		desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		blendStates[static_cast<int>(BlendState::Multiply)] = desc;
	}

#pragma endregion

#pragma region デプスステート

	//深度テストあり&深度書き込みあり
	{
		D3D12_DEPTH_STENCIL_DESC desc{};
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		depthStencilStates[static_cast<int>(DepthState::TestAndWrite)] = desc;
	}

	//深度テストあり&深度書き込みなし
	{
		D3D12_DEPTH_STENCIL_DESC desc{};
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		depthStencilStates[static_cast<int>(DepthState::TestOnly)] = desc;
	}

	//深度テストなし&深度書き込みあり
	{
		D3D12_DEPTH_STENCIL_DESC desc{};
		desc.DepthEnable = false;
		desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		depthStencilStates[static_cast<int>(DepthState::WriteOnly)] = desc;
	}

	//深度テストなし&深度書き込みなし
	{
		D3D12_DEPTH_STENCIL_DESC desc{};
		desc.DepthEnable = false;
		desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		depthStencilStates[static_cast<int>(DepthState::NoTestNoWrite)] = desc;
	}

#pragma endregion

#pragma region ラスタライザーステート

	//べた塗り&カリングなし
	{
		D3D12_RASTERIZER_DESC desc{};
		desc.FrontCounterClockwise = true;
		desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		desc.DepthClipEnable = true;
		desc.MultisampleEnable = true;
		desc.FillMode = D3D12_FILL_MODE_SOLID;
		desc.CullMode = D3D12_CULL_MODE_NONE;
		desc.AntialiasedLineEnable = false;

		rasterizerStates[static_cast<int>(RasterizerState::SolidCullNone)] = desc;
	}

	//べた塗り&裏面カリング
	{
		D3D12_RASTERIZER_DESC desc{};
		desc.FrontCounterClockwise = true;
		desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		desc.DepthClipEnable = true;
		desc.MultisampleEnable = true;
		desc.FillMode = D3D12_FILL_MODE_SOLID;
		desc.CullMode = D3D12_CULL_MODE_BACK;
		desc.AntialiasedLineEnable = false;

		rasterizerStates[static_cast<int>(RasterizerState::SolidCullBack)] = desc;
	}

	//ワイヤーフレーム&カリングなし
	{
		D3D12_RASTERIZER_DESC desc{};
		desc.FrontCounterClockwise = true;
		desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		desc.DepthClipEnable = true;
		desc.MultisampleEnable = true;
		desc.FillMode = D3D12_FILL_MODE_WIREFRAME;
		desc.CullMode = D3D12_CULL_MODE_NONE;
		desc.AntialiasedLineEnable = false;

		rasterizerStates[static_cast<int>(RasterizerState::WireCullnone)] = desc;
	}

	//ワイヤーフレーム&裏面カリング
	{
		D3D12_RASTERIZER_DESC desc{};
		desc.FrontCounterClockwise = true;
		desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		desc.DepthClipEnable = true;
		desc.MultisampleEnable = true;
		desc.FillMode = D3D12_FILL_MODE_WIREFRAME;
		desc.CullMode = D3D12_CULL_MODE_BACK;
		desc.AntialiasedLineEnable = false;

		rasterizerStates[static_cast<int>(RasterizerState::WireCullBack)] = desc;
	}

#pragma endregion
}
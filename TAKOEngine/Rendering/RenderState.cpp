#include "Misc.h"
#include "RenderState.h"

// コンストラクタ
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
			//desc.Filter = D3D11_FILTER_ANISOTROPIC;

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
			desc.DepthEnable = true;
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
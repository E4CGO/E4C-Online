#pragma once

#include <wrl.h>
#include <d3d11.h>

enum class SamplerState
{
	PointWrap,
	PointClamp,
	LinearWrap,
	LinearClamp,
	LinearBorder,

	EnumCount
};

// デプスステート
enum class DepthState
{
	TestAndWrite,
	TestOnly,
	WriteOnly,
	NoTestNoWrite,

	EnumCount
};

// ブレンドステート
enum class BlendState
{
	Opaque,
	Transparency,
	OIT,
	Additive,
	Subtraction,
	Multiply,

	EnumCount
};

// ラスタライザステート
enum class RasterizerState
{
	SolidCullNone,
	SolidCullBack,
	WireCullnone,
	WireCullBack,

	EnumCount
};

// レンダーステート
class RenderState
{
public:
	RenderState(ID3D11Device* device);
	~RenderState() = default;

	// サンプラステート取得
	ID3D11SamplerState* GetSamplerState(SamplerState state) const
	{
		return samplerStates[static_cast<int>(state)].Get();
	}

	// デプスステート取得
	ID3D11DepthStencilState* GetDepthStencilState(DepthState state) const
	{
		return depthStencilStates[static_cast<int>(state)].Get();
	}

	// ブレンドステート取得
	ID3D11BlendState* GetBlendState(BlendState state) const
	{
		return blendStates[static_cast<int>(state)].Get();
	}

	// ラスタライザ―ステート取得
	ID3D11RasterizerState* GetRasterizerState(RasterizerState state) const
	{
		return rasterizerStates[static_cast<int>(state)].Get();
	}
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerStates[static_cast<int>(SamplerState::EnumCount)];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilStates[static_cast<int>(DepthState::EnumCount)];
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendStates[static_cast<int>(BlendState::EnumCount)];
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates[static_cast<int>(RasterizerState::EnumCount)];
};

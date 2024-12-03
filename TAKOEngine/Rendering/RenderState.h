//! @file RenderState.h
//! @note

#ifndef __GRAHICS_RENDER_STATE_H__
#define __GRAHICS_RENDER_STATE_H__

#include <wrl.h>
#include <d3d11.h>
#include <d3d12.h>

#include "TAKOEngine\Rendering\Descriptor.h"

//�T���v���[�X�e�[�g
enum class SamplerState
{
	PointWrap,
	PointClamp,
	LinearWrap,
	LinearClamp,
	LinearBorder,
	LinearMirror,
	AnisotropicWrap,
	ShadowMap,

	EnumCount
};

// �f�v�X�X�e�[�g
enum class DepthState
{
	TestAndWrite,
	TestOnly,
	WriteOnly,
	NoTestNoWrite,

	EnumCount
};

// �u�����h�X�e�[�g
enum class BlendState
{
	Opaque,
	Transparency,
	OIT,
	Additive,
	Subtraction,
	Screen,
	Multiply,

	EnumCount
};

// ���X�^���C�U�X�e�[�g
enum class RasterizerState
{
	SolidCullNone,
	SolidCullBack,
	SolidCullBackCCW,
	SolidCullFront,
	WireCullnone,
	WireCullBack,

	EnumCount
};

//****************************************************************
// @class RenderState
// @brief �T���v���[�A�f�v�X�A�u�����h�A���X�^���C�U�[�X�e�[�g�̐ݒ�or�擾
// @par   [����]
//****************************************************************
class RenderState
{
public:
	RenderState(ID3D11Device* device);
	~RenderState() = default;

	// �T���v���X�e�[�g�擾
	ID3D11SamplerState* GetSamplerState(SamplerState state) const
	{
		return samplerStates[static_cast<int>(state)].Get();
	}

	// �f�v�X�X�e�[�g�擾
	ID3D11DepthStencilState* GetDepthStencilState(DepthState state) const
	{
		return depthStencilStates[static_cast<int>(state)].Get();
	}

	// �u�����h�X�e�[�g�擾
	ID3D11BlendState* GetBlendState(BlendState state) const
	{
		return blendStates[static_cast<int>(state)].Get();
	}

	// ���X�^���C�U�\�X�e�[�g�擾
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

//*****************************************************************
// @class SamplerManager
// @brief �T���v���[�𐶐�
// @par   [����]
//*****************************************************************
class SamplerManager
{
public:
	SamplerManager(SamplerState state);
	~SamplerManager();

	const Descriptor* GetDescriptor() { return m_sampler_descriptor; }

private:
	const Descriptor* m_sampler_descriptor = nullptr;

	// �T���v���[���쐬��������֐�
	void CreateSampler(ID3D12Device* device, SamplerState type, D3D12_SAMPLER_DESC& samplerDesc);
};

//****************************************************************
// @class RenderStateDX12
// @brief �f�v�X�A�u�����h�A���X�^���C�U�[�X�e�[�g�̐ݒ�or�擾
// @par   [����]
//****************************************************************
class RenderStateDX12
{
public:
	RenderStateDX12();
	~RenderStateDX12() = default;

	//�u�����h�X�e�[�g�擾
	D3D12_BLEND_DESC GetBlendState(BlendState state) const
	{
		return blendStates[static_cast<int>(state)];
	}

	//�f�v�X�X�e�[�g�擾
	D3D12_DEPTH_STENCIL_DESC GetDepthState(DepthState state) const
	{
		return depthStencilStates[static_cast<int>(state)];
	}

	//���X�^���C�U�[�X�e�[�g�擾
	D3D12_RASTERIZER_DESC GetRasterizer(RasterizerState state) const
	{
		return rasterizerStates[static_cast<int>(state)];
	}

private:
	D3D12_BLEND_DESC blendStates[static_cast<int>(BlendState::EnumCount)];
	D3D12_DEPTH_STENCIL_DESC depthStencilStates[static_cast<int>(DepthState::EnumCount)];
	D3D12_RASTERIZER_DESC rasterizerStates[static_cast<int>(RasterizerState::EnumCount)];
};

#endif // !__GRAHICS_RENDER_STATE_H__
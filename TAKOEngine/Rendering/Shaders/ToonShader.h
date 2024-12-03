//! @file ToonShader.h
//! @note 

#ifndef __GRAHICS_TOON_SHADER_H__
#define __GRAHICS_TOON_SHADER_H__

#include "ModelShader.h"
#include "ModelShaderDX12.h"

//***********************************************************
// @file   ToonShader
// @brief  ToonShader�̃p���[���[�^�ݒ�N���X
// @par    [����]
// @par    �Z�����b�N�ȕ`��
//***********************************************************
class ToonShader : public ModelShader
{
private:
	const char* VertexShaderName = "Data/Shader/ToonVS.cso";
	const char* PixelShaderName = "Data/Shader/ToonPS.cso";

public:
	ToonShader(ID3D11Device* device);
	~ToonShader() override = default;

private:
	// �����_�[�X�e�[�g�ݒ�
	void SetRenderState(const RenderContext& rc) override;
	
	// �V�F�[�_�[���\�[�X�r���[�ݒ�
	void SetShaderResourceView(const ModelResource::Mesh& mesh, ID3D11DeviceContext*& dc) override;

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> toontexture;
};

//***********************************************************
// @file   ToonShaderDX12
// @brief  ToonShaderDX12�̃p���[���[�^�ݒ�N���X
// @par    [����]
// @par    DX12�ɑΉ������Z�����b�N�ȕ`��
//***********************************************************
class ToonShaderDX12 : public ModelShaderDX12
{
public:
	ToonShaderDX12(ID3D12Device* device, bool instancing = false); 
	~ToonShaderDX12() override;

	void Render(const RenderContextDX12& rc, const ModelDX12::Mesh& mesh) override;

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;

	SamplerManager* m_sampler = nullptr;
};

#endif // !__GRAHICS_TOON_SHADER_H__
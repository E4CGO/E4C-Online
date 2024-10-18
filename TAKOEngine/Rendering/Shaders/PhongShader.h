//! @file PhongShader.h
//! @note 

#ifndef __GRAPHICS_PHONG_SHADER_H__
#define __GRAPHICS_PHONG_SHADER_H__

#include "ModelShader.h"
#include "ModelShaderDX12.h"

//*******************************************************
// @class PhongShader
// @brief PhongShader�̃p���[���[�^�ݒ�N���X
// @par   [����]
//*******************************************************
class PhongShader : public ModelShader
{
public:
	PhongShader(ID3D11Device* device) : ModelShader(device, "Data/Shader/PhongVS.cso", "Data/Shader/PhongPS.cso") {};
	~PhongShader() override = default;
private:
	// �����_�[�X�e�[�g�ݒ�
	void SetRenderState(const RenderContext& rc) override;
	
	// �V�F�[�_�[���\�[�X�r���[�ݒ�
	void SetShaderResourceView(const ModelResource::Mesh& mesh, ID3D11DeviceContext*& dc) override;
};

//*******************************************************
// @class PhongShaderDX12
// @brief PhongShaderDX12�̃p���[���[�^�ݒ�N���X
// @par   [����]
// @par   DX12�ł�Shader�`��
//*******************************************************
class PhongShaderDX12 : public ModelShaderDX12
{
public:
	PhongShaderDX12(ID3D12Device* device);
	~PhongShaderDX12() override;

	void Render(const RenderContextDX12& rc, ModelDX12* model) override;

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;

	SamplerManager* m_sampler;
};

#endif // !__GRAPHICS_PHONG_SHADER_H__
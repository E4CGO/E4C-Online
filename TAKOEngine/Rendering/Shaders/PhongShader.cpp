//! @file PhongShader.cpp
//! @note 

#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/GpuResourceUtils.h"
#include "TAKOEngine/Rendering/Shaders/PhongShader.h"

//********************************************************
// @brief       �����_�[�X�e�[�g�ݒ�
// @param[in]   rc  �����_�[�R���e�L�X�g
// @return      �Ȃ�
//********************************************************
void PhongShader::SetRenderState(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// �����_�[�X�e�[�g�ݒ�
	const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Opaque), blend_factor, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));
}

//********************************************************
// @brief       �V�F�[�_�[���\�[�X�r���[�ݒ�
// @param[in]   mash �V�F�[�_�[���\�[�X�Ƃ��Đݒ肷�郁�b�V���f�[�^
// @param[in]   dc   �f�o�C�X�R���e�L�X�g
// @return      �Ȃ�
//********************************************************
void PhongShader::SetShaderResourceView(const ModelResource::Mesh& mesh, ID3D11DeviceContext*& dc)
{
	// �V�F�[�_�[���\�[�X�r���[�ݒ�
	ID3D11ShaderResourceView* srvs[] =
	{
		mesh.material->diffuseMap.Get(),
		mesh.material->normalMap.Get(),
	};
	dc->PSSetShaderResources(0, _countof(srvs), srvs);
}

//***********************************************************
// @brief       �R���X�g���N�^
// @param[in]   �Ȃ�
// @return      �Ȃ�
//***********************************************************
PhongShaderDX12::PhongShaderDX12()
{

}

//***********************************************************
// @brief       �f�X�g���N�^
// @param[in]   �Ȃ�
// @return      �Ȃ�
//***********************************************************
PhongShaderDX12::~PhongShaderDX12()
{
}

//***********************************************************
// @brief       �`��
// @param[in]   rc     �����_�[�R���e�L�X�g
// @param[in]   model  �`��Ώۂ̃��f���f�[�^���w���|�C���^
// @return      �Ȃ�
//***********************************************************
void PhongShaderDX12::Render(const RenderContextDX12& rc, ModelDX12* model)
{

}

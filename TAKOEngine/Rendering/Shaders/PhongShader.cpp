//! @file PhongShader.cpp
//! @note 

#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/Graphics.h"
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
PhongShaderDX12::PhongShaderDX12(ID3D12Device* device)
{
	Graphics&                 graphics = Graphics::Instance();
	const RenderStateDX12* renderState = graphics.GetRenderStateDX12();

	HRESULT hr = S_OK;

	// �V�F�[�_�[
	std::vector<BYTE> vsData, psData;
	{
		GpuResourceUtils::LoadShaderFile("Data/Shader/PhongDX12VS.cso", vsData);
		GpuResourceUtils::LoadShaderFile("Data/Shader/PhongDX12PS.cso", psData);
	}

	// ���[�g�V�O�l�N�`������
	{
		hr = device->CreateRootSignature(
			0,
			vsData.data(),
			vsData.size(),
			IID_PPV_ARGS(m_d3d_root_signature.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_root_signature->SetName(L"PhongShaderRootSignature");
	}

	// �p�C�v���C���X�e�[�g�̐���
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline_state_desc = {};

		// ���[�g�V�O�l�N�`��
		pipeline_state_desc.pRootSignature = m_d3d_root_signature.Get();

		pipeline_state_desc.VS.pShaderBytecode = vsData.data();
		pipeline_state_desc.VS.BytecodeLength  = vsData.size();
		pipeline_state_desc.PS.pShaderBytecode = psData.data();
		pipeline_state_desc.PS.BytecodeLength  = psData.size();

		// ���̓��C�A�E�g
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "BONE_WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "BONE_INDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR",        0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TANGENT",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
		pipeline_state_desc.InputLayout.pInputElementDescs = inputElementDescs;
		pipeline_state_desc.InputLayout.NumElements = _countof(inputElementDescs);

		//�u�����h�X�e�[�g
		pipeline_state_desc.BlendState = renderState->GetBlendState(BlendState::Opaque);

		//�[�x�X�e���V���X�e�[�g
		pipeline_state_desc.DepthStencilState = renderState->GetDepthState(DepthState::TestAndWrite);

		//���X�^���C�U�[�X�e�[�g
		pipeline_state_desc.RasterizerState = renderState->GetRasterizer(RasterizerState::SolidCullNone);

		//�v���~�e�B�u�g�|���W�[
		pipeline_state_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		//�X�g���b�v�X���̃J�b�g�l
		pipeline_state_desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		//�����_�[�^�[�Q�b�g��
		pipeline_state_desc.NumRenderTargets = 1;
		pipeline_state_desc.RTVFormats[0]    = RenderTargetFormat;
		pipeline_state_desc.DSVFormat        = DepthStencilFormat;

		//�}���`�T���v�����O
		pipeline_state_desc.SampleDesc.Count   = 1;
		pipeline_state_desc.SampleDesc.Quality = 0;

		//�A�_�v�^
		pipeline_state_desc.NodeMask = 0;
		pipeline_state_desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		//�p�C�v���C���X�e�[�g�̐���
		hr = device->CreateGraphicsPipelineState(
			&pipeline_state_desc,
			IID_PPV_ARGS(m_d3d_pipeline_state.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_pipeline_state->SetName(L"PhongShaderPipelineState");
	}

	//�T���v���[�X�e�[�g�ݒ�
	m_sampler = graphics.GetSampler(SamplerState::LinearWrap);
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
	//�p�C�v���C���ݒ�
	rc.d3d_command_list->SetGraphicsRootSignature(m_d3d_root_signature.Get());
	rc.d3d_command_list->SetPipelineState(m_d3d_pipeline_state.Get());

	//�V�[���萔�o�b�t�@�ݒ�
	rc.d3d_command_list->SetGraphicsRootDescriptorTable(0, rc.scene_cbv_descriptor->GetGpuHandle());  //CbScene

	Graphics& graphics = Graphics::Instance();

	for (ModelDX12::Mesh& mesh : model->GetMeshes())
	{
		const ModelResource::Mesh* res_mesh = mesh.mesh;
		ModelDX12::Mesh::FrameResource& frame_resource = mesh.frame_resources.at(graphics.GetCurrentBufferIndex());

		// ���b�V���萔�o�b�t�@�ݒ�
		rc.d3d_command_list->SetGraphicsRootDescriptorTable(1, frame_resource.cbv_descriptor->GetGpuHandle());  //CbMesh

		// ���_�o�b�t�@�ݒ�
		rc.d3d_command_list->IASetVertexBuffers(0, 1, mesh.bones.empty() ? &mesh.mesh->d3d_vbv : &frame_resource.d3d_vbv);
		rc.d3d_command_list->IASetIndexBuffer(&res_mesh->d3d_ibv);
		rc.d3d_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// �T�u�Z�b�g
		const ModelResource::Material* material = res_mesh->material;

		// �}�e���A���萔�o�b�t�@�ݒ�
		rc.d3d_command_list->SetGraphicsRootDescriptorTable(2, material->cbv_descriptor->GetGpuHandle());  //CbMaterial

		// �V�F�[�_�[���\�[�X�r���[�ݒ�
		rc.d3d_command_list->SetGraphicsRootDescriptorTable(3, material->srv_descriptor->GetGpuHandle());         //�f�B�t���[�Y�e�N�X�`��
		rc.d3d_command_list->SetGraphicsRootDescriptorTable(4, material->srv_normal_descriptor->GetGpuHandle());  //�@���e�N�X�`��

		//�T���v���[�X�e�[�g�ݒ�
		rc.d3d_command_list->SetGraphicsRootDescriptorTable(5, m_sampler->GetDescriptor()->GetGpuHandle());

		// �`��
		rc.d3d_command_list->DrawIndexedInstanced(static_cast<UINT>(res_mesh->indices.size()), 1, 0, 0, 0);
	}
}

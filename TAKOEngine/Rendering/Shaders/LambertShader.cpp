#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/Graphics.h"
#include "TAKOEngine/Rendering/Model.h"
#include "TAKOEngine/Rendering/ModelDX12.h"
#include "TAKOEngine/Rendering/Shaders/LambertShader.h"

LambertShader::LambertShader()
{
	ID3D12Device* device = Graphics::Instance().GetDeviceDX12();

	HRESULT hr = S_OK;

	// ���[�g�V�O�l�`���̐���
	{
		// �f�B�X�N���v�^�����W�̐ݒ�
		D3D12_DESCRIPTOR_RANGE d3d_descriptor_ranges[4] = {};
		{
			// b0
			d3d_descriptor_ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;	// �����W���
			d3d_descriptor_ranges[0].NumDescriptors = 1;							// �f�B�X�N���v�^��
			d3d_descriptor_ranges[0].BaseShaderRegister = 0;						// �擪���W�X�^�ԍ�
			d3d_descriptor_ranges[0].RegisterSpace = 0;								// ���܂����킹�邽�߂̃X�y�[�X
			d3d_descriptor_ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			// b1
			d3d_descriptor_ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;	// �����W���
			d3d_descriptor_ranges[1].NumDescriptors = 1;							// �f�B�X�N���v�^��
			d3d_descriptor_ranges[1].BaseShaderRegister = 1;						// �擪���W�X�^�ԍ�
			d3d_descriptor_ranges[1].RegisterSpace = 0;								// ���܂����킹�邽�߂̃X�y�[�X
			d3d_descriptor_ranges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			// b2
			d3d_descriptor_ranges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;	// �����W���
			d3d_descriptor_ranges[2].NumDescriptors = 1;							// �f�B�X�N���v�^��
			d3d_descriptor_ranges[2].BaseShaderRegister = 2;						// �擪���W�X�^�ԍ�
			d3d_descriptor_ranges[2].RegisterSpace = 0;								// ���܂����킹�邽�߂̃X�y�[�X
			d3d_descriptor_ranges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			// t0
			d3d_descriptor_ranges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// �����W���
			d3d_descriptor_ranges[3].NumDescriptors = 1;							// �f�B�X�N���v�^��
			d3d_descriptor_ranges[3].BaseShaderRegister = 0;						// �擪���W�X�^�ԍ�
			d3d_descriptor_ranges[3].RegisterSpace = 0;								// ���܂����킹�邽�߂̃X�y�[�X
			d3d_descriptor_ranges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		}

		// ���[�g�p�����[�^�̐ݒ�
		D3D12_ROOT_PARAMETER d3d_root_parameters[4] = {};
		{
			d3d_root_parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// �p�����[�^���
			d3d_root_parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;				// �ǂ̃V�F�[�_�[���痘�p�\��
			d3d_root_parameters[0].DescriptorTable.NumDescriptorRanges = 1;							// �f�B�X�N���v�^�����W��
			d3d_root_parameters[0].DescriptorTable.pDescriptorRanges = &d3d_descriptor_ranges[0];	// �f�B�X�N���v�^�����W�̃A�h���X

			d3d_root_parameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// �p�����[�^���
			d3d_root_parameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;				// �ǂ̃V�F�[�_�[���痘�p�\��
			d3d_root_parameters[1].DescriptorTable.NumDescriptorRanges = 1;							// �f�B�X�N���v�^�����W��
			d3d_root_parameters[1].DescriptorTable.pDescriptorRanges = &d3d_descriptor_ranges[1];	// �f�B�X�N���v�^�����W�̃A�h���X

			d3d_root_parameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// �p�����[�^���
			d3d_root_parameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;				// �ǂ̃V�F�[�_�[���痘�p�\��
			d3d_root_parameters[2].DescriptorTable.NumDescriptorRanges = 1;							// �f�B�X�N���v�^�����W��
			d3d_root_parameters[2].DescriptorTable.pDescriptorRanges = &d3d_descriptor_ranges[2];	// �f�B�X�N���v�^�����W�̃A�h���X

			d3d_root_parameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// �p�����[�^���
			d3d_root_parameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;				// �ǂ̃V�F�[�_�[���痘�p�\��
			d3d_root_parameters[3].DescriptorTable.NumDescriptorRanges = 1;							// �f�B�X�N���v�^�����W��
			d3d_root_parameters[3].DescriptorTable.pDescriptorRanges = &d3d_descriptor_ranges[3];	// �f�B�X�N���v�^�����W�̃A�h���X
		}

		// �ÓI�T���v���[�̐ݒ�.
		D3D12_STATIC_SAMPLER_DESC d3d_static_sampler_descs[1] = {};
		{
			d3d_static_sampler_descs[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			d3d_static_sampler_descs[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			d3d_static_sampler_descs[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			d3d_static_sampler_descs[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			d3d_static_sampler_descs[0].MipLODBias = 0;
			d3d_static_sampler_descs[0].MaxAnisotropy = 0;
			d3d_static_sampler_descs[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			d3d_static_sampler_descs[0].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
			d3d_static_sampler_descs[0].MinLOD = 0.0f;
			d3d_static_sampler_descs[0].MaxLOD = D3D12_FLOAT32_MAX;
			d3d_static_sampler_descs[0].ShaderRegister = 0;
			d3d_static_sampler_descs[0].RegisterSpace = 0;
			d3d_static_sampler_descs[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		}

		// ���[�g�V�O�l�`���̐ݒ�
		D3D12_ROOT_SIGNATURE_DESC d3d_root_signature_desc = {};
		d3d_root_signature_desc.NumParameters = _countof(d3d_root_parameters);
		d3d_root_signature_desc.pParameters = d3d_root_parameters;
		d3d_root_signature_desc.NumStaticSamplers = _countof(d3d_static_sampler_descs);
		d3d_root_signature_desc.pStaticSamplers = d3d_static_sampler_descs;
		d3d_root_signature_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		// �V���A���C�Y
		Microsoft::WRL::ComPtr<ID3DBlob> d3d_signature_blob;
		Microsoft::WRL::ComPtr<ID3DBlob> d3d_error_blob;
		hr = D3D12SerializeRootSignature(
			&d3d_root_signature_desc,
			D3D_ROOT_SIGNATURE_VERSION_1,
			d3d_signature_blob.GetAddressOf(),
			d3d_error_blob.GetAddressOf()
		);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// ���[�g�V�O�l�`������
		hr = device->CreateRootSignature(
			0,
			d3d_signature_blob->GetBufferPointer(),
			d3d_signature_blob->GetBufferSize(),
			IID_PPV_ARGS(m_d3d_root_signature.GetAddressOf())
		);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_root_signature->SetName(L"LambertShaderRootSignature");
	}

	// �p�C�v���C���X�e�[�g�̐���
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC d3d_graphics_pipeline_state_desc = {};

		// ���[�g�V�O�l�`��
		d3d_graphics_pipeline_state_desc.pRootSignature = m_d3d_root_signature.Get();

		// �V�F�[�_�[
		std::vector<BYTE> vsData, psData;
		LoadShaderFile("Data/Shader/LambertDX12VS.cso", vsData);
		LoadShaderFile("Data/Shader/LambertDX12PS.cso", psData);

		d3d_graphics_pipeline_state_desc.VS.pShaderBytecode = vsData.data();
		d3d_graphics_pipeline_state_desc.VS.BytecodeLength = vsData.size();
		d3d_graphics_pipeline_state_desc.PS.pShaderBytecode = psData.data();
		d3d_graphics_pipeline_state_desc.PS.BytecodeLength = psData.size();

		// ���̓��C�A�E�g
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "WEIGHTS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "BONES",    0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
		d3d_graphics_pipeline_state_desc.InputLayout.pInputElementDescs = inputElementDescs;
		d3d_graphics_pipeline_state_desc.InputLayout.NumElements = _countof(inputElementDescs);

		// �u�����h�X�e�[�g
		d3d_graphics_pipeline_state_desc.BlendState.AlphaToCoverageEnable = false;
		d3d_graphics_pipeline_state_desc.BlendState.IndependentBlendEnable = false;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].BlendEnable = true;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].LogicOpEnable = false;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		// �[�x�X�e���V���X�e�[�g
		d3d_graphics_pipeline_state_desc.DepthStencilState.DepthEnable = true;
		d3d_graphics_pipeline_state_desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		d3d_graphics_pipeline_state_desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		d3d_graphics_pipeline_state_desc.DepthStencilState.StencilEnable = false;

		// ���X�^���C�U�[�X�e�[�g
		d3d_graphics_pipeline_state_desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		d3d_graphics_pipeline_state_desc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		d3d_graphics_pipeline_state_desc.RasterizerState.FrontCounterClockwise = true;
		d3d_graphics_pipeline_state_desc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		d3d_graphics_pipeline_state_desc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		d3d_graphics_pipeline_state_desc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		d3d_graphics_pipeline_state_desc.RasterizerState.DepthClipEnable = true;
		d3d_graphics_pipeline_state_desc.RasterizerState.MultisampleEnable = false;
		d3d_graphics_pipeline_state_desc.RasterizerState.AntialiasedLineEnable = false;
		d3d_graphics_pipeline_state_desc.RasterizerState.ForcedSampleCount = 0;
		d3d_graphics_pipeline_state_desc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		// �v���~�e�B�u�g�|���W�[
		d3d_graphics_pipeline_state_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// �X�g���b�v���̃J�b�g�l
		d3d_graphics_pipeline_state_desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		// �����_�[�^�[�Q�b�g��
		d3d_graphics_pipeline_state_desc.NumRenderTargets = 1;
		d3d_graphics_pipeline_state_desc.RTVFormats[0] = RenderTargetFormat;
		d3d_graphics_pipeline_state_desc.DSVFormat = DepthStencilFormat;

		// �}���`�T���v�����O
		d3d_graphics_pipeline_state_desc.SampleDesc.Count = 1;
		d3d_graphics_pipeline_state_desc.SampleDesc.Quality = 0;

		// �A�_�v�^
		d3d_graphics_pipeline_state_desc.NodeMask = 0;
		d3d_graphics_pipeline_state_desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		// �p�C�v���C���X�e�[�g����
		hr = device->CreateGraphicsPipelineState(
			&d3d_graphics_pipeline_state_desc,
			IID_PPV_ARGS(&m_d3d_pipeline_state)
		);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_pipeline_state->SetName(L"LambertShaderPipelineState");
	}
}

LambertShader::~LambertShader()
{
}

void LambertShader::Begin(const RenderContext& rc)
{
}

// �`��J�n
void LambertShader::Begin(const RenderContextDX12& rc)
{
	// �p�C�v���C���ݒ�
	rc.d3d_command_list->SetGraphicsRootSignature(m_d3d_root_signature.Get());
	rc.d3d_command_list->SetPipelineState(m_d3d_pipeline_state.Get());

	// �V�[���萔�o�b�t�@�ݒ�
	rc.d3d_command_list->SetGraphicsRootDescriptorTable(0, rc.scene_cbv_descriptor->GetGpuHandle());
}

void LambertShader::Draw(const RenderContextDX12& rc, Model* model)
{
}

// �`��
void LambertShader::Draw(const RenderContextDX12& rc, ModelDX12* model)
{
	Graphics& graphics = Graphics::Instance();
	const ModelResource* resource = model->GetResource();
	const std::vector<ModelDX12::Node>& nodes = model->GetNodes();

	struct CbSkeleton
	{
		DirectX::XMFLOAT4X4 boneTransforms[256];
	};

	for (ModelDX12::Mesh& mesh : model->GetMeshes())
	{
		const ModelResource::Mesh* res_mesh = mesh.mesh;
		ModelDX12::FrameResource& frame_resource = mesh.frame_resources.at(graphics.GetCurrentBufferIndex());

		// ���b�V���萔�o�b�t�@�ݒ�
		rc.d3d_command_list->SetGraphicsRootDescriptorTable(1, frame_resource.cbv_descriptor->GetGpuHandle());

		// �X�P���g���p�萔�o�b�t�@�X�V
		CbSkeleton* cbSkeleton = reinterpret_cast<CbSkeleton*>(frame_resource.cbv_data);
		if (res_mesh->bones.size() > 0)
		{
			for (size_t i = 0; i < res_mesh->bones.size(); ++i)
			{
				DirectX::XMMATRIX worldTransform = DirectX::XMLoadFloat4x4(&nodes.at(res_mesh->bones.at(i).nodeIndex).world_transform);
				DirectX::XMMATRIX offsetTransform = DirectX::XMLoadFloat4x4(&res_mesh->bones.at(i).offsetTransform);
				DirectX::XMMATRIX boneTransform = offsetTransform * worldTransform;
				DirectX::XMStoreFloat4x4(&cbSkeleton->boneTransforms[i], boneTransform);
			}
		}
		else
		{
			cbSkeleton->boneTransforms[0] = nodes.at(res_mesh->nodeIndex).world_transform;
		}

		// ���_�o�b�t�@�ݒ�
		rc.d3d_command_list->IASetVertexBuffers(0, 1, &res_mesh->d3d_vbv);
		rc.d3d_command_list->IASetIndexBuffer(&res_mesh->d3d_ibv);
		rc.d3d_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// �T�u�Z�b�g

		const ModelResource::Material* material = res_mesh->material;

		// �}�e���A���萔�o�b�t�@�ݒ�
		rc.d3d_command_list->SetGraphicsRootDescriptorTable(2, material->cbv_descriptor->GetGpuHandle());

		// �V�F�[�_�[���\�[�X�r���[�ݒ�
		rc.d3d_command_list->SetGraphicsRootDescriptorTable(3, material->srv_descriptor->GetGpuHandle());

		// �`��
		rc.d3d_command_list->DrawIndexedInstanced(static_cast<UINT>(res_mesh->indices.size()), 1, 0, 0, 0);
	}
}

void LambertShader::End(const RenderContext& rc)
{
}

// �`��I��
void LambertShader::End(const RenderContextDX12& rc)
{
}
//! @file ColorGradingShader.cpp
//! @note

#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/Graphics.h"
#include "ColorGradingShader.h"

//**********************************************************************
// @brief     �R���X�g���N�^
// @param[in] device  ID3D11Device*
// @return    �Ȃ�
//**********************************************************************
ColorGradingShader::ColorGradingShader(ID3D11Device* device) : SpriteShader(device, "Data/Shader/SpriteVS.cso", "Data/Shader/ColorGradingPS.cso")
{
	// �V�[���p�o�b�t�@
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(CBColorGrading),
		constantBuffer.GetAddressOf()
	);
}

//**********************************************************************
// @brief     �R���X�^���g�o�b�t�@�X�V�֐�
// @param[in] rc  �����_�[�R���X�g���N�^
// @return    �Ȃ�
//**********************************************************************
void ColorGradingShader::UpdateConstantBuffer(const RenderContext& rc)
{
	CBColorGrading cbColorGrading;
	cbColorGrading.hueShift = rc.colorGradingData.hueShift;
	cbColorGrading.saturation = rc.colorGradingData.saturation;
	cbColorGrading.brightness = rc.colorGradingData.brightness;
	rc.deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &cbColorGrading, 0, 0);
}

//**********************************************************************
// @brief     �R���X�g���N�^
// @param[in] device  ID3D12Device*
// @return    �Ȃ�
//**********************************************************************
ColorGradingShaderDX12::ColorGradingShaderDX12(ID3D12Device* device)
{
	Graphics&                 graphics = Graphics::Instance();
	const RenderStateDX12* renderState = graphics.GetRenderStateDX12();

	HRESULT hr = S_OK;

	// �V�F�[�_�[
	std::vector<BYTE> vsData, psData;
	{
		GpuResourceUtils::LoadShaderFile("Data/Shader/ColorGradingDX12VS.cso", vsData);
		GpuResourceUtils::LoadShaderFile("Data/Shader/ColorGradingDX12PS.cso", psData);
	}

	// ���[�g�V�O�l�N�`���̐���
	{
		hr = device->CreateRootSignature(
			0,
			vsData.data(),
			vsData.size(),
			IID_PPV_ARGS(m_d3d_root_signature.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_root_signature->SetName(L"ColorGradingShaderRootSignature");
	}

	// �p�C�v���C���X�e�[�g�̐���
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC d3d_graphics_pipeline_state_desc = {};

		// ���[�g�V�O�l�`��
		d3d_graphics_pipeline_state_desc.pRootSignature = m_d3d_root_signature.Get();

		d3d_graphics_pipeline_state_desc.VS.pShaderBytecode = vsData.data();
		d3d_graphics_pipeline_state_desc.VS.BytecodeLength  = vsData.size();
		d3d_graphics_pipeline_state_desc.PS.pShaderBytecode = psData.data();
		d3d_graphics_pipeline_state_desc.PS.BytecodeLength  = psData.size();

		// ���̓��C�A�E�g
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
		d3d_graphics_pipeline_state_desc.InputLayout.pInputElementDescs = inputElementDescs;
		d3d_graphics_pipeline_state_desc.InputLayout.NumElements = _countof(inputElementDescs);

		// �u�����h�X�e�[�g
		d3d_graphics_pipeline_state_desc.BlendState = renderState->GetBlendState(BlendState::Transparency);

		// �[�x�X�e���V���X�e�[�g
		d3d_graphics_pipeline_state_desc.DepthStencilState.DepthEnable    = true;
		d3d_graphics_pipeline_state_desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		d3d_graphics_pipeline_state_desc.DepthStencilState.DepthFunc      = D3D12_COMPARISON_FUNC_ALWAYS;
		d3d_graphics_pipeline_state_desc.DepthStencilState.StencilEnable  = false;

		// ���X�^���C�U�[�X�e�[�g
		d3d_graphics_pipeline_state_desc.RasterizerState.FillMode              = D3D12_FILL_MODE_SOLID;
		d3d_graphics_pipeline_state_desc.RasterizerState.CullMode              = D3D12_CULL_MODE_NONE;
		d3d_graphics_pipeline_state_desc.RasterizerState.FrontCounterClockwise = false;
		d3d_graphics_pipeline_state_desc.RasterizerState.DepthBias             = D3D12_DEFAULT_DEPTH_BIAS;
		d3d_graphics_pipeline_state_desc.RasterizerState.DepthBiasClamp        = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		d3d_graphics_pipeline_state_desc.RasterizerState.SlopeScaledDepthBias  = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		d3d_graphics_pipeline_state_desc.RasterizerState.DepthClipEnable       = true;
		d3d_graphics_pipeline_state_desc.RasterizerState.MultisampleEnable     = false;
		d3d_graphics_pipeline_state_desc.RasterizerState.AntialiasedLineEnable = false;
		d3d_graphics_pipeline_state_desc.RasterizerState.ForcedSampleCount     = 0;
		d3d_graphics_pipeline_state_desc.RasterizerState.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

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
			IID_PPV_ARGS(m_d3d_pipeline_state.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_pipeline_state->SetName(L"ColorGradingShaderPipelineState");
	}
}

//***********************************************************
// @brief       �`��
// @param[in]   rc      �����_�[�R���e�L�X�g
// @param[in]   sprite  �`��Ώۂ̃X�v���C�g�f�[�^���w���|�C���^
// @return      �Ȃ�
//***********************************************************
void ColorGradingShaderDX12::Render(const RenderContextDX12& rc, SpriteDX12* sprite)
{
	Graphics& graphics = Graphics::Instance();
	const SpriteDX12::FrameResource& frame_resource = sprite->m_frame_resources.at(graphics.GetCurrentBufferIndex());

	//�p�C�v���C���ݒ�
	rc.d3d_command_list->SetPipelineState(m_d3d_pipeline_state.Get());
	rc.d3d_command_list->SetGraphicsRootSignature(m_d3d_root_signature.Get());

	//�f�B�X�N���v�^�e�[�u��
	rc.d3d_command_list->SetGraphicsRootDescriptorTable(0, frame_resource.cbv_descriptor->GetGpuHandle());
	rc.d3d_command_list->SetGraphicsRootDescriptorTable(1, sprite->GetDescriptor()->GetGpuHandle());

	//���_�o�b�t�@
	rc.d3d_command_list->IASetVertexBuffers(0, 1, &frame_resource.d3d_vbv);
	rc.d3d_command_list->IASetIndexBuffer(&frame_resource.d3d_ibv);
	rc.d3d_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//�`��
	rc.d3d_command_list->DrawIndexedInstanced(sprite->GetSpriteCount() * 6, 1, 0, 0, 0);
}

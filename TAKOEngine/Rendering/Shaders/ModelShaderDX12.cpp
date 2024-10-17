//! @file ModelShaderDX12.cpp
//! @note

#include <cassert>
#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/Graphics.h"
#include "TAKOEngine/Rendering/Shaders/ModelShaderDX12.h"

//*************************************************************
// @brief       コンストラクタ
// @param[in]   device　ID3D12Device*
// @return      なし
//*************************************************************
SkinningPipeline::SkinningPipeline(ID3D12Device* device)
{
	HRESULT hr = S_OK;

	//シェーダー読み込み
	std::vector<BYTE> cs_data;
	{
		GpuResourceUtils::LoadShaderFile("Data/Shader/SkinningCS.cso", cs_data);
	}

	//ルートシグネクチャの生成
	{
		hr = device->CreateRootSignature(
			0,
			cs_data.data(),
			cs_data.size(),
			IID_PPV_ARGS(m_d3d_root_signature.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_root_signature->SetName(L"SkinningRootSignature");
	}

	//パイプラインステートの生成
	{
		D3D12_COMPUTE_PIPELINE_STATE_DESC d3d_compute_pipeline_state_desc = {};

		//ルートシグネクチャ
		d3d_compute_pipeline_state_desc.pRootSignature = m_d3d_root_signature.Get();

		d3d_compute_pipeline_state_desc.CS.pShaderBytecode = cs_data.data();
		d3d_compute_pipeline_state_desc.CS.BytecodeLength  = cs_data.size();

		//パイプラインステート生成
		hr = device->CreateComputePipelineState(
			&d3d_compute_pipeline_state_desc,
			IID_PPV_ARGS(m_d3d_pipeline_state.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_pipeline_state->SetName(L"SkinningPipelineState");
	}
}

//*************************************************************
// @brief       デストラクタ
// @param[in]   なし
// @return      なし
//*************************************************************
SkinningPipeline::~SkinningPipeline()
{
}

//*************************************************************
// @brief       計算
// @param[in]   rc     レンダーコンテキスト(DX12)
// @param[in]   model  描画対象のモデルデータを指すポインタ(DX12)
// @return      なし
//*************************************************************
void SkinningPipeline::Compute(const RenderContextDX12& rc, ModelDX12* model)
{
	Graphics& graphics = Graphics::Instance();

	//パイプライン設定
	rc.d3d_command_list->SetComputeRootSignature(m_d3d_root_signature.Get());
	rc.d3d_command_list->SetPipelineState(m_d3d_pipeline_state.Get());

	for (const ModelDX12::Mesh& mesh : model->GetMeshes())
	{
		if (mesh.mesh->bones.empty()) continue;

		const ModelDX12::Mesh::FrameResource& frame_resource = mesh.frame_resources.at(graphics.GetCurrentBufferIndex());

		//ディスクリプタ設定
		rc.d3d_command_list->SetComputeRootDescriptorTable(0, mesh.mesh->srv_descriptor->GetGpuHandle());
		rc.d3d_command_list->SetComputeRootDescriptorTable(1, frame_resource.uav_descriptor->GetGpuHandle());
		rc.d3d_command_list->SetComputeRootDescriptorTable(2, frame_resource.cbv_descriptor->GetGpuHandle());

		//計算実行
		rc.d3d_command_list->Dispatch(mesh.vertex_count / SkinningCSThreadNum, 1, 1);
	}
}
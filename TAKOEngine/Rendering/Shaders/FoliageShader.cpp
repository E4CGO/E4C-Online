//! @file FoliageShader.cpp
//! @note

#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/Graphics.h"
#include "TAKOEngine/Editor/Camera/CameraManager.h"

#include "TAKOEngine/Rendering/Shaders/FoliageShader.h"

//***********************************************************
// @brief       コンストラクタ
// @param[in]   device　    ID3D12Device*
// @return      なし
//***********************************************************
FoliageShader::FoliageShader(ID3D12Device* device)
{
	Graphics&                 graphics = Graphics::Instance();
	const RenderStateDX12* renderState = graphics.GetRenderStateDX12();

	HRESULT hr = S_OK;

	// シェーダー
	std::vector<BYTE> vsData, psData;
	{
		GpuResourceUtils::LoadShaderFile("Data/Shader/FoliageVS.cso", vsData);
		GpuResourceUtils::LoadShaderFile("Data/Shader/FoliagePS.cso", psData);
	}

	// ルートシグネクチャの生成
	{
		hr = device->CreateRootSignature(
			0,
			vsData.data(),
			vsData.size(),
			IID_PPV_ARGS(m_d3d_root_signature.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_root_signature->SetName(L"FoliageShaderRootSignature");
	}

	// パイプラインステートの生成
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC d3d_graphics_pipeline_state_desc = {};

		// ルートシグネクチャ
		d3d_graphics_pipeline_state_desc.pRootSignature = m_d3d_root_signature.Get();

		d3d_graphics_pipeline_state_desc.VS.pShaderBytecode = vsData.data();
		d3d_graphics_pipeline_state_desc.VS.BytecodeLength  = vsData.size();
		d3d_graphics_pipeline_state_desc.PS.pShaderBytecode = psData.data();
		d3d_graphics_pipeline_state_desc.PS.BytecodeLength  = psData.size();

		// 入力レイアウト
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
		d3d_graphics_pipeline_state_desc.InputLayout.pInputElementDescs = inputElementDescs;
		d3d_graphics_pipeline_state_desc.InputLayout.NumElements        = _countof(inputElementDescs);

		// ブレンドステート
		d3d_graphics_pipeline_state_desc.BlendState = renderState->GetBlendState(BlendState::Opaque);

		// 深度ステンシルステート
		d3d_graphics_pipeline_state_desc.DepthStencilState = renderState->GetDepthState(DepthState::TestAndWrite);

		// ラスタライザーステート
		d3d_graphics_pipeline_state_desc.RasterizerState = renderState->GetRasterizer(RasterizerState::SolidCullBack);

		// プリミティブトポロジー
		d3d_graphics_pipeline_state_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// ストリップ時のカット値
		d3d_graphics_pipeline_state_desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		// レンダーターゲット数
		d3d_graphics_pipeline_state_desc.NumRenderTargets = 1;
		d3d_graphics_pipeline_state_desc.RTVFormats[0]    = RenderTargetFormat;
		d3d_graphics_pipeline_state_desc.DSVFormat        = DepthStencilFormat;

		// マルチサンプリング
		d3d_graphics_pipeline_state_desc.SampleDesc.Count   = 1;
		d3d_graphics_pipeline_state_desc.SampleDesc.Quality = 0;

		// アダプタ
		d3d_graphics_pipeline_state_desc.NodeMask   = 0;
		d3d_graphics_pipeline_state_desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		// パイプラインステート生成
		hr = device->CreateGraphicsPipelineState(
			&d3d_graphics_pipeline_state_desc,
			IID_PPV_ARGS(m_d3d_pipeline_state.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_pipeline_state->SetName(L"FoliageShaderPipelineState");
	}

	//サンプラーステート設定
	m_sampler = graphics.GetSampler(SamplerState::AnisotropicWrap);
}

//***********************************************************
// @brief       デストラクタ
// @param[in]   なし
// @return      なし
//***********************************************************
FoliageShader::~FoliageShader()
{
}

//***********************************************************
// @brief       描画
// @param[in]   rc     レンダーコンテキスト
// @param[in]   mesh   描画対象のモデルデータのmesh
// @return      なし
//***********************************************************
void FoliageShader::Render(const RenderContextDX12& rc, const ModelDX12::Mesh& mesh)
{
	Graphics& graphics = Graphics::Instance();

	// パイプライン設定
	rc.d3d_command_list->SetGraphicsRootSignature(m_d3d_root_signature.Get());
	rc.d3d_command_list->SetPipelineState(m_d3d_pipeline_state.Get());

	// シーン定数バッファ設定
	rc.d3d_command_list->SetGraphicsRootDescriptorTable(0, rc.scene_cbv_descriptor->GetGpuHandle());

	// シャドウマップ設定
	rc.d3d_command_list->SetGraphicsRootDescriptorTable(6, rc.shadowMap.shadow_srv_descriptor->GetGpuHandle());
	rc.d3d_command_list->SetGraphicsRootDescriptorTable(7, rc.shadowMap.shadow_sampler_descriptor->GetGpuHandle());

	const ModelResource::Mesh* res_mesh = mesh.mesh;
	const ModelDX12::Mesh::FrameResource& frame_resource = mesh.frame_resources.at(graphics.GetCurrentBufferIndex());

	// メッシュ定数バッファ設定
	rc.d3d_command_list->SetGraphicsRootDescriptorTable(1, frame_resource.cbv_descriptor->GetGpuHandle());

	// 頂点バッファ設定
	rc.d3d_command_list->IASetVertexBuffers(0, 1, mesh.bones.empty() ? &mesh.mesh->d3d_vbv : &frame_resource.d3d_vbv);
	rc.d3d_command_list->IASetIndexBuffer(&res_mesh->d3d_ibv);
	rc.d3d_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// サブセット
	const ModelResource::Material* material = res_mesh->material;

	// マテリアル定数バッファ設定
	rc.d3d_command_list->SetGraphicsRootDescriptorTable(2, material->cbv_descriptor->GetGpuHandle());

	// シェーダーリソースビュー設定
	rc.d3d_command_list->SetGraphicsRootDescriptorTable(3, material->srv_descriptor->GetGpuHandle());
	rc.d3d_command_list->SetGraphicsRootDescriptorTable(4, material->srv_emissive_descriptor->GetGpuHandle());

	//サンプラーステート設定
	rc.d3d_command_list->SetGraphicsRootDescriptorTable(5, m_sampler->GetDescriptor()->GetGpuHandle());

	// 描画
	rc.d3d_command_list->DrawIndexedInstanced(static_cast<UINT>(res_mesh->indices.size()), 1, 0, 0, 0);
}

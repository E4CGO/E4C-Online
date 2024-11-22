//! @file Skydomeshader.cpp
//! @note

#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/Graphics.h"
#include "TAKOEngine/Rendering/Shaders/SkydomeShader.h"

//********************************************************
// @brief       レンダーステート設定
// @param[in]   rc  レンダーコンテキスト
// @return      なし
//********************************************************
void SkydomeShader::SetRenderState(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	
	// レンダーステート設定
	const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Opaque), blend_factor, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullBack));
}

//********************************************************
// @brief       シェーダーリソースビュー設定
// @param[in]   mash シェーダーリソースとして設定するメッシュデータ
// @param[in]   dc   デバイスコンテキスト
// @return      なし
//********************************************************
void SkydomeShader::SetShaderResourceView(const ModelResource::Mesh& mesh, ID3D11DeviceContext*& dc)
{
	// シェーダーリソースビュー設定
	dc->PSSetShaderResources(0, 1, mesh.material->diffuseMap.GetAddressOf());
}

//***********************************************************
// @brief       コンストラクタ
// @param[in]   device　    ID3D12Device*
// @return      なし
//***********************************************************
SkydomeShaderDX12::SkydomeShaderDX12(ID3D12Device* device)
{
	Graphics&                 graphics = Graphics::Instance();
	const RenderStateDX12* renderState = graphics.GetRenderStateDX12();

	HRESULT hr = S_OK;

	// シェーダー
	std::vector<BYTE> vsData, psData;
	{
		GpuResourceUtils::LoadShaderFile("Data/Shader/SkyBoxDX12VS.cso", vsData);
		GpuResourceUtils::LoadShaderFile("Data/Shader/SkyBoxDX12PS.cso", psData);
	}

	// ルートシグネクチャ生成
	{
		hr = device->CreateRootSignature(
			0,
			vsData.data(),
			vsData.size(),
			IID_PPV_ARGS(m_d3d_root_signature.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_root_signature->SetName(L"SkydomeShaderRootSignature");
	}

	// パイプラインステートの生成
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline_state_desc = {};

		// ルートシグネクチャ
		pipeline_state_desc.pRootSignature = m_d3d_root_signature.Get();

		pipeline_state_desc.VS.pShaderBytecode = vsData.data();
		pipeline_state_desc.VS.BytecodeLength  = vsData.size();
		pipeline_state_desc.PS.pShaderBytecode = psData.data();
		pipeline_state_desc.PS.BytecodeLength  = psData.size();

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
		pipeline_state_desc.InputLayout.pInputElementDescs = inputElementDescs;
		pipeline_state_desc.InputLayout.NumElements = _countof(inputElementDescs);

		//ブレンドステート
		pipeline_state_desc.BlendState = renderState->GetBlendState(BlendState::Opaque);

		//深度ステンシルステート
		pipeline_state_desc.DepthStencilState = renderState->GetDepthState(DepthState::NoTestNoWrite);

		//ラスタライザーステート
		pipeline_state_desc.RasterizerState = renderState->GetRasterizer(RasterizerState::SolidCullNone);

		//プリミティブトポロジー
		pipeline_state_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		//ストリップス時のカット値
		pipeline_state_desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		//レンダーターゲット数
		pipeline_state_desc.NumRenderTargets = 1;
		pipeline_state_desc.RTVFormats[0]    = RenderTargetFormat;
		pipeline_state_desc.DSVFormat        = DepthStencilFormat;

		//マルチサンプリング
		pipeline_state_desc.SampleDesc.Count   = 1;
		pipeline_state_desc.SampleDesc.Quality = 0;

		//アダプタ
		pipeline_state_desc.NodeMask = 0;
		pipeline_state_desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		//パイプラインステートの生成
		hr = device->CreateGraphicsPipelineState(
			&pipeline_state_desc,
			IID_PPV_ARGS(m_d3d_pipeline_state.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_pipeline_state->SetName(L"SkydomeShaderPipelineState");
	}

	//サンプラーステート設定
	m_sampler = graphics.GetSampler(SamplerState::LinearWrap);
}

//***********************************************************
// @brief       デストラクタ
// @param[in]   なし
// @return      なし
//***********************************************************
SkydomeShaderDX12::~SkydomeShaderDX12()
{
}

//***********************************************************
// @brief       描画
// @param[in]   rc     レンダーコンテキスト
// @param[in]   model  描画対象のモデルデータを指すポインタ
// @return      なし
//***********************************************************
void SkydomeShaderDX12::Render(const RenderContextDX12& rc, ModelDX12* model)
{
	Graphics& graphics = Graphics::Instance();

	//パイプライン設定
	rc.d3d_command_list->SetGraphicsRootSignature(m_d3d_root_signature.Get());
	rc.d3d_command_list->SetPipelineState(m_d3d_pipeline_state.Get());

	//シーン定数バッファ設定
	rc.d3d_command_list->SetGraphicsRootDescriptorTable(0, rc.scene_cbv_descriptor->GetGpuHandle());  //CbScene

	for (const ModelDX12::Mesh& mesh : model->GetMeshes())
	{
		const ModelResource::Mesh* res_mesh = mesh.mesh;
		const ModelDX12::Mesh::FrameResource& frame_resource = mesh.frame_resources.at(graphics.GetCurrentBufferIndex());

		// メッシュ定数バッファ設定
		rc.d3d_command_list->SetGraphicsRootDescriptorTable(1, frame_resource.cbv_descriptor->GetGpuHandle());  //CbMesh

		// 頂点バッファ設定
		rc.d3d_command_list->IASetVertexBuffers(0, 1, mesh.bones.empty() ? &mesh.mesh->d3d_vbv : &frame_resource.d3d_vbv);
		rc.d3d_command_list->IASetIndexBuffer(&res_mesh->d3d_ibv);
		rc.d3d_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// サブセット
		const ModelResource::Material* material = res_mesh->material;

		// マテリアル定数バッファ設定
		rc.d3d_command_list->SetGraphicsRootDescriptorTable(2, material->cbv_descriptor->GetGpuHandle());  //CbMaterial

		// シェーダーリソースビュー設定
		rc.d3d_command_list->SetGraphicsRootDescriptorTable(3, rc.skydomeData.skyTexture->GetGpuHandle());  //Skydomeテクスチャ

		// 描画
		rc.d3d_command_list->DrawIndexedInstanced(static_cast<UINT>(res_mesh->indices.size()), 1, 0, 0, 0);
	}
}
//! @file LambertShader.cpp
//! @note 

#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/Graphics.h"
#include "TAKOEngine/Rendering/Shaders/LambertShader.h"

//***********************************************************
// @brief       コンストラクタ
// @param[in]   なし
// @return      なし
//***********************************************************
LambertShader::LambertShader(ID3D12Device* device, bool instancing)
{
	Graphics&   graphics = Graphics::Instance();
	const RenderStateDX12* renderState = graphics.GetRenderStateDX12();
	
	HRESULT hr = S_OK;

	// シェーダー
	std::vector<BYTE> vsData, psData;
	{
		if (!instancing) GpuResourceUtils::LoadShaderFile("Data/Shader/LambertDX12VS.cso", vsData);
		else GpuResourceUtils::LoadShaderFile("Data/Shader/LambertInstancingVS.cso", vsData);
		GpuResourceUtils::LoadShaderFile("Data/Shader/LambertDX12PS.cso", psData);
	}

	// ルートシグネチャの生成
	{
		hr = device->CreateRootSignature(
			0,
			vsData.data(),
			vsData.size(),
			IID_PPV_ARGS(m_d3d_root_signature.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_root_signature->SetName(L"LambertShaderRootSignature");
	}

	// パイプラインステートの生成
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC d3d_graphics_pipeline_state_desc = {};

		// ルートシグネチャ
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
		d3d_graphics_pipeline_state_desc.InputLayout.NumElements = _countof(inputElementDescs);

		// ブレンドステート
		/*d3d_graphics_pipeline_state_desc.BlendState.AlphaToCoverageEnable = false;
		d3d_graphics_pipeline_state_desc.BlendState.IndependentBlendEnable = false;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].BlendEnable = true;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].LogicOpEnable = false;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;*/
		d3d_graphics_pipeline_state_desc.BlendState = renderState->GetBlendState(BlendState::Opaque);

		// 深度ステンシルステート
		/*d3d_graphics_pipeline_state_desc.DepthStencilState.DepthEnable = true;
		d3d_graphics_pipeline_state_desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		d3d_graphics_pipeline_state_desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		d3d_graphics_pipeline_state_desc.DepthStencilState.StencilEnable = false;*/
		d3d_graphics_pipeline_state_desc.DepthStencilState = renderState->GetDepthState(DepthState::TestAndWrite);

		// ラスタライザーステート
		/*d3d_graphics_pipeline_state_desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		d3d_graphics_pipeline_state_desc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		d3d_graphics_pipeline_state_desc.RasterizerState.FrontCounterClockwise = true;
		d3d_graphics_pipeline_state_desc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		d3d_graphics_pipeline_state_desc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		d3d_graphics_pipeline_state_desc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		d3d_graphics_pipeline_state_desc.RasterizerState.DepthClipEnable = true;
		d3d_graphics_pipeline_state_desc.RasterizerState.MultisampleEnable = false;
		d3d_graphics_pipeline_state_desc.RasterizerState.AntialiasedLineEnable = false;
		d3d_graphics_pipeline_state_desc.RasterizerState.ForcedSampleCount = 0;
		d3d_graphics_pipeline_state_desc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;*/
		d3d_graphics_pipeline_state_desc.RasterizerState = renderState->GetRasterizer(RasterizerState::SolidCullBack);

		// プリミティブトポロジー
		d3d_graphics_pipeline_state_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// ストリップ時のカット値
		d3d_graphics_pipeline_state_desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		// レンダーターゲット数
		d3d_graphics_pipeline_state_desc.NumRenderTargets = 1;
		d3d_graphics_pipeline_state_desc.RTVFormats[0] = RenderTargetFormat;
		d3d_graphics_pipeline_state_desc.DSVFormat = DepthStencilFormat;

		// マルチサンプリング
		d3d_graphics_pipeline_state_desc.SampleDesc.Count = 1;
		d3d_graphics_pipeline_state_desc.SampleDesc.Quality = 0;

		// アダプタ
		d3d_graphics_pipeline_state_desc.NodeMask = 0;
		d3d_graphics_pipeline_state_desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		// パイプラインステート生成
		hr = device->CreateGraphicsPipelineState(
			&d3d_graphics_pipeline_state_desc,
			IID_PPV_ARGS(m_d3d_pipeline_state.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_pipeline_state->SetName(L"LambertShaderPipelineState");
	}

	//サンプラーステート設定
	m_sampler = graphics.GetSampler(SamplerState::LinearWrap);
}

//***********************************************************
// @brief       デストラクタ
// @param[in]   なし
// @return      なし
//***********************************************************
LambertShader::~LambertShader()
{
}

//***********************************************************
// @brief       描画
// @param[in]   rc     レンダーコンテキスト
// @param[in]   model  描画対象のモデルデータを指すポインタ
// @return      なし
//***********************************************************
void LambertShader::Render(const RenderContextDX12& rc, ModelDX12* model)
{
	Graphics& graphics = Graphics::Instance();

	// カメラに写っている範囲のオブジェクトをフラグでマークする配列を用意
	std::vector<bool> visibleObjects(model->GetMeshes().size(), false);

	// 視錐台カリングを実行して可視オブジェクトをマーク
	FrustumCulling::FrustumCullingFlag(Camera::Instance(), model->GetMeshes(), visibleObjects);
	int culling = 0;

	// パイプライン設定
	rc.d3d_command_list->SetGraphicsRootSignature(m_d3d_root_signature.Get());
	rc.d3d_command_list->SetPipelineState(m_d3d_pipeline_state.Get());

	// シーン定数バッファ設定
	rc.d3d_command_list->SetGraphicsRootDescriptorTable(0, rc.scene_cbv_descriptor->GetGpuHandle());

	for (const ModelDX12::Mesh& mesh : model->GetMeshes())
	{
		if (!visibleObjects[culling++]) continue;

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
		
		//サンプラーステート設定
		rc.d3d_command_list->SetGraphicsRootDescriptorTable(4, m_sampler->GetDescriptor()->GetGpuHandle());

		// 描画
		if (frame_resource.instancingCount == 0)
		{
			rc.d3d_command_list->DrawIndexedInstanced(static_cast<UINT>(res_mesh->indices.size()), 1, 0, 0, 0);
		}
		else
		{
			//インスタンシング
			rc.d3d_command_list->DrawIndexedInstanced(static_cast<UINT>(res_mesh->indices.size()), frame_resource.instancingCount, 0, 0, 0);
		}
	}
}

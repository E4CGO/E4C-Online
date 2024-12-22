//! @file ShadowMapShader.cpp
//! @note 

#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/Graphics.h"

#include "TAKOEngine/Rendering/Shaders/ShadowMapShader.h"

//********************************************************
// @brief       描画開始
// @param[in]   rc  レンダーコンテキスト
// @return      なし
//********************************************************
void ShadowMapShader::Begin(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シェーダー設定
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
	dc->IASetInputLayout(inputLayout.Get());

	// 定数バッファ設定
	ID3D11Buffer* constantBuffers[] =
	{
		sceneConstantBuffer.Get(),
		nullptr,
		skeletonConstantBuffer.Get(),
		nullptr
	};
	dc->VSSetConstantBuffers(0, _countof(constantBuffers), constantBuffers);

	//レンダーステート設定
	SetRenderState(rc);

	// シーン用定数バッファ更新
	CbScene cbScene{};
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.shadowMapData.view);
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.shadowMapData.projection);
	DirectX::XMStoreFloat4x4(&cbScene.lightViewProjection, V * P);
	dc->UpdateSubresource(sceneConstantBuffer.Get(), 0, 0, &cbScene, 0, 0);
}

//********************************************************
// @brief       描画
// @param[in]   rc     レンダーコンテキスト
// @param[in]   model  モデルデータ
// @param[in]   color  色
// @return      なし
//********************************************************
void ShadowMapShader::Draw(const RenderContext& rc, const iModel* model, DirectX::XMFLOAT4 color)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	const ModelResource* resource = model->GetResource();
	const std::vector<iModel::Node>& nodes = model->GetNodes();

	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		// 頂点バッファ設定
		UINT stride = sizeof(ModelResource::Vertex);
		UINT offset = 0;
		dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// スケルトン用定数バッファ更新
		CbSkeleton cbSkeleton{};
		if (mesh.bones.size() > 0)
		{
			for (size_t i = 0; i < mesh.bones.size(); ++i)
			{
				DirectX::XMMATRIX worldTransform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.bones.at(i).nodeIndex).worldTransform);
				DirectX::XMMATRIX offsetTransform = DirectX::XMLoadFloat4x4(&mesh.bones.at(i).offsetTransform);
				DirectX::XMMATRIX boneTransform = offsetTransform * worldTransform;
				DirectX::XMStoreFloat4x4(&cbSkeleton.boneTransforms[i], boneTransform);
			}
		}
		else
		{
			cbSkeleton.boneTransforms[0] = nodes.at(mesh.nodeIndex).worldTransform;
		}
		rc.deviceContext->UpdateSubresource(skeletonConstantBuffer.Get(), 0, 0, &cbSkeleton, 0, 0);

		// 描画
		dc->DrawIndexed(static_cast<UINT>(mesh.indices.size()), 0, 0);
	}
}

//********************************************************
// @brief       レンダーステート設定
// @param[in]   rc  レンダーコンテキスト
// @return      なし
//********************************************************
void ShadowMapShader::SetRenderState(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// レンダーステート設定
	const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Transparency), blend_factor, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(nullptr, 0);
	dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullBack));
}

//***********************************************************
// @brief       コンストラクタ
// @param[in]   device　    ID3D12Device*
// @return      なし
//***********************************************************
ShadowMapShaderDX12::ShadowMapShaderDX12(ID3D12Device* device)
{
	Graphics&                 graphics = Graphics::Instance();
	const RenderStateDX12* renderState = graphics.GetRenderStateDX12();

	HRESULT hr = S_OK;

	// シェーダー
	std::vector<BYTE> vsData;
	GpuResourceUtils::LoadShaderFile("Data/Shader/ShadowMapDX12VS.cso", vsData);

	// ルートシグネクチャの生成
	{
		hr = device->CreateRootSignature(
			0,
			vsData.data(),
			vsData.size(),
			IID_PPV_ARGS(m_d3d_root_signature.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_root_signature->SetName(L"ShadowShaderRootSignature");
	}

	// パイプラインステートの生成
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline_state_desc = {};

		// ルートシグネクチャ
		pipeline_state_desc.pRootSignature = m_d3d_root_signature.Get();

		pipeline_state_desc.VS.pShaderBytecode = vsData.data();
		pipeline_state_desc.VS.BytecodeLength  = vsData.size();

		// 入力レイアウト
		D3D12_INPUT_ELEMENT_DESC d3d_input_element_descs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
		pipeline_state_desc.InputLayout.pInputElementDescs = d3d_input_element_descs;
		pipeline_state_desc.InputLayout.NumElements = _countof(d3d_input_element_descs);

		//ブレンドステート
		pipeline_state_desc.BlendState = renderState->GetBlendState(BlendState::Opaque);

		//深度ステンシルステート
		pipeline_state_desc.DepthStencilState = renderState->GetDepthState(DepthState::TestAndWrite);

		//ラスタライザーステート
		pipeline_state_desc.RasterizerState.FillMode              = D3D12_FILL_MODE_SOLID;
		pipeline_state_desc.RasterizerState.CullMode              = D3D12_CULL_MODE_BACK;
		pipeline_state_desc.RasterizerState.FrontCounterClockwise = false;
		pipeline_state_desc.RasterizerState.DepthBias             = D3D12_DEFAULT_DEPTH_BIAS;
		pipeline_state_desc.RasterizerState.DepthBiasClamp        = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		pipeline_state_desc.RasterizerState.SlopeScaledDepthBias  = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		pipeline_state_desc.RasterizerState.DepthClipEnable       = true;
		pipeline_state_desc.RasterizerState.MultisampleEnable     = false;
		pipeline_state_desc.RasterizerState.AntialiasedLineEnable = false;
		pipeline_state_desc.RasterizerState.ForcedSampleCount     = 0;
		pipeline_state_desc.RasterizerState.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		//プリミティブトポロジー
		pipeline_state_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		//ストリップス時のカット値
		pipeline_state_desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		//レンダーターゲット数
		pipeline_state_desc.NumRenderTargets = 0;
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
		m_d3d_pipeline_state->SetName(L"ShadowShaderPipelineState");
	}
}

//***********************************************************
// @brief       描画
// @param[in]   rc     レンダーコンテキスト
// @param[in]   mesh   描画対象のモデルデータのmesh
// @return      なし
//***********************************************************
void ShadowMapShaderDX12::Render(const RenderContextDX12& rc, const ModelDX12::Mesh& mesh)
{
	Graphics& graphics = Graphics::Instance();

	// パイプライン設定
	rc.d3d_command_list->SetGraphicsRootSignature(m_d3d_root_signature.Get());
	rc.d3d_command_list->SetPipelineState(m_d3d_pipeline_state.Get());

	// シーン定数バッファ設定
	rc.d3d_command_list->SetGraphicsRootDescriptorTable(0, rc.scene_cbv_descriptor->GetGpuHandle());  //CbScene

	const ModelResource::Mesh* res_mesh = mesh.mesh;
	const ModelDX12::Mesh::FrameResource& frame_resource = mesh.frame_resources.at(graphics.GetCurrentBufferIndex());

	// メッシュ定数バッファ設定
	rc.d3d_command_list->SetGraphicsRootDescriptorTable(1, frame_resource.cbv_descriptor->GetGpuHandle());  //CbMesh

	// 頂点バッファ設定
	rc.d3d_command_list->IASetVertexBuffers(0, 1, mesh.bones.empty() ? &mesh.mesh->d3d_vbv : &frame_resource.d3d_vbv);
	rc.d3d_command_list->IASetIndexBuffer(&res_mesh->d3d_ibv);
	rc.d3d_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 描画
	rc.d3d_command_list->DrawIndexedInstanced(static_cast<UINT>(res_mesh->indices.size()), 1, 0, 0, 0);
}

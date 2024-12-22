//! @file LambertShader.cpp
//! @note

#include "TAKOEngine/Rendering/Shaders/LambertShader.h"

#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/Graphics.h"
#include "TAKOEngine/Editor/Camera/CameraManager.h"

/**************************************************************************//**
// @brief       コンストラクタ
// @param[in]   device　    ID3D11Device*
*//***************************************************************************/
LambertShader::LambertShader(ID3D11Device* device) : ModelShader()
{
	//// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
			{ "POSITION", 0,DXGI_FORMAT_R32G32B32_FLOAT , 0, 0, D3D11_INPUT_PER_VERTEX_DATA},
			{ "NORMAL", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0,DXGI_FORMAT_R32G32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "JOINTS", 0,  DXGI_FORMAT_R16G16B16A16_UINT, 4, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 5, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// 頂点シェーダー
	GpuResourceUtils::LoadVertexShader(
		device,
		VertexShaderName,
		inputElementDesc,
		_countof(inputElementDesc),
		m_InputLayout.GetAddressOf(),
		m_VertexShader.GetAddressOf());

	// ピクセルシェーダー
	if (PixelShaderName)
	{
		GpuResourceUtils::LoadPixelShader(
			device,
			PixelShaderName,
			m_PixelShader.GetAddressOf());
	}

	// シーン用定数バッファ
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(::CbScene),
		this->m_sceneConstantBuffer.GetAddressOf());

	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(primitive_constants),
		this->m_PrimitiveConstantBuffer.GetAddressOf());

	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(primitive_joint_constants),
		this->m_PrimitiveJointConstantBuffer.GetAddressOf());
}

//******************************************************************
// @brief       描画開始
// @param[in]   rc  レンダーコンテキスト
// @return      なし
//******************************************************************
void LambertShader::Begin(const RenderContext& rc)
{
	ID3D11DeviceContext* immediate_context = rc.deviceContext;

	//// シェーダー設定
	immediate_context->IASetInputLayout(m_InputLayout.Get());
	immediate_context->VSSetShader(m_VertexShader.Get(), nullptr, 0);
	immediate_context->PSSetShader(m_PixelShader.Get(), nullptr, 0);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//// 定数バッファ設定
	ID3D11Buffer* constantBuffers[] =
	{
		m_sceneConstantBuffer.Get(),
	};
	immediate_context->VSSetConstantBuffers(1, _countof(constantBuffers), constantBuffers);
	immediate_context->PSSetConstantBuffers(1, _countof(constantBuffers), constantBuffers);

	// サンプラステート
	ID3D11SamplerState* samplerStates[] =
	{
		rc.renderState->GetSamplerState(SamplerState::PointWrap),
		rc.renderState->GetSamplerState(SamplerState::LinearWrap),
		rc.renderState->GetSamplerState(SamplerState::AnisotropicWrap),
	};
	immediate_context->PSSetSamplers(0, _countof(samplerStates), samplerStates);

	//// レンダーステート設定
	SetRenderState(rc);

	//// シーン用定数バッファ更新

	::CbScene cbscene;

	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.camera->GetView());
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.camera->GetProjection());
	DirectX::XMStoreFloat4x4(&cbscene.view, V);
	DirectX::XMStoreFloat4x4(&cbscene.projection, P);

	const DirectX::XMFLOAT3& eye = rc.camera->GetEye();
	cbscene.camera_position.x = eye.x;
	cbscene.camera_position.y = eye.y;
	cbscene.camera_position.z = eye.z;

	cbscene.ambientLightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	cbscene.directionalLightData = rc.directionalLightData;

	memcpy_s(
		cbscene.pointLightData,
		sizeof(cbscene.pointLightData),
		rc.pointLightData,
		sizeof(rc.pointLightData)
	);
	cbscene.pointLightCount = rc.pointLightCount;
	memcpy_s(
		cbscene.spotLightData,
		sizeof(cbscene.spotLightData),
		rc.spotLightData,
		sizeof(rc.spotLightData)
	);
	cbscene.spotLightCount = rc.spotLightCount;

	immediate_context->UpdateSubresource(m_sceneConstantBuffer.Get(), 0, 0, &cbscene, 0, 0);
}

//******************************************************************
// @brief       描画終了
// @param[in]   rc     レンダーコンテキスト
// @return      なし
//******************************************************************
void LambertShader::End(const RenderContext& rc)
{
}

//******************************************************************
// @brief       モデル描画
// @param[in]   rc     レンダーコンテキスト
// @param[in]   model  描画対象のモデルデータを指すポインタ
// @param[in]   color  マテリアルカラー
// @return      なし
//******************************************************************
void LambertShader::Draw(const RenderContext& rc, const iModel* model, DirectX::XMFLOAT4 color)
{
	ID3D11DeviceContext* immediate_context = rc.deviceContext;

	std::function<void(int)> traverse{ [&](int node_index)->void {
		const ModelResource::node& node {model->gltf_nodes.at(node_index)};
		if (node.skin > -1)
		{
			const ModelResource::skin& skin{ model->skins.at(node.skin)};
			primitive_joint_constants primitive_joint_data{};
			for (size_t joint_index = 0; joint_index < skin.joints.size(); ++joint_index)
			{
				XMStoreFloat4x4(&primitive_joint_data.matrices[joint_index],
					XMLoadFloat4x4(&skin.inverse_bind_matrices.at(joint_index)) *
					XMLoadFloat4x4(&model->gltf_nodes.at(skin.joints.at(joint_index)).global_transform) *
					XMMatrixInverse(NULL, XMLoadFloat4x4(&node.global_transform))
				);
			}
			immediate_context->UpdateSubresource(m_PrimitiveJointConstantBuffer.Get(), 0, 0, &primitive_joint_data, 0, 0);
		}

		immediate_context->VSSetConstantBuffers(2, 1, m_PrimitiveJointConstantBuffer.GetAddressOf());
		immediate_context->PSSetConstantBuffers(2, 1, m_PrimitiveJointConstantBuffer.GetAddressOf());

		if (node.mesh > -1)
		{
			const ModelResource::mesh& mesh{ model->meshes.at(node.mesh)};

			//FrustumCulling::FrustumCullingFlag(model->meshes, visibleObjects);
			//int culling = 0;

			//for (const ModelResource::Mesh& mesh : model->meshes)
			//{
			//	if (!visibleObjects[culling++]) continue;
			//}

			for (std::vector<ModelResource::mesh::primitive>::const_reference primitive : mesh.primitives)
			{
				ID3D11Buffer* vertex_buffers[]
				{
					primitive.vertex_buffer_views.at("POSITION").buffer.Get(),
					primitive.vertex_buffer_views.at("NORMAL").buffer.Get(),
					primitive.vertex_buffer_views.at("TANGENT").buffer.Get(),
					primitive.vertex_buffer_views.at("TEXCOORD_0").buffer.Get(),
					primitive.vertex_buffer_views.at("JOINTS_0").buffer.Get(),
					primitive.vertex_buffer_views.at("WEIGHTS_0").buffer.Get(),
				};
				UINT strides[]{
					static_cast<UINT>(primitive.vertex_buffer_views.at("POSITION").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("NORMAL").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("TANGENT").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("TEXCOORD_0").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("JOINTS_0").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("WEIGHTS_0").stride_in_bytes),
				};
				UINT offsets[_countof(vertex_buffers)]{ 0 };
				immediate_context->IASetVertexBuffers(0, _countof(vertex_buffers), vertex_buffers, strides, offsets);
				immediate_context->IASetIndexBuffer(primitive.index_buffer_view.buffer.Get(), primitive.index_buffer_view.format, 0);

				primitive_constants primitive_data{};
				primitive_data.material = primitive.material;
				primitive_data.has_tangent = primitive.vertex_buffer_views.at("TANGENT").buffer != NULL;
				primitive_data.skin = node.skin;

				DirectX::XMFLOAT4X4 world = model->GetWorldMatrix();

				DirectX::XMStoreFloat4x4(&primitive_data.world, DirectX::XMLoadFloat4x4(&node.global_transform) * DirectX::XMLoadFloat4x4(&world));

				//model->SetWorldMatrix(world);

				immediate_context->UpdateSubresource(m_PrimitiveConstantBuffer.Get(), 0, 0, &primitive_data, 0, 0);
				immediate_context->VSSetConstantBuffers(0, 1, m_PrimitiveConstantBuffer.GetAddressOf());
				immediate_context->PSSetConstantBuffers(0, 1, m_PrimitiveConstantBuffer.GetAddressOf());

				immediate_context->PSSetShaderResources(0, 1, model->material_resource_view.GetAddressOf());

				const ModelResource::material& material{ model->materials.at(primitive.material) };
				const int texture_indices[]
				{
					material.data.pbr_metallic_roughness.basecolor_texture.index,
					material.data.pbr_metallic_roughness.metallic_roughness_texture.index,
					material.data.normal_texture.index,
					material.data.emissive_texture.index,
					material.data.occlusion_texture.index,
				};
				ID3D11ShaderResourceView* null_shader_resource_view{};
				std::vector<ID3D11ShaderResourceView*> shader_resource_views(_countof(texture_indices));
				for (int texture_index = 0; texture_index < shader_resource_views.size(); ++texture_index)
				{
					shader_resource_views.at(texture_index) = texture_indices[texture_index] > -1 ?
						model->texture_resource_views.at(model->textures.at(texture_indices[texture_index]).source).Get() :
						null_shader_resource_view;
				}
				immediate_context->PSSetShaderResources(1, static_cast<UINT>(shader_resource_views.size()), shader_resource_views.data());

				immediate_context->DrawIndexed(static_cast<UINT>(primitive.index_buffer_view.count()), 0, 0);
			}
		}

		for (std::vector<int>::value_type child_index : node.children)
		{
			traverse(child_index);
		}
		} };

	for (std::vector<int>::value_type node_index : model->scenes.at(0).nodes)
	{
		traverse(node_index);
	}
}

/**************************************************************************//**
	@brief
	@param[in]    rc
*//***************************************************************************/
void LambertShader::SetRenderState(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// レンダーステート設定
	const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Opaque), blend_factor, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullBackCCW));
}

void LambertShader::SetShaderResourceView(const ModelResource::Mesh& mesh, ID3D11DeviceContext*& dc)
{
}

//***********************************************************
// @brief       コンストラクタ
// @param[in]   device　    ID3D12Device*
// @param[in]   instancing インスタンシング　true : あり, false : なし
// @return      なし
//***********************************************************
LambertShaderDX12::LambertShaderDX12(ID3D12Device* device, bool instancing)
{
	Graphics& graphics = Graphics::Instance();
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
		d3d_graphics_pipeline_state_desc.VS.BytecodeLength = vsData.size();
		d3d_graphics_pipeline_state_desc.PS.pShaderBytecode = psData.data();
		d3d_graphics_pipeline_state_desc.PS.BytecodeLength = psData.size();

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
	m_sampler = graphics.GetSampler(SamplerState::AnisotropicWrap);
}

//***********************************************************
// @brief       デストラクタ
// @param[in]   なし
// @return      なし
//***********************************************************
LambertShaderDX12::~LambertShaderDX12()
{
}

//***********************************************************
// @brief       描画
// @param[in]   rc     レンダーコンテキスト
// @param[in]   mesh   描画対象のモデルデータのmesh
// @return      なし
//***********************************************************
void LambertShaderDX12::Render(const RenderContextDX12& rc, const ModelDX12::Mesh& mesh)
{
	Graphics& graphics = Graphics::Instance();

	// パイプライン設定
	rc.d3d_command_list->SetGraphicsRootSignature(m_d3d_root_signature.Get());
	rc.d3d_command_list->SetPipelineState(m_d3d_pipeline_state.Get());

	// シーン定数バッファ設定
	rc.d3d_command_list->SetGraphicsRootDescriptorTable(0, rc.scene_cbv_descriptor->GetGpuHandle());

	// シャドウマップ設定
	rc.d3d_command_list->SetGraphicsRootDescriptorTable(5, rc.shadowMap.shadow_srv_descriptor->GetGpuHandle());
	rc.d3d_command_list->SetGraphicsRootDescriptorTable(6, rc.shadowMap.shadow_sampler_descriptor->GetGpuHandle());

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
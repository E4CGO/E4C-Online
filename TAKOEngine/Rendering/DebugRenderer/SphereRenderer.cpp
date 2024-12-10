//! @file SphereRenderer.cpp
//! @note

#include <stdio.h>
#include <memory>

#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/Graphics.h"
#include "TAKOEngine/Rendering/GpuResourceUtils.h"

#include "SphereRenderer.h"

//*******************************************************
// @brief     コンストラクタ
// @param[in] device  ID3D12Device*
// @return    なし
//*******************************************************
SphereRenderer::SphereRenderer(ID3D12Device* device, UINT sphere_count) : m_count(sphere_count)
{
	Graphics& graphics = Graphics::Instance();
	const RenderStateDX12* renderState = graphics.GetRenderStateDX12();

	HRESULT hr = S_OK;

	// シェーダー
	std::vector<BYTE> vsData, psData;
	{
		GpuResourceUtils::LoadShaderFile("Data/Shader/DebugDX12VS.cso", vsData);
		GpuResourceUtils::LoadShaderFile("Data/Shader/DebugDX12PS.cso", psData);
	}

	// ルートシグネクチャの生成
	{
		hr = device->CreateRootSignature(
			0,
			vsData.data(),
			vsData.size(),
			IID_PPV_ARGS(m_d3d_root_signature.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_root_signature->SetName(L"DebugRendererRootSignature");
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
		};
		d3d_graphics_pipeline_state_desc.InputLayout.pInputElementDescs = inputElementDescs;
		d3d_graphics_pipeline_state_desc.InputLayout.NumElements = _countof(inputElementDescs);

		// ブレンドステート
		d3d_graphics_pipeline_state_desc.BlendState = renderState->GetBlendState(BlendState::Opaque);

		// 深度ステンシルステート
		d3d_graphics_pipeline_state_desc.DepthStencilState = renderState->GetDepthState(DepthState::TestAndWrite);

		// ラスタライザーステート
		d3d_graphics_pipeline_state_desc.RasterizerState = renderState->GetRasterizer(RasterizerState::SolidCullNone);

		// プリミティブトポロジー
		d3d_graphics_pipeline_state_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

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
		m_d3d_pipeline_state->SetName(L"DebugRendererPipelineState");
	}

	// コンスタントバッファ作成
	CreateConstantBuffer(device);

	// フレームリソース生成
	CreateFrameResource(device, 1.0f, 16, 16);
}

//*******************************************************
// @brief     デストラクタ
// @param[in] なし
// @return    なし
//*******************************************************
SphereRenderer::~SphereRenderer()
{
	for (FrameResource& frame_resource : m_frame_resources)
	{
		if (frame_resource.vertex_data != nullptr)
		{
			frame_resource.d3d_vb_resource->Unmap(0, nullptr);
		}
		if (frame_resource.cb_mesh_data != nullptr)
		{
			frame_resource.d3d_cbv_resource->Unmap(0, nullptr);
		}
		if (frame_resource.cbv_descriptor != nullptr)
		{
			Graphics::Instance().GetShaderResourceDescriptorHeap()->PushDescriptor(frame_resource.cbv_descriptor);
		}
	}
}

//***********************************************************
// @brief       描画
// @param[in]   rc      レンダーコンテキスト
// @return      なし
//***********************************************************
void SphereRenderer::Render(const RenderContextDX12& rc)
{
	Graphics& graphics = Graphics::Instance();
	const Camera* camera = CameraManager::Instance().GetCamera(); 

	FrameResource& frame_resource = m_frame_resources.at(graphics.GetCurrentBufferIndex());

	// ビュープロジェクション行列作成
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&camera->GetView());
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&camera->GetProjection());
	DirectX::XMMATRIX viewProjection = V * P;

	// ワールドビュープロジェクション行列作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(m_radius, m_radius, m_radius);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(m_center.x, m_center.y, m_center.z);
	DirectX::XMMATRIX W = S * T;
	DirectX::XMMATRIX worldViewProjection = W * viewProjection;

	DirectX::XMStoreFloat4x4(&frame_resource.cb_mesh_data->worldViewProjection, worldViewProjection);
	frame_resource.cb_mesh_data->color = m_color;

	// 球メッシュ作成
	CreateSphereMesh(1.0f, 16, 16);

	// パイプライン設定
	rc.d3d_command_list->SetGraphicsRootSignature(m_d3d_root_signature.Get());
	rc.d3d_command_list->SetPipelineState(m_d3d_pipeline_state.Get());

	// メッシュ定数バッファ設定
	rc.d3d_command_list->SetGraphicsRootDescriptorTable(0, frame_resource.cbv_descriptor->GetGpuHandle());

	// プリミティブ設定
	rc.d3d_command_list->IASetVertexBuffers(0, 1, &frame_resource.d3d_vbv);
	rc.d3d_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	// 描画
	rc.d3d_command_list->DrawInstanced(vertexCount, 1, 0, 0);
}

//*******************************************************
// @brief     コンスタントバッファ作成
// @param[in] device  ID3D12Device*
// @return    なし
//*******************************************************
void SphereRenderer::CreateConstantBuffer(ID3D12Device* device)
{
	Graphics& graphics = Graphics::Instance();

	// フレームリソース
	m_frame_resources.resize(graphics.GetBufferCount());
	for (FrameResource& frame_resource : m_frame_resources)
	{
		D3D12_HEAP_PROPERTIES d3d_heap_props{};
		d3d_heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
		d3d_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		d3d_heap_props.CreationNodeMask = 1;
		d3d_heap_props.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC d3d_resource_desc{};
		d3d_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		d3d_resource_desc.Alignment = 0;
		d3d_resource_desc.Width = ((sizeof(CbMesh)) + 255) & ~255;
		d3d_resource_desc.Height = 1;
		d3d_resource_desc.DepthOrArraySize = 1;
		d3d_resource_desc.MipLevels = 1;
		d3d_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
		d3d_resource_desc.SampleDesc.Count = 1;
		d3d_resource_desc.SampleDesc.Quality = 0;
		d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		d3d_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		HRESULT hr = device->CreateCommittedResource(
			&d3d_heap_props,
			D3D12_HEAP_FLAG_NONE,
			&d3d_resource_desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(frame_resource.d3d_cbv_resource.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		frame_resource.d3d_cbv_resource->SetName(L"SphereMeshConstantBuffer");

		frame_resource.cbv_descriptor = Graphics::Instance().GetShaderResourceDescriptorHeap()->PopDescriptor();

		D3D12_CONSTANT_BUFFER_VIEW_DESC d3d_cbv_desc;
		d3d_cbv_desc.BufferLocation = frame_resource.d3d_cbv_resource->GetGPUVirtualAddress();
		d3d_cbv_desc.SizeInBytes = static_cast<UINT>(d3d_resource_desc.Width);

		device->CreateConstantBufferView(
			&d3d_cbv_desc,
			frame_resource.cbv_descriptor->GetCpuHandle());

		hr = frame_resource.d3d_cbv_resource->Map(0, nullptr, reinterpret_cast<void**>(&frame_resource.cb_mesh_data));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

//*******************************************************
// @brief     フレームリソース生成
// @param[in] device   ID3D12Device*
// @param[in] radius   半径
// @param[in] slices   スライス
// @param[in] stacks   スタック
// @return    なし
//*******************************************************
void SphereRenderer::CreateFrameResource(ID3D12Device* device, float radius, int slices, int stacks)
{
	Graphics& graphics = Graphics::Instance();

	vertexCount = stacks * slices * 2 + slices * stacks * 2;

	// 頂点バッファ作成
	m_frame_resources.resize(graphics.GetBufferCount());
	for (FrameResource& frame_resource : m_frame_resources)
	{
		D3D12_HEAP_PROPERTIES d3d_heap_props{};
		d3d_heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
		d3d_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		d3d_heap_props.CreationNodeMask = 1;
		d3d_heap_props.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC d3d_resource_desc{};
		d3d_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		d3d_resource_desc.Alignment = 0;
		d3d_resource_desc.Width = sizeof(Vertex) * vertexCount * m_count;
		d3d_resource_desc.Height = 1;
		d3d_resource_desc.DepthOrArraySize = 1;
		d3d_resource_desc.MipLevels = 1;
		d3d_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
		d3d_resource_desc.SampleDesc.Count = 1;
		d3d_resource_desc.SampleDesc.Quality = 0;
		d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		d3d_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		// リソース生成
		HRESULT hr = device->CreateCommittedResource(
			&d3d_heap_props,
			D3D12_HEAP_FLAG_NONE,
			&d3d_resource_desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(frame_resource.d3d_vb_resource.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		frame_resource.d3d_vb_resource->SetName(L"SphereVertexBuffer");

		// 頂点バッファビュー設定
		frame_resource.d3d_vbv.BufferLocation = frame_resource.d3d_vb_resource->GetGPUVirtualAddress();
		frame_resource.d3d_vbv.SizeInBytes = static_cast<UINT>(d3d_resource_desc.Width);
		frame_resource.d3d_vbv.StrideInBytes = sizeof(Vertex);

		// マップする
		frame_resource.d3d_vb_resource->Map(0, nullptr, reinterpret_cast<void**>(&frame_resource.vertex_data));
	}
}

//*******************************************************
// @brief     球メッシュ作成
// @param[in] device   ID3D12Device*
// @param[in] radius   半径
// @param[in] slices   スライス
// @param[in] stacks   スタック
// @return    なし
//*******************************************************
void SphereRenderer::CreateSphereMesh(float radius, int slices, int stacks)
{
	Graphics& graphics = Graphics::Instance();

	if (m_index >= m_count) return;

	// 頂点バッファ作成
	FrameResource& frame_resource = m_frame_resources.at(graphics.GetCurrentBufferIndex());
	Vertex* p = frame_resource.vertex_data + (m_index * vertexCount);

	float phiStep = DirectX::XM_PI / stacks;     // 緯度方向のステップ
	float thetaStep = DirectX::XM_2PI / slices;  // 経度方向のステップ

	for (int i = 0; i < stacks; ++i)  // 頂点の生成 (緯度方向)
	{
		float phi = i * phiStep;
		float y = radius * cosf(phi);
		float r = radius * sinf(phi);

		for (int j = 0; j < slices; ++j) // 頂点の生成 (経度方向)
		{
			float theta = j * thetaStep;
			p->position.x = r * sinf(theta);
			p->position.y = y;
			p->position.z = r * cosf(theta);
			p++;

			theta += thetaStep;

			p->position.x = r * sinf(theta);
			p->position.y = y;
			p->position.z = r * cosf(theta);
			p++;
		}
	}

	thetaStep = DirectX::XM_2PI / stacks;
	for (int i = 0; i < slices; ++i)
	{
		DirectX::XMMATRIX M = DirectX::XMMatrixRotationY(i * thetaStep);
		for (int j = 0; j < stacks; ++j)
		{
			float theta = j * thetaStep;
			DirectX::XMVECTOR V1 = DirectX::XMVectorSet(radius * sinf(theta), radius * cosf(theta), 0.0f, 1.0f);
			DirectX::XMVECTOR P1 = DirectX::XMVector3TransformCoord(V1, M);
			DirectX::XMStoreFloat3(&p->position, P1);
			p++;

			int n = (j + 1) % stacks;
			theta += thetaStep;

			DirectX::XMVECTOR V2 = DirectX::XMVectorSet(radius * sinf(theta), radius * cosf(theta), 0.0f, 1.0f);
			DirectX::XMVECTOR P2 = DirectX::XMVector3TransformCoord(V2, M);
			DirectX::XMStoreFloat3(&p->position, P2);
			p++;
		}
	}
	m_index++;
}

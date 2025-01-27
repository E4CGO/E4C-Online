//! @file LineRenderer.cpp
//! @note

#include <stdio.h>
#include <memory>

#include "Misc.h"
#include "GpuResourceUtils.h"
#include "Graphics.h"
#include "RenderState.h"

#include "LineRenderer.h"

//**************************************************************
// @brief       コンストラクタ
// @param[in]   device        ID3D11Device*
// @param[in]   vertexCount   頂点数
// @return      なし
//**************************************************************
LineRenderer::LineRenderer(ID3D11Device* device, UINT vertexCount)
	: capacity(vertexCount)
{
	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"TEXCOORD",  0,DXGI_FORMAT_R32G32_FLOAT,        0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	// 頂点シェーダー
	GpuResourceUtils::LoadVertexShader(
		device,
		"Data/Shader/LineVS.cso",
		inputElementDesc,
		_countof(inputElementDesc),
		inputLayout.GetAddressOf(),
		vertexShader.GetAddressOf()
	);

	// ピクセルシェーダー
	GpuResourceUtils::LoadPixelShader(
		device,
		"Data/Shader/LinePS.cso",
		pixelShader.GetAddressOf()
	);

	// 定数バッファ
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(ConstantBuffer),
		constantBuffer.GetAddressOf()
	);

	// ブレンドステート
	{
		D3D11_BLEND_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = device->CreateBlendState(&desc, blendState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	}

	// 深度ステンシルステート
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		HRESULT hr = device->CreateDepthStencilState(&desc, depthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// ラスタライザーステート
	{
		D3D11_RASTERIZER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.FrontCounterClockwise = true;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = true;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.AntialiasedLineEnable = false; 

		HRESULT hr = device->CreateRasterizerState(&desc, rasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// 頂点バッファ
	{
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = sizeof(Vertex) * vertexCount;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		HRESULT hr = device->CreateBuffer(&desc, nullptr, vertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	//GpuResourceUtils::LoadTexture(device, "Data/Sprites/20160517231910.png", texture.ReleaseAndGetAddressOf(), &textureDesc);
	GpuResourceUtils::LoadTexture(device, "Data/Sprites/trail.png", texture.ReleaseAndGetAddressOf(), &textureDesc);
}

//***********************************************************
// @brief       描画
// @param[in]   context     ID3D11DeviceContext*
// @param[in]   view        カメラのビュー
// @param[in]   projection  カメラのプロジェクション
// @return      なし
//***********************************************************
void LineRenderer::Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	RenderState* renderState = Graphics::Instance().GetRenderState();

	// シェーダー設定
	context->VSSetShader(vertexShader.Get(), nullptr, 0);
	context->PSSetShader(pixelShader.Get(), nullptr, 0);
	context->PSSetShaderResources(0, 1, texture.GetAddressOf());
	context->IASetInputLayout(inputLayout.Get());

	// 定数バッファ設定
	context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
    context->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	// レンダーステート設定
	const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	context->OMSetBlendState(blendState.Get(), blendFactor, 0xFFFFFFFF);
	context->OMSetDepthStencilState(depthStencilState.Get(), 0);
	context->RSSetState(rasterizerState.Get());

	// プリミティブ設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	// 定数バッファ更新
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX VP = V * P;
	ConstantBuffer data;
	DirectX::XMStoreFloat4x4(&data.wvp, VP);
	context->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);

	// 描画
	UINT totalVertexCount = static_cast<UINT>(vertices.size());
	UINT start = 0;
	UINT count = (totalVertexCount < capacity) ? totalVertexCount : capacity;

	while (start < totalVertexCount)
	{
		D3D11_MAPPED_SUBRESOURCE mappedVB;
		HRESULT hr = context->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVB);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		memcpy(mappedVB.pData, &vertices[start], sizeof(Vertex) * count);

		context->Unmap(vertexBuffer.Get(), 0);

		context->Draw(count, 0);

		start += count;
		if ((start + count) > totalVertexCount)
		{
			count = totalVertexCount - start;
		}
	}
	vertices.clear();
}

//***********************************************************
// @brief       頂点追加
// @param[in]   position    位置
// @param[in]   color       色
// @param[in]   texcoord    uv
// @return      なし
//***********************************************************
void LineRenderer::AddVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT2& texcoord)
{
	Vertex v;
	v.position = position;
	v.color = color;
	v.texcoord = texcoord;
	vertices.emplace_back(v);
}

//**************************************************************
// @brief       コンストラクタ
// @param[in]   device        ID3D12Device*
// @param[in]   vertexCount   頂点数
// @return      なし
//**************************************************************
LineRendererDX12::LineRendererDX12(ID3D12Device* device, UINT vertexCount) : capacity(vertexCount)
{
	Graphics&                 graphics = Graphics::Instance();
	const RenderStateDX12* renderState = graphics.GetRenderStateDX12();

	HRESULT hr = S_OK;

	// シェーダー
	std::vector<BYTE> vsData, psData;
	{
		GpuResourceUtils::LoadShaderFile("Data/Shader/LineDX12VS.cso", vsData);
		GpuResourceUtils::LoadShaderFile("Data/Shader/LinePS.cso", psData);
	}

	// ルートシグネチャの生成
	{
		hr = device->CreateRootSignature(
			0,
			vsData.data(),
			vsData.size(),
			IID_PPV_ARGS(m_d3d_root_signature.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_root_signature->SetName(L"LineShaderRootSignature");
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
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
		d3d_graphics_pipeline_state_desc.InputLayout.pInputElementDescs = inputElementDescs;
		d3d_graphics_pipeline_state_desc.InputLayout.NumElements = _countof(inputElementDescs);

		// ブレンドステート
		d3d_graphics_pipeline_state_desc.BlendState = renderState->GetBlendState(BlendState::Transparency);

		// 深度ステンシルステート
		d3d_graphics_pipeline_state_desc.DepthStencilState = renderState->GetDepthState(DepthState::TestAndWrite);

		// ラスタライザーステート
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

		// プリミティブトポロジー
		d3d_graphics_pipeline_state_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// ストリップ時のカット値
		d3d_graphics_pipeline_state_desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		// レンダーターゲット数
		d3d_graphics_pipeline_state_desc.NumRenderTargets = 1;
		d3d_graphics_pipeline_state_desc.RTVFormats[0] = RenderTargetFormat;
		d3d_graphics_pipeline_state_desc.DSVFormat     = DepthStencilFormat;

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
		m_d3d_pipeline_state->SetName(L"LineShaderPipelineState");
	}

	CreateConstantBuffer();
}

//***********************************************************
// @brief       デストラクタ
// @param[in]   なし
// @return      なし
//***********************************************************
LineRendererDX12::~LineRendererDX12()
{
}

//***********************************************************
// @brief       描画
// @param[in]   framBuffer     フレームバッファ
// @return      なし
//***********************************************************
void LineRendererDX12::Render(ID3D12GraphicsCommandList* d3d_command_list, SpriteDX12* sprite)
{
	Graphics& graphics = Graphics::Instance();

	RenderContextDX12 rc;
	rc.d3d_command_list = d3d_command_list;

	// コンスタントバッファ更新
	cbv_descriptor = UpdateSceneConstantBuffer();

	// 頂点バッファ更新
	void* vb;
	D3D12_RANGE readRange = { 0, 0 };
	d3d_vb_resource->Map(0, &readRange, &vb);
	memcpy(vb, vertices.data(), sizeof(Vertex) * vertices.size());
	d3d_vb_resource->Unmap(0, nullptr);

	// パイプライン設定
	rc.d3d_command_list->SetGraphicsRootSignature(m_d3d_root_signature.Get());
	rc.d3d_command_list->SetPipelineState(m_d3d_pipeline_state.Get());

	// 頂点バッファ設定
	rc.d3d_command_list->IASetVertexBuffers(0, 1, &d3d_vbv);
	rc.d3d_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ディスクリプタ設定
	rc.d3d_command_list->SetGraphicsRootDescriptorTable(0, sprite->GetDescriptor()->GetGpuHandle());
	rc.d3d_command_list->SetGraphicsRootDescriptorTable(1, cbv_descriptor->GetGpuHandle());

	// 描画
	rc.d3d_command_list->DrawInstanced(static_cast<UINT>(vertices.size()), 1, 0, 0);

	vertices.clear();
}

//***********************************************************
// @brief       頂点追加
// @param[in]   position    位置
// @param[in]   color       色
// @param[in]   texcoord    uv
// @return      なし
//***********************************************************
void LineRendererDX12::AddVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT2& texcoord)
{
	Vertex v;
	v.position = position;
	v.color    = color;
	v.texcoord = texcoord;
	vertices.emplace_back(v);
}

//*************************************************************
// @brief       フレームリソース生成
// @param[in]   なし
// @return      なし
//*************************************************************
void LineRendererDX12::CreateConstantBuffer()
{
	Graphics&       graphics = Graphics::Instance();
	ID3D12Device* d3d_device = graphics.GetDeviceDX12();

	HRESULT hr = S_OK;

	// 頂点バッファ生成
	{
		UINT stride      = sizeof(Vertex);
		UINT buffer_size = (sizeof(Vertex) * capacity);

		// ヒーププロパティの設定
		D3D12_HEAP_PROPERTIES heap_props{};
		heap_props.Type                 = D3D12_HEAP_TYPE_UPLOAD;
		heap_props.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heap_props.CreationNodeMask     = 1;
		heap_props.VisibleNodeMask      = 1;

		// リソースの設定
		D3D12_RESOURCE_DESC resource_desc{};
		resource_desc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
		resource_desc.Alignment          = 0;
		resource_desc.Width              = buffer_size;
		resource_desc.Height             = 1;
		resource_desc.DepthOrArraySize   = 1;
		resource_desc.MipLevels          = 1;
		resource_desc.Format             = DXGI_FORMAT_UNKNOWN;
		resource_desc.SampleDesc.Count   = 1;
		resource_desc.SampleDesc.Quality = 0;
		resource_desc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resource_desc.Flags              = D3D12_RESOURCE_FLAG_NONE;

		hr = d3d_device->CreateCommittedResource(
			&heap_props,
			D3D12_HEAP_FLAG_NONE,
			&resource_desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(d3d_vb_resource.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		d3d_vb_resource->SetName(L"LineVertexBuffer");

		// 頂点バッファビュー設定
		d3d_vbv.BufferLocation = d3d_vb_resource->GetGPUVirtualAddress();
		d3d_vbv.SizeInBytes    = buffer_size;
		d3d_vbv.StrideInBytes  = stride;
	}

	// 定数バッファ
	{
		// ヒーププロパティの設定
		D3D12_HEAP_PROPERTIES heap_props{};
		heap_props.Type                 = D3D12_HEAP_TYPE_UPLOAD;
		heap_props.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heap_props.CreationNodeMask     = 1;
		heap_props.VisibleNodeMask      = 1;

		// リソースの設定
		D3D12_RESOURCE_DESC resource_desc{};
		resource_desc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
		resource_desc.Alignment          = 0;
		resource_desc.Width              = ((sizeof(CbScene)) + 255) & ~255;
		resource_desc.Height             = 1;
		resource_desc.DepthOrArraySize   = 1;
		resource_desc.MipLevels          = 1;
		resource_desc.Format             = DXGI_FORMAT_UNKNOWN;
		resource_desc.SampleDesc.Count   = 1;
		resource_desc.SampleDesc.Quality = 0;
		resource_desc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resource_desc.Flags              = D3D12_RESOURCE_FLAG_NONE;

		// リソースの生成
		hr = d3d_device->CreateCommittedResource(
			&heap_props,
			D3D12_HEAP_FLAG_NONE,
			&resource_desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(d3d_cbv_resource.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		d3d_cbv_resource->SetName(L"LineConstatBuffer");

		// ディスクリプタ取得
		cbv_descriptor = graphics.GetShaderResourceDescriptorHeap()->PopDescriptor();

		// コンスタントバッファビューの生成
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc{};
		cbv_desc.BufferLocation = d3d_cbv_resource->GetGPUVirtualAddress();
		cbv_desc.SizeInBytes = static_cast<UINT>(resource_desc.Width);

		d3d_device->CreateConstantBufferView(
			&cbv_desc,
			cbv_descriptor->GetCpuHandle());

		hr = d3d_cbv_resource->Map(0, nullptr, reinterpret_cast<void**>(&cbv_data));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

//*************************************************************
// @brief       終了処理
// @param[in]   なし
// @return      なし
//*************************************************************
void LineRendererDX12::Finalize()
{
	Graphics& graphics = Graphics::Instance();

	if (cbv_descriptor != nullptr)
	{
		graphics.GetShaderResourceDescriptorHeap()->PushDescriptor(cbv_descriptor);
	}
	if (cbv_data != nullptr)
	{
		d3d_cbv_resource->Unmap(0, nullptr);
		cbv_data = nullptr;
	}
}

//*************************************************************
// @brief       コンスタントバッファ更新
// @param[in]   なし
// @return      const Descriptor*
//*************************************************************
const Descriptor* LineRendererDX12::UpdateSceneConstantBuffer()
{
	const Camera* camera = CameraManager::Instance().GetCamera();

	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&camera->GetView());
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&camera->GetProjection());
	DirectX::XMMATRIX VP = V * P;
	DirectX::XMStoreFloat4x4(&cbv_data->wvp, VP);

	return cbv_descriptor;
}

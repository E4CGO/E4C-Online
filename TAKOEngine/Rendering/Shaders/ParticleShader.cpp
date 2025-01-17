//! @file ParticalShader.cpp
//! @note

#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/Graphics.h"

#include "ParticleShader.h"

//**************************************************************
// @brief       コンストラクタ
// @param[in]   device　    ID3D12Device*
// @return      なし
//**************************************************************
ParticleShader::ParticleShader(ID3D12Device* device)
{
	Graphics& graphics = Graphics::Instance();
	const RenderStateDX12* renderState = graphics.GetRenderStateDX12();

	HRESULT hr = S_OK;

	// シェーダー
	std::vector<BYTE> vsData, psData, gsData;
	{
		GpuResourceUtils::LoadShaderFile("Data/Shader/ParticleVS.cso", vsData);
		GpuResourceUtils::LoadShaderFile("Data/Shader/ParticlePS.cso", psData);
		GpuResourceUtils::LoadShaderFile("Data/Shader/ParticleGS.cso", gsData);
	}

	// ルートシグネチャの生成
	{
		hr = device->CreateRootSignature(
			0,
			vsData.data(),
			vsData.size(),
			IID_PPV_ARGS(m_d3d_root_signature.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_root_signature->SetName(L"ParticleShaderRootSignature");
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
		d3d_graphics_pipeline_state_desc.GS.pShaderBytecode = gsData.data();
		d3d_graphics_pipeline_state_desc.GS.BytecodeLength  = gsData.size();

		// 入力レイアウト
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "PARAM",    0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
		d3d_graphics_pipeline_state_desc.InputLayout.pInputElementDescs = inputElementDescs;
		d3d_graphics_pipeline_state_desc.InputLayout.NumElements = _countof(inputElementDescs);

		// ブレンドステート
		d3d_graphics_pipeline_state_desc.BlendState.AlphaToCoverageEnable          = false;
		d3d_graphics_pipeline_state_desc.BlendState.IndependentBlendEnable         = false;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].BlendEnable    = true;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].SrcBlend       = D3D12_BLEND_ONE;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].DestBlend      = D3D12_BLEND_INV_SRC_ALPHA;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].BlendOp        = D3D12_BLEND_OP_ADD;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].SrcBlendAlpha  = D3D12_BLEND_ONE;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].BlendOpAlpha   = D3D12_BLEND_OP_ADD;
		d3d_graphics_pipeline_state_desc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		
		// 深度ステンシルステート
		d3d_graphics_pipeline_state_desc.DepthStencilState.DepthEnable    = true;
		d3d_graphics_pipeline_state_desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		d3d_graphics_pipeline_state_desc.DepthStencilState.DepthFunc      = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		d3d_graphics_pipeline_state_desc.DepthStencilState.StencilEnable  = false;

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
		d3d_graphics_pipeline_state_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

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
		m_d3d_pipeline_state->SetName(L"ParticleShaderPipelineState");
	}
}

//***********************************************************
// @brief       デストラクタ
// @param[in]   なし
// @return      なし
//***********************************************************
ParticleShader::~ParticleShader()
{
}

//***********************************************************
// @brief       描画
// @param[in]   rc     レンダーコンテキスト
// @return      なし
//***********************************************************
void ParticleShader::Render(const RenderContextDX12& rc, SpriteDX12* sprite)
{
	Graphics& graphics = Graphics::Instance();

	// パイプライン設定
	rc.d3d_command_list->SetGraphicsRootSignature(m_d3d_root_signature.Get());
	rc.d3d_command_list->SetPipelineState(m_d3d_pipeline_state.Get());

	// 頂点バッファ設定
	rc.d3d_command_list->IASetVertexBuffers(0, 1, &graphics.GetParticleCompute(ComputeShaderDX12Id::Injection)->d3d_vbv);
	rc.d3d_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	// ディスクリプタ設定
	rc.d3d_command_list->SetGraphicsRootDescriptorTable(0, sprite->GetDescriptor()->GetGpuHandle());
	rc.d3d_command_list->SetGraphicsRootDescriptorTable(1, rc.particleData.cbv_descriptor->GetGpuHandle());

	// 描画
	rc.d3d_command_list->DrawInstanced(graphics.GetParticleCompute(ComputeShaderDX12Id::Injection)->GetMaxParticle(), 1, 0, 0);
}

//*************************************************************
// @brief       コンストラクタ
// @param[in]   device　ID3D12Device*
// @return      なし
//*************************************************************
ParticleCompute::ParticleCompute(ID3D12Device* device)
{
	HRESULT hr = S_OK;

	MaxParticles = 10000;

	// シェーダー読み込み
	std::vector<BYTE> cs_data;
	{
		GpuResourceUtils::LoadShaderFile("Data/Shader/ParticleCS.cso", cs_data);
	}

	// ルートシグネクチャの生成
	{
		hr = device->CreateRootSignature(
			0,
			cs_data.data(),
			cs_data.size(),
			IID_PPV_ARGS(m_d3d_root_signature.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_root_signature->SetName(L"ParticalCSRootSignature");
	}

	// パイプラインステートの生成
	{
		D3D12_COMPUTE_PIPELINE_STATE_DESC d3d_compute_pipeline_state_desc = {};

		// ルートシグネクチャ
		d3d_compute_pipeline_state_desc.pRootSignature = m_d3d_root_signature.Get();

		d3d_compute_pipeline_state_desc.CS.pShaderBytecode = cs_data.data();
		d3d_compute_pipeline_state_desc.CS.BytecodeLength  = cs_data.size();

		// パイプラインステート生成
		hr = device->CreateComputePipelineState(
			&d3d_compute_pipeline_state_desc,
			IID_PPV_ARGS(m_d3d_pipeline_state.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_pipeline_state->SetName(L"ParticalCSPipelineState");
	}

	// 頂点設定
	points.resize(MaxParticles);
	for (int i = 0; i < MaxParticles; i++)
	{
		points[i].position.x = (rand() % 200 - 100) * 0.1f;
		points[i].position.y = (rand() % 100) * 0.1f;
		points[i].position.z = (rand() % 200 - 50) * 0.1f; // z範囲を調整
		
		points[i].velocity.x = (rand() % 200 - 100) * 0.001f;
		points[i].velocity.y = (rand() % 100) * 0.001f;
		points[i].velocity.z = (rand() % 200 - 100) * 0.001f;

		points[i].Life = 0;
		points[i].LifeTime = -1;
	}

	// フレームリソース生成
	CreateFrameResource();
}

//*************************************************************
// @brief       デストラクタ
// @param[in]   なし
// @return      なし
//*************************************************************
ParticleCompute::~ParticleCompute()
{
	points.clear();
}

//*************************************************************
// @brief       計算
// @param[in]   rc     レンダーコンテキスト(DX12)
// @return      なし
//*************************************************************
void ParticleCompute::Compute(const RenderContextDX12& rc, SpriteDX12* sprite)
{
	Graphics& graphics = Graphics::Instance();

	//パイプライン設定
	rc.d3d_command_list->SetComputeRootSignature(m_d3d_root_signature.Get());
	rc.d3d_command_list->SetPipelineState(m_d3d_pipeline_state.Get());

	// ディスクリプタ設定
	rc.d3d_command_list->SetComputeRootDescriptorTable(0, sprite->GetDescriptor()->GetGpuHandle());
	rc.d3d_command_list->SetComputeRootDescriptorTable(1, uav_descriptor->GetGpuHandle());
	rc.d3d_command_list->SetComputeRootDescriptorTable(2, rc.particleData.cbv_descriptor->GetGpuHandle());

	// 計算実行
	rc.d3d_command_list->Dispatch((MaxParticles + 255) / 256, 1, 1);
}

//*************************************************************
// @brief       フレームリソース生成
// @param[in]   なし
// @return      なし
//*************************************************************
void ParticleCompute::CreateFrameResource()
{
	Graphics& graphics = Graphics::Instance();
	ID3D12Device* d3d_device = graphics.GetDeviceDX12();

	HRESULT hr = S_OK;

	// アンオーダードアクセス生成
	{
		UINT stride = sizeof(Point);
		UINT buffer_size = (sizeof(Point) * MaxParticles);

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

		// アップロード用定数バッファの生成
		Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_upload_resource;
		hr = d3d_device->CreateCommittedResource(
			&heap_props,
			D3D12_HEAP_FLAG_NONE,
			&resource_desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(d3d_upload_resource.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// アップロード用アンオーダードアクセスにデータ内容をコピー
		void* vb = nullptr;
		hr = d3d_upload_resource->Map(0, nullptr, &vb);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		::memcpy(vb, points.data(), buffer_size);
		d3d_upload_resource->Unmap(0, nullptr);

		// 描画用アンオーダードアクセス生成
		heap_props.Type     = D3D12_HEAP_TYPE_DEFAULT;
		resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		hr = d3d_device->CreateCommittedResource(
			&heap_props,
			D3D12_HEAP_FLAG_NONE,
			&resource_desc,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			nullptr,
			IID_PPV_ARGS(d3d_vbv_uav_resource.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		d3d_vbv_uav_resource->SetName(L"ParticleVertexBuffer");

		// アップロード用バッファを描画用バッファにコピー
		graphics.CopyBuffer(d3d_upload_resource.Get(), d3d_vbv_uav_resource.Get());

		// 頂点バッファビュー設定
		d3d_vbv.BufferLocation = d3d_vbv_uav_resource->GetGPUVirtualAddress();
		d3d_vbv.SizeInBytes    = buffer_size;
		d3d_vbv.StrideInBytes  = stride;

		// ディスクリプタ取得
		uav_descriptor = graphics.GetShaderResourceDescriptorHeap()->PopDescriptor();

		// アンオーダードアクセスビューの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc{};
		uav_desc.Format                      = DXGI_FORMAT_UNKNOWN;
		uav_desc.ViewDimension               = D3D12_UAV_DIMENSION_BUFFER;
		uav_desc.Buffer.FirstElement         = 0;
		uav_desc.Buffer.NumElements          = MaxParticles;
		uav_desc.Buffer.StructureByteStride  = stride;
		uav_desc.Buffer.CounterOffsetInBytes = 0;

		// アンオーダードアクセスビューの生成
		d3d_device->CreateUnorderedAccessView(
			d3d_vbv_uav_resource.Get(),
			nullptr,
			&uav_desc,
			uav_descriptor->GetCpuHandle());
	}
}
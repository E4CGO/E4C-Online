//! @file SpriteDX12.cpp
//! @note

#include "SpriteDX12.h"

#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/Graphics.h"

/**************************************************************************//**
		@brief		コンストラクタ
		@param[in]	sprite_count	描画数
		@param[in]	filename		画像場所
		@return		なし
*//***************************************************************************/
SpriteDX12::SpriteDX12(UINT sprite_count, const char* filename) : m_sprite_count(sprite_count)
{
	Graphics& graphics = Graphics::Instance();
	const RenderStateDX12* renderState = graphics.GetRenderStateDX12();
	ID3D12Device* d3d_device = graphics.GetDeviceDX12();

	HRESULT hr = S_OK;

	// シェーダー
	std::vector<BYTE> vsData, psData;
	{
		GpuResourceUtils::LoadShaderFile("Data/Shader/SpriteVS.cso", vsData);
		GpuResourceUtils::LoadShaderFile("Data/Shader/SpritePS.cso", psData);
	}

	// ルートシグネチャの生成
	{
		hr = d3d_device->CreateRootSignature(
			0,
			vsData.data(),
			vsData.size(),
			IID_PPV_ARGS(m_d3d_root_signature.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_root_signature->SetName(L"SpriteRootSignature");
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
		D3D12_INPUT_ELEMENT_DESC d3d_input_element_descs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
		d3d_graphics_pipeline_state_desc.InputLayout.pInputElementDescs = d3d_input_element_descs;
		d3d_graphics_pipeline_state_desc.InputLayout.NumElements = _countof(d3d_input_element_descs);

		// ブレンドステート
		d3d_graphics_pipeline_state_desc.BlendState = renderState->GetBlendState(BlendState::Transparency);

		// 深度ステンシルステート
		d3d_graphics_pipeline_state_desc.DepthStencilState.DepthEnable = true;
		d3d_graphics_pipeline_state_desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		d3d_graphics_pipeline_state_desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		d3d_graphics_pipeline_state_desc.DepthStencilState.StencilEnable = false;

		// ラスタライザーステート
		d3d_graphics_pipeline_state_desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		d3d_graphics_pipeline_state_desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		d3d_graphics_pipeline_state_desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		d3d_graphics_pipeline_state_desc.RasterizerState.FrontCounterClockwise = false;
		d3d_graphics_pipeline_state_desc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		d3d_graphics_pipeline_state_desc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		d3d_graphics_pipeline_state_desc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		d3d_graphics_pipeline_state_desc.RasterizerState.DepthClipEnable = true;
		d3d_graphics_pipeline_state_desc.RasterizerState.MultisampleEnable = false;
		d3d_graphics_pipeline_state_desc.RasterizerState.AntialiasedLineEnable = false;
		d3d_graphics_pipeline_state_desc.RasterizerState.ForcedSampleCount = 0;
		d3d_graphics_pipeline_state_desc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

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

		hr = d3d_device->CreateGraphicsPipelineState(
			&d3d_graphics_pipeline_state_desc,
			IID_PPV_ARGS(&m_d3d_pipeline_state)
		);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_pipeline_state->SetName(L"SpriteGraphicsPipelineState");
	}

	// テクスチャの生成
	{
		if (filename != nullptr)
		{
			// テクスチャ読み込み
			hr = Graphics::Instance().LoadTexture(filename, m_d3d_texture.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
		else
		{
			// ダミーテクスチャ生成
			hr = Graphics::Instance().CreateDummyTexture(m_d3d_texture.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}

		// ディスクリプタ取得
		m_srv_descriptor = Graphics::Instance().GetShaderResourceDescriptorHeap()->PopDescriptor();

		// シェーダーリソースビューの生成
		D3D12_RESOURCE_DESC d3d_resource_desc = m_d3d_texture->GetDesc();

		// シェーダーリソースビューの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC d3d_srv_desc = {};
		d3d_srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		d3d_srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		d3d_srv_desc.Format = d3d_resource_desc.Format;
		d3d_srv_desc.Texture2D.MipLevels = d3d_resource_desc.MipLevels;
		d3d_srv_desc.Texture2D.MostDetailedMip = 0;

		// シェーダリソースビューを生成
		d3d_device->CreateShaderResourceView(
			m_d3d_texture.Get(),
			&d3d_srv_desc,
			m_srv_descriptor->GetCpuHandle()
		);

		// テクスチャ情報の取得
		m_texture_width = static_cast<int>(d3d_resource_desc.Width);
		m_texture_height = static_cast<int>(d3d_resource_desc.Height);
	}

	// フレームリソース生成
	CreateFrameResource();
}

/**************************************************************************//**
		@brief		コンストラクタ
		@param[in]	sprite_count	描画数
		@param[in]	frameBuffer		フレームバッファ画像
		@return		なし
*//***************************************************************************/
SpriteDX12::SpriteDX12(UINT sprite_count, FrameBufferTexture* frameBuffer) : m_sprite_count(sprite_count)
{
	Graphics& graphics = Graphics::Instance();
	const RenderStateDX12* renderState = graphics.GetRenderStateDX12();
	ID3D12Device* d3d_device = graphics.GetDeviceDX12();

	HRESULT hr = S_OK;

	// シェーダー
	std::vector<BYTE> vsData, psData;
	{
		GpuResourceUtils::LoadShaderFile("Data/Shader/SpriteVS.cso", vsData);
		GpuResourceUtils::LoadShaderFile("Data/Shader/SpritePS.cso", psData);
	}

	// ルートシグネチャの生成
	{
		hr = d3d_device->CreateRootSignature(
			0,
			vsData.data(),
			vsData.size(),
			IID_PPV_ARGS(m_d3d_root_signature.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_root_signature->SetName(L"SpriteRootSignature");
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
		D3D12_INPUT_ELEMENT_DESC d3d_input_element_descs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
		d3d_graphics_pipeline_state_desc.InputLayout.pInputElementDescs = d3d_input_element_descs;
		d3d_graphics_pipeline_state_desc.InputLayout.NumElements = _countof(d3d_input_element_descs);

		// ブレンドステート
		d3d_graphics_pipeline_state_desc.BlendState = renderState->GetBlendState(BlendState::Transparency);

		// 深度ステンシルステート
		d3d_graphics_pipeline_state_desc.DepthStencilState.DepthEnable = true;
		d3d_graphics_pipeline_state_desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		d3d_graphics_pipeline_state_desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		d3d_graphics_pipeline_state_desc.DepthStencilState.StencilEnable = false;

		// ラスタライザーステート
		d3d_graphics_pipeline_state_desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		d3d_graphics_pipeline_state_desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		d3d_graphics_pipeline_state_desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		d3d_graphics_pipeline_state_desc.RasterizerState.FrontCounterClockwise = false;
		d3d_graphics_pipeline_state_desc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		d3d_graphics_pipeline_state_desc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		d3d_graphics_pipeline_state_desc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		d3d_graphics_pipeline_state_desc.RasterizerState.DepthClipEnable = true;
		d3d_graphics_pipeline_state_desc.RasterizerState.MultisampleEnable = false;
		d3d_graphics_pipeline_state_desc.RasterizerState.AntialiasedLineEnable = false;
		d3d_graphics_pipeline_state_desc.RasterizerState.ForcedSampleCount = 0;
		d3d_graphics_pipeline_state_desc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

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

		hr = d3d_device->CreateGraphicsPipelineState(
			&d3d_graphics_pipeline_state_desc,
			IID_PPV_ARGS(&m_d3d_pipeline_state));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_pipeline_state->SetName(L"SpriteGraphicsPipelineState");
	}

	// テクスチャの生成
	{
		m_d3d_texture = frameBuffer->Get();

		if (m_d3d_texture)
		{
			m_srv_descriptor = Graphics::Instance().GetShaderResourceDescriptorHeap()->PopDescriptor();

			// シェーダーリソースビューの生成
			D3D12_RESOURCE_DESC d3d_resource_desc = m_d3d_texture->GetDesc();

			D3D12_SHADER_RESOURCE_VIEW_DESC d3d_srv_desc = {};
			d3d_srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			d3d_srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			d3d_srv_desc.Format = d3d_resource_desc.Format;
			d3d_srv_desc.Texture2D.MipLevels = d3d_resource_desc.MipLevels;
			d3d_srv_desc.Texture2D.MostDetailedMip = 0;

			// シェーダリソースビューを生成.
			d3d_device->CreateShaderResourceView(
				m_d3d_texture.Get(),
				&d3d_srv_desc,
				m_srv_descriptor->GetCpuHandle());

			// テクスチャ情報の取得
			m_texture_width = static_cast<int>(d3d_resource_desc.Width);
			m_texture_height = static_cast<int>(d3d_resource_desc.Height);
		}
	}

	// フレームリソース生成
	CreateFrameResource();
}

/**************************************************************************//**
		@brief		コンストラクタ
		@param[in]	sprite_count	描画数
		@param[in]	filename		キューブマップ用テクスチャ
		@return		なし
*//***************************************************************************/
SpriteDX12::SpriteDX12(UINT sprite_count, const std::wstring& filename) : m_sprite_count(sprite_count)
{
	Graphics& graphics = Graphics::Instance();
	const RenderStateDX12* renderState = graphics.GetRenderStateDX12();
	ID3D12Device* d3d_device = graphics.GetDeviceDX12();

	HRESULT hr = S_OK;

	// シェーダー
	std::vector<BYTE> vsData, psData;
	{
		GpuResourceUtils::LoadShaderFile("Data/Shader/SpriteVS.cso", vsData);
		GpuResourceUtils::LoadShaderFile("Data/Shader/SpritePS.cso", psData);
	}

	// ルートシグネチャの生成
	{
		hr = d3d_device->CreateRootSignature(
			0,
			vsData.data(),
			vsData.size(),
			IID_PPV_ARGS(m_d3d_root_signature.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_root_signature->SetName(L"SpriteRootSignature");
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
		D3D12_INPUT_ELEMENT_DESC d3d_input_element_descs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
		d3d_graphics_pipeline_state_desc.InputLayout.pInputElementDescs = d3d_input_element_descs;
		d3d_graphics_pipeline_state_desc.InputLayout.NumElements = _countof(d3d_input_element_descs);

		// ブレンドステート
		d3d_graphics_pipeline_state_desc.BlendState = renderState->GetBlendState(BlendState::Transparency);

		// 深度ステンシルステート
		d3d_graphics_pipeline_state_desc.DepthStencilState.DepthEnable = true;
		d3d_graphics_pipeline_state_desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		d3d_graphics_pipeline_state_desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		d3d_graphics_pipeline_state_desc.DepthStencilState.StencilEnable = false;

		// ラスタライザーステート
		d3d_graphics_pipeline_state_desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		d3d_graphics_pipeline_state_desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		d3d_graphics_pipeline_state_desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		d3d_graphics_pipeline_state_desc.RasterizerState.FrontCounterClockwise = false;
		d3d_graphics_pipeline_state_desc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		d3d_graphics_pipeline_state_desc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		d3d_graphics_pipeline_state_desc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		d3d_graphics_pipeline_state_desc.RasterizerState.DepthClipEnable = true;
		d3d_graphics_pipeline_state_desc.RasterizerState.MultisampleEnable = false;
		d3d_graphics_pipeline_state_desc.RasterizerState.AntialiasedLineEnable = false;
		d3d_graphics_pipeline_state_desc.RasterizerState.ForcedSampleCount = 0;
		d3d_graphics_pipeline_state_desc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

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

		hr = d3d_device->CreateGraphicsPipelineState(
			&d3d_graphics_pipeline_state_desc,
			IID_PPV_ARGS(&m_d3d_pipeline_state));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		m_d3d_pipeline_state->SetName(L"SpriteGraphicsPipelineState");
	}

	// テクスチャ生成
	{
		// テクスチャ読み込み
		hr = Graphics::Instance().LoadCubeTexture(filename, m_d3d_texture.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// ディスクリプタ取得
		m_srv_descriptor = Graphics::Instance().GetShaderResourceDescriptorHeap()->PopDescriptor();

		// シェーダーリソースビューの生成
		D3D12_RESOURCE_DESC d3d_resource_desc = m_d3d_texture->GetDesc();

		// シェーダーリソースビューの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC d3d_srv_desc = {};
		d3d_srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		d3d_srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		d3d_srv_desc.Format = d3d_resource_desc.Format;;
		d3d_srv_desc.TextureCube.MipLevels = d3d_resource_desc.MipLevels;
		d3d_srv_desc.TextureCube.MostDetailedMip = 0;
		d3d_srv_desc.TextureCube.ResourceMinLODClamp = 0.0f;

		// シェーダリソースビューを生成.
		d3d_device->CreateShaderResourceView(
			m_d3d_texture.Get(),
			&d3d_srv_desc,
			m_srv_descriptor->GetCpuHandle());

		// テクスチャ情報の取得
		m_texture_width = static_cast<int>(d3d_resource_desc.Width);
		m_texture_height = static_cast<int>(d3d_resource_desc.Height);
	}

	// フレームリソース生成
	CreateFrameResource();
}

/**************************************************************************//**
		@brief		デストラクタ
		@param[in]	なし
		@return		なし
*//***************************************************************************/
SpriteDX12::~SpriteDX12()
{
	for (FrameResource& frame_resource : m_frame_resources)
	{
		if (frame_resource.vertex_data != nullptr)
		{
			frame_resource.d3d_vb_resource->Unmap(0, nullptr);
		}
		if (frame_resource.cb_scene_data != nullptr)
		{
			frame_resource.d3d_cbv_resource->Unmap(0, nullptr);
		}
		if (frame_resource.cbv_descriptor != nullptr)
		{
			Graphics::Instance().GetShaderResourceDescriptorHeap()->PushDescriptor(frame_resource.cbv_descriptor);
		}
	}

	if (m_srv_descriptor != nullptr)
	{
		Graphics::Instance().GetShaderResourceDescriptorHeap()->PushDescriptor(m_srv_descriptor);
	}
}

/**************************************************************************//**
		@brief		描画開始
		@param[in]	rc	レンダーコンテキスト
		@return		なし
*//***************************************************************************/
void SpriteDX12::Begin(const RenderContextDX12& rc)
{
	Graphics& graphics = Graphics::Instance();

	m_sprite_index = 0;

	FrameResource& fram_resource = m_frame_resources.at(graphics.GetCurrentBufferIndex());

	// LuminanceExtraction
	fram_resource.cb_scene_data->threshold = rc.luminanceExtractionData.threshold;
	fram_resource.cb_scene_data->intensity = rc.luminanceExtractionData.intensity;

	// フィルター値計算(GaussianBlur)
	CalcGaussianFilter(fram_resource, rc.gaussianFilterData);

	// ColorGrading
	fram_resource.cb_scene_data->hueShift = rc.colorGradingData.hueShift;
	fram_resource.cb_scene_data->saturation = rc.colorGradingData.saturation;
	fram_resource.cb_scene_data->brightness = rc.colorGradingData.brightness;
}

//**********************************************************************
// @brief     フィルター値計算
// @param[in] fram_resource       コンスタントバッファに送るデータ
// @param[in] gaussianFilterData  ガウスフィルター計算情報
// @return    なし
//**********************************************************************
void SpriteDX12::CalcGaussianFilter(FrameResource& fram_resource, const GaussianFilterData& gaussianFilterData)
{
	Graphics& graphics = Graphics::Instance();

	int kernelSize = gaussianFilterData.kernelSize;

	//偶数の場合は奇数に直す
	if (kernelSize % 2 == 0) kernelSize++;
	kernelSize = max(1, min(MaxKernelSize - 1, kernelSize));

	fram_resource.cb_scene_data->kernelSize = static_cast<float>(kernelSize);
	fram_resource.cb_scene_data->textureSize.x = 1.0f / gaussianFilterData.textureSize.x;
	fram_resource.cb_scene_data->textureSize.y = 1.0f / gaussianFilterData.textureSize.y;

	float deviationPow2 = 2.0f * gaussianFilterData.deviation * gaussianFilterData.deviation;
	float sum = 0.0f;
	int id = 0;
	for (int y = -kernelSize / 2; y <= kernelSize / 2; y++)
	{
		for (int x = -kernelSize / 2; x <= kernelSize / 2; x++)
		{
			fram_resource.cb_scene_data->weights[id].x = (float)x;
			fram_resource.cb_scene_data->weights[id].y = (float)y;
			fram_resource.cb_scene_data->weights[id].z = (1 / DirectX::XM_PI * deviationPow2 * exp(0 - (x * x + y * y) / deviationPow2));

			sum += fram_resource.cb_scene_data->weights[id].z;
			id++;
		}
	}

	for (int i = 0; i < fram_resource.cb_scene_data->kernelSize * fram_resource.cb_scene_data->kernelSize; i++)
	{
		fram_resource.cb_scene_data->weights[i].z /= sum;
	}
}

/**************************************************************************//**
		@brief		描画登録
		@param[in]	dx	X座標
		@param[in]	dy	Y座標
		@param[in]	dw	幅
		@param[in]	dh	高さ
		@param[in]	angle	角
		@param[in]	r	赤色
		@param[in]	g	緑色
		@param[in]	b	青色
		@param[in]	a	透明
		@return		なし
*//***************************************************************************/
void SpriteDX12::Draw(
	float dx, float dy,
	float dw, float dh,
	float angle,
	float r, float g, float b, float a)
{
	Graphics& graphics = Graphics::Instance();

	Draw(
		dx, dy, dw, dh,
		0, 0, static_cast<float>(m_texture_width), static_cast<float>(m_texture_height),
		angle,
		r, g, b, a,
		graphics.GetViwePort());
}

/**************************************************************************//**
		@brief		描画登録
		@param[in]	dx	X座標
		@param[in]	dy	Y座標
		@param[in]	dw	幅
		@param[in]	dh	高さ
		@param[in]	sx	テクスチャのX座標
		@param[in]	sy	テクスチャのY座標
		@param[in]	sw	テクスチャの幅
		@param[in]	sh	テクスチャの高さ
		@param[in]	angle	角
		@param[in]	r	赤色
		@param[in]	g	緑色
		@param[in]	b	青色
		@param[in]	a	透明
		@return		なし
*//***************************************************************************/
void SpriteDX12::Draw(
	float dx, float dy,
	float dw, float dh,
	float sx, float sy,
	float sw, float sh,
	float angle,
	float r, float g, float b, float a,
	D3D12_VIEWPORT viewport)
{
	if (m_sprite_index >= m_sprite_count) return;

	Graphics& graphics = Graphics::Instance();

	// スプライトを構成する４頂点のスクリーン座標を計算する
	DirectX::XMFLOAT2 positions[] = {
		DirectX::XMFLOAT2(dx,      dy),			// 左上
		DirectX::XMFLOAT2(dx + dw, dy),			// 右上
		DirectX::XMFLOAT2(dx,      dy + dh),	// 左下
		DirectX::XMFLOAT2(dx + dw, dy + dh),	// 右下
	};

	// スプライトを構成する４頂点のテクスチャ座標を計算する
	DirectX::XMFLOAT2 texcoords[] = {
		DirectX::XMFLOAT2(sx,      sy),			// 左上
		DirectX::XMFLOAT2(sx + sw, sy),			// 右上
		DirectX::XMFLOAT2(sx,      sy + sh),	// 左下
		DirectX::XMFLOAT2(sx + sw, sy + sh),	// 右下
	};

	// スプライトの中心で回転させるために４頂点の中心位置が
	// 原点(0, 0)になるように一旦頂点を移動させる。
	float mx = dx + dw * 0.5f;
	float my = dy + dh * 0.5f;
	for (auto& p : positions)
	{
		p.x -= mx;
		p.y -= my;
	}

	// 頂点を回転させる
	float theta = DirectX::XMConvertToRadians(angle);	// 角度をラジアン(θ)に変換
	float c = cosf(theta);
	float s = sinf(theta);
	for (auto& p : positions)
	{
		DirectX::XMFLOAT2 r = p;
		p.x = c * r.x + -s * r.y;
		p.y = s * r.x + c * r.y;
	}

	// 回転のために移動させた頂点を元の位置に戻す
	for (auto& p : positions)
	{
		p.x += mx;
		p.y += my;
	}

	// 現在設定されているビューポートからスクリーンサイズを取得する
	float screen_width = viewport.Width;
	float screen_height = viewport.Height;

	// スクリーン座標系からNDC座標系へ変換する。
	for (auto& p : positions)
	{
		p.x = 2.0f * p.x / screen_width - 1.0f;
		p.y = 1.0f - 2.0f * p.y / screen_height;
	}

	// 頂点バッファの内容の編集する。
	FrameResource& frame_resource = m_frame_resources.at(graphics.GetCurrentBufferIndex());
	Vertex* v = frame_resource.vertex_data + (m_sprite_index * 4);

	for (int i = 0; i < 4; ++i)
	{
		v[i].position.x = positions[i].x;
		v[i].position.y = positions[i].y;
		v[i].position.z = 0.0f;

		v[i].color.x = r;
		v[i].color.y = g;
		v[i].color.z = b;
		v[i].color.w = a;

		v[i].texcoord.x = texcoords[i].x / m_texture_width;
		v[i].texcoord.y = texcoords[i].y / m_texture_height;
	}
	m_sprite_index++;
}

/**************************************************************************//**
		@brief		描画終了
		@param[in]	d3d_command_list	コマンドリスト
		@return		なし
*//***************************************************************************/
void SpriteDX12::End(ID3D12GraphicsCommandList* d3d_command_list)
{
	Graphics& graphics = Graphics::Instance();
	FrameResource& frame_resource = m_frame_resources.at(graphics.GetCurrentBufferIndex());

	// パイプライン設定
	d3d_command_list->SetPipelineState(m_d3d_pipeline_state.Get());
	d3d_command_list->SetGraphicsRootSignature(m_d3d_root_signature.Get());

	// ディスクリプタテーブル
	d3d_command_list->SetGraphicsRootDescriptorTable(0, m_srv_descriptor->GetGpuHandle());

	// 頂点バッファ設定
	d3d_command_list->IASetVertexBuffers(0, 1, &frame_resource.d3d_vbv);
	d3d_command_list->IASetIndexBuffer(&frame_resource.d3d_ibv);
	d3d_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 描画
	d3d_command_list->DrawIndexedInstanced(m_sprite_index * 6, 1, 0, 0, 0);
}

/**************************************************************************//**
		@brief		フレームリソース生成
		@param[in]	なし
		@return		なし
*//***************************************************************************/
void SpriteDX12::CreateFrameResource()
{
	Graphics& graphics = Graphics::Instance();
	ID3D12Device* d3d_device = graphics.GetDeviceDX12();

	HRESULT hr = S_OK;

	// フレームリソース
	m_frame_resources.resize(graphics.GetBufferCount());
	for (FrameResource& frame_resource : m_frame_resources)
	{
		// 頂点バッファの生成
		{
			// ヒーププロパティの設定
			D3D12_HEAP_PROPERTIES d3d_heap_props{};
			d3d_heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
			d3d_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			d3d_heap_props.CreationNodeMask = 1;
			d3d_heap_props.VisibleNodeMask = 1;

			// リソースの設定
			D3D12_RESOURCE_DESC d3d_resource_desc{};
			d3d_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			d3d_resource_desc.Alignment = 0;
			d3d_resource_desc.Width = sizeof(Vertex) * 4 * m_sprite_count;
			d3d_resource_desc.Height = 1;
			d3d_resource_desc.DepthOrArraySize = 1;
			d3d_resource_desc.MipLevels = 1;
			d3d_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
			d3d_resource_desc.SampleDesc.Count = 1;
			d3d_resource_desc.SampleDesc.Quality = 0;
			d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			d3d_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

			// リソース生成
			hr = d3d_device->CreateCommittedResource(
				&d3d_heap_props,
				D3D12_HEAP_FLAG_NONE,
				&d3d_resource_desc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(frame_resource.d3d_vb_resource.GetAddressOf())
			);
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			frame_resource.d3d_vb_resource->SetName(L"SpriteVertexBuffer");

			// 頂点バッファビュー
			frame_resource.d3d_vbv.BufferLocation = frame_resource.d3d_vb_resource->GetGPUVirtualAddress();
			frame_resource.d3d_vbv.SizeInBytes = static_cast<UINT>(d3d_resource_desc.Width);
			frame_resource.d3d_vbv.StrideInBytes = sizeof(Vertex);

			// マップする
			frame_resource.d3d_vb_resource->Map(0, nullptr, reinterpret_cast<void**>(&frame_resource.vertex_data));
		}

		// コンスタントバッファ生成
		{
			// ヒーププロパティの設定
			D3D12_HEAP_PROPERTIES d3d_head_props{};
			d3d_head_props.Type = D3D12_HEAP_TYPE_UPLOAD;
			d3d_head_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			d3d_head_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			d3d_head_props.CreationNodeMask = 1;
			d3d_head_props.VisibleNodeMask = 1;

			// リソースの設定
			D3D12_RESOURCE_DESC d3d_resource_desc{};
			d3d_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			d3d_resource_desc.Alignment = 0;
			d3d_resource_desc.Width = ((sizeof(CbSpriteData)) + 255) & ~255;
			d3d_resource_desc.Height = 1;
			d3d_resource_desc.DepthOrArraySize = 1;
			d3d_resource_desc.MipLevels = 1;
			d3d_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
			d3d_resource_desc.SampleDesc.Count = 1;
			d3d_resource_desc.SampleDesc.Quality = 0;
			d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			d3d_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

			// リソース生成
			hr = d3d_device->CreateCommittedResource(
				&d3d_head_props,
				D3D12_HEAP_FLAG_NONE,
				&d3d_resource_desc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(frame_resource.d3d_cbv_resource.GetAddressOf()));
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			frame_resource.d3d_cbv_resource->SetName(L"SpriteSceneConstatBuffer");

			// ディスクリプタ取得
			frame_resource.cbv_descriptor = Graphics::Instance().GetShaderResourceDescriptorHeap()->PopDescriptor();

			// コンスタントバッファビューの生成
			D3D12_CONSTANT_BUFFER_VIEW_DESC d3d_cbv_desc;
			d3d_cbv_desc.BufferLocation = frame_resource.d3d_cbv_resource->GetGPUVirtualAddress();
			d3d_cbv_desc.SizeInBytes = static_cast<UINT>(d3d_resource_desc.Width);
			d3d_device->CreateConstantBufferView(
				&d3d_cbv_desc,
				frame_resource.cbv_descriptor->GetCpuHandle());

			hr = frame_resource.d3d_cbv_resource->Map(0, nullptr, reinterpret_cast<void**>(&frame_resource.cb_scene_data));
		}

		// インデックスバッファの生成
		{
			// ヒーププロパティの設定
			D3D12_HEAP_PROPERTIES d3d_heap_props{};
			d3d_heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
			d3d_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			d3d_heap_props.CreationNodeMask = 1;
			d3d_heap_props.VisibleNodeMask = 1;

			// リソースの設定
			D3D12_RESOURCE_DESC d3d_resource_desc{};
			d3d_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			d3d_resource_desc.Alignment = 0;
			d3d_resource_desc.Width = sizeof(UINT) * 6 * m_sprite_count;
			d3d_resource_desc.Height = 1;
			d3d_resource_desc.DepthOrArraySize = 1;
			d3d_resource_desc.MipLevels = 1;
			d3d_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
			d3d_resource_desc.SampleDesc.Count = 1;
			d3d_resource_desc.SampleDesc.Quality = 0;
			d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			d3d_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

			// リソース生成
			hr = d3d_device->CreateCommittedResource(
				&d3d_heap_props,
				D3D12_HEAP_FLAG_NONE,
				&d3d_resource_desc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(frame_resource.d3d_ib_resource.GetAddressOf())
			);
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			frame_resource.d3d_ib_resource->SetName(L"SpriteIndexBuffer");

			// インデックスバッファビュー設定
			frame_resource.d3d_ibv.BufferLocation = frame_resource.d3d_ib_resource->GetGPUVirtualAddress();
			frame_resource.d3d_ibv.SizeInBytes = static_cast<UINT>(d3d_resource_desc.Width);
			frame_resource.d3d_ibv.Format = DXGI_FORMAT_R32_UINT;

			// インデックスデータ設定
			UINT* index;
			frame_resource.d3d_ib_resource->Map(0, nullptr, reinterpret_cast<void**>(&index));

			// 四角形 を 三角形 ２つに展開
			// 0---1      0---1  4
			// |   |  →  |／  ／|
			// 2---3      2  3---5
			for (UINT i = 0; i < m_sprite_count * 4; i += 4)
			{
				index[0] = i + 0;
				index[1] = i + 1;
				index[2] = i + 2;
				index[3] = i + 2;
				index[4] = i + 1;
				index[5] = i + 3;
				index += 6;
			}
			frame_resource.d3d_ib_resource->Unmap(0, nullptr);
		}
	}
}
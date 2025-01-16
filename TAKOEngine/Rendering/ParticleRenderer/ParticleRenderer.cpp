//! @file ParticleRenderer.cpp
//! @note

#include "TAKOEngine\Runtime\tentacle_lib.h"
#include "TAKOEngine\Rendering\Misc.h"
#include "TAKOEngine\Rendering\Graphics.h"
#include "ParticleRenderer.h"

//**************************************************************
// @brief       コンストラクタ
// @param[in]   なし
// @return      なし
//**************************************************************
InjectionParticleRenderer::InjectionParticleRenderer()
{
	m_emitRate = 500;  //秒間500発
	m_emitWork = 0;

	// テクスチャ
	m_sprites[static_cast<int>(ParticleSprite::Noise)] = std::make_unique<SpriteDX12>(1, "Data/Sprites/noise.png");
	m_sprites[static_cast<int>(ParticleSprite::Anime)] = std::make_unique<SpriteDX12>(1, "Data/Sprites/Explosion01_5x5.png");

	// コンスタントバッファ生成
	CreateConstantBuffer();
}

//***********************************************************
// @brief       デストラクタ
// @param[in]   なし
// @return      なし
//***********************************************************
InjectionParticleRenderer::~InjectionParticleRenderer()
{
	if (T_GRAPHICS.GetParticleCompute(ComputeShaderDX12Id::Injection)->uav_descriptor != nullptr)
	{
		T_GRAPHICS.GetShaderResourceDescriptorHeap()->PushDescriptor(T_GRAPHICS.GetParticleCompute(ComputeShaderDX12Id::Injection)->uav_descriptor);
	}
	if (cbv_descriptor != nullptr)
	{
		T_GRAPHICS.GetShaderResourceDescriptorHeap()->PushDescriptor(cbv_descriptor);
	}
	if (cbv_data != nullptr)
	{
		d3d_cbv_resource->Unmap(0, nullptr);
		cbv_data = nullptr;
	}
}

//***********************************************************
// @brief       描画
// @param[in]   framBuffer     フレームバッファ
// @return      なし
//***********************************************************
void InjectionParticleRenderer::Render(FrameBufferManager* framBuffer)
{
	RenderContextDX12 rc;
	rc.d3d_command_list = framBuffer->GetCommandList();
	
	// コンスタントバッファ更新
	rc.particleData.cbv_descriptor = UpdateSceneConstantBuffer(rc);
	
	// 計算
	ComputeShader* m_compute = T_GRAPHICS.GetParticleCompute(ComputeShaderDX12Id::Injection);
	m_compute->Compute(rc, m_sprites[static_cast<int>(ParticleSprite::Noise)].get());

	// 発生インデックス更新
	cbv_data->emitIndex += cbv_data->emitCount;
	if (cbv_data->emitIndex >= m_compute->GetMaxParticle())
	{
		cbv_data->emitIndex = 0;
	}

	// シェーダー
	SpriteShaderDX12* shader = T_GRAPHICS.GetSpriteShaderDX12(SpriteShaderDX12Id::InjectionParticle);
	shader->Render(rc, m_sprites[static_cast<int>(ParticleSprite::Anime)].get());
}

//*************************************************************
// @brief       コンスタントバッファ生成
// @param[in]   なし
// @return      なし
//*************************************************************
void InjectionParticleRenderer::CreateConstantBuffer()
{
	Graphics& graphics = Graphics::Instance();
	ID3D12Device* d3d_device = graphics.GetDeviceDX12();

	HRESULT hr = S_OK;

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
	d3d_cbv_resource->SetName(L"InjectionParticleConstatBuffer");

	// ディスクリプタ取得
	cbv_descriptor = graphics.GetShaderResourceDescriptorHeap()->PopDescriptor();

	// コンスタントバッファビューの生成
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc{};
	cbv_desc.BufferLocation = d3d_cbv_resource->GetGPUVirtualAddress();
	cbv_desc.SizeInBytes    = static_cast<UINT>(resource_desc.Width);

	d3d_device->CreateConstantBufferView(
		&cbv_desc,
		cbv_descriptor->GetCpuHandle());

	hr = d3d_cbv_resource->Map(0, nullptr, reinterpret_cast<void**>(&cbv_data));
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

//*************************************************************
// @brief       コンスタントバッファ更新
// @param[in]   rc レンダーコンテキスト
// @return      const Descriptor*
//*************************************************************
const Descriptor* InjectionParticleRenderer::UpdateSceneConstantBuffer(const RenderContextDX12& rc)
{
	const Camera* camera = CameraManager::Instance().GetCamera();

	cbv_data->view       = camera->GetView();
	cbv_data->projection = camera->GetProjection();

	cbv_data->scale    = rc.particleData.scale;
	cbv_data->lifetime = rc.particleData.lifetime;

	// カラー
	cbv_data->startColor = rc.particleData.startColor;
	cbv_data->endColor   = rc.particleData.endColor;

	// ノイズ
	cbv_data->noiseSpeed = rc.particleData.noiseSpeed;
	cbv_data->noisePower = rc.particleData.noisePower;

	cbv_data->deltaTime    = T_TIMER.Delta();
	cbv_data->elapsedTime += cbv_data->deltaTime;

	// 発生管理
	m_emitWork += cbv_data->deltaTime;
	float e = 1.0f / m_emitRate;                 // 1発の発生時間
	int   n = static_cast<int>(m_emitWork / e);
	m_emitWork -= n * e;                         // 残り時間
	cbv_data->emitCount = n;

	return cbv_descriptor;
}
//! @file HitParticleRenderer.cpp
//! @note

#include "TAKOEngine\Runtime\tentacle_lib.h"
#include "TAKOEngine\Rendering\Misc.h"
#include "TAKOEngine\Rendering\Graphics.h"

#include "HitParticleRenderer.h"
#include <DirectXMath.h>

//**************************************************************
// @brief       コンストラクタ
// @param[in]   なし
// @return      なし
//**************************************************************
HitParticleRenderer::HitParticleRenderer(DirectX::XMFLOAT3 pos)
{
	// コンスタントバッファ生成
	CreateConstantBuffer();

	m_compute = std::make_unique<HitParticleCompute>(T_GRAPHICS.GetDeviceDX12(), pos);
}

//***********************************************************
// @brief       デストラクタ
// @param[in]   なし
// @return      なし
//***********************************************************
HitParticleRenderer::~HitParticleRenderer()
{
	if (m_compute->uav_descriptor != nullptr)
	{
		T_GRAPHICS.GetShaderResourceDescriptorHeap()->PushDescriptor(m_compute->uav_descriptor);
	}
	if (m_compute->srv_descriptor != nullptr)
	{
		T_GRAPHICS.GetShaderResourceDescriptorHeap()->PushDescriptor(m_compute->srv_descriptor);
	}
	if (cbv_descriptor != nullptr)
	{
		Graphics::Instance().GetShaderResourceDescriptorHeap()->PushDescriptor(cbv_descriptor);
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
void HitParticleRenderer::Render(FrameBufferManager* framBuffer)
{
	RenderContextDX12 rc;
	rc.d3d_command_list = framBuffer->GetCommandList();

	// コンスタントバッファ更新
	rc.hitParticleData.cbv_descriptor = UpdateSceneConstantBuffer(rc);

	// 計算
	m_compute->Compute(rc);

	// シェーダーリソースビュー更新
	rc.hitParticleData.srv_descriptor = m_compute->srv_descriptor;
	rc.hitParticleData.d3d_vbv = m_compute->d3d_vbv;
	rc.hitParticleData.maxParticle = m_compute->GetMaxParticle();

	// シェーダー
	SpriteShaderDX12* shader = T_GRAPHICS.GetSpriteShaderDX12(SpriteShaderDX12Id::HitParticle);
	shader->Render(rc, nullptr);
}

//*************************************************************
// @brief       コンスタントバッファ生成
// @param[in]   なし
// @return      なし
//*************************************************************
void HitParticleRenderer::CreateConstantBuffer()
{
	Graphics&     graphics   = Graphics::Instance();
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
	d3d_cbv_resource->SetName(L"HitParticleConstatBuffer");

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

//*************************************************************
// @brief       コンスタントバッファ更新
// @param[in]   rc レンダーコンテキスト
// @return      const Descriptor*
//*************************************************************
const Descriptor* HitParticleRenderer::UpdateSceneConstantBuffer(const RenderContextDX12& rc)
{
	const Camera* camera = CameraManager::Instance().GetCamera();

	cbv_data->view       = camera->GetView();
	cbv_data->projection = camera->GetProjection();

	cbv_data->time      += T_TIMER.Delta();
	cbv_data->delta_time = T_TIMER.Delta();
	cbv_data->max_particle_count = m_compute->GetMaxParticle();

	cbv_data->sprite_sheet_grid = { 3,2 };
	cbv_data->respawn_radius = 0.5f;

	cbv_data->emission_position.x = m_compute->GetPosition().x;
	cbv_data->emission_position.y = m_compute->GetPosition().y;
	cbv_data->emission_position.z = m_compute->GetPosition().z;
	cbv_data->emission_position.w = 1.0f;

	return cbv_descriptor;
}

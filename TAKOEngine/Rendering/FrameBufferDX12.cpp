//! @file FrameBufferDX12.cpp
//! @note

#include "Misc.h"
#include "Graphics.h"
#include "FrameBufferDX12.h"

//******************************************************************
// @brief       コンストラクタ
// @param[in]   device  ID3D12Device*
// @param[in]   resourceName[]  リソースの名前の配列　0:レンダーターゲット, 1:深度ステンシルバッファ
// @param[in]   width   レンダーターゲットの幅
// @param[in]   height  レンダーターゲットの高さ
// @param[in]   rtv_format  レンダーターゲットビューのフォーマット
// @param[in]   dsv_format  深度ステンシルビューのフォーマット
// @return      なし
//******************************************************************
FrameBufferDX12::FrameBufferDX12(ID3D12Device* device, const wchar_t* resourceName[2], UINT wight, UINT height, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format)
{
	m_width = wight;
	m_height = height;

	m_renderTargetTexture = std::make_unique<FrameBufferTexture>();
	m_depthStencilTexture = std::make_unique<FrameBufferTexture>();

	// レンダーターゲットとなるテクスチャを作成する
	CreateRenderTargetTexture(device, resourceName[0], wight, height, rtv_format);

	// 深度ステンシルバッファとなるテクスチャを作成
	if (dsv_format != DXGI_FORMAT_UNKNOWN)
	{
		CreateDepthStencilTexture(device, resourceName[1], wight, height, dsv_format);
	}

	// ディスクリプタヒープを取得
	CreateDescriptorHeap();

	//  ディスクリプタを作成する
	CreateDescriptor(device);
}

//******************************************************************
// @class     デストラクタ
// @param[in] なし
// @return    なし
//******************************************************************
FrameBufferDX12::~FrameBufferDX12()
{
}

//******************************************************************
// @brief       レンダーターゲットとなるテクスチャを作成する
// @param[in]   device        ID3D12Device*
// @param[in]   resourceName  リソースの名前
// @param[in]   width         レンダーターゲットの幅
// @param[in]   height        レンダーターゲットの高さ
// @param[in]   format        レンダーターゲットビューのフォーマット
// @return      なし
//******************************************************************
void FrameBufferDX12::CreateRenderTargetTexture(ID3D12Device* device, const wchar_t* resourceName, UINT wight, UINT height, DXGI_FORMAT format)
{
	D3D12_HEAP_PROPERTIES d3d_heap_props{};
	d3d_heap_props.Type                 = D3D12_HEAP_TYPE_DEFAULT;
	d3d_heap_props.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3d_heap_props.CreationNodeMask     = 1;
	d3d_heap_props.VisibleNodeMask      = 1;

	D3D12_RESOURCE_DESC d3d_resource_desc{};
	d3d_resource_desc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3d_resource_desc.Alignment          = 0;
	d3d_resource_desc.Width              = wight;
	d3d_resource_desc.Height             = height;
	d3d_resource_desc.DepthOrArraySize   = 1;
	d3d_resource_desc.MipLevels          = 1;
	d3d_resource_desc.Format             = format;
	d3d_resource_desc.SampleDesc.Count   = 1;
	d3d_resource_desc.SampleDesc.Quality = 0;
	d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3d_resource_desc.Flags  = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format   = format;
	clearValue.Color[0] = m_rtvClearColor[0];
	clearValue.Color[1] = m_rtvClearColor[1];
	clearValue.Color[2] = m_rtvClearColor[2];
	clearValue.Color[3] = m_rtvClearColor[3];

	HRESULT hr = device->CreateCommittedResource(
		&d3d_heap_props,
		D3D12_HEAP_FLAG_NONE,
		&d3d_resource_desc,
		D3D12_RESOURCE_STATE_COMMON,
		&clearValue,
		IID_PPV_ARGS(d3d_rtv_resource.GetAddressOf()));
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	d3d_rtv_resource->SetName(resourceName);
	
	m_renderTargetTexture->InitFromD3DResource(d3d_rtv_resource.Get());
}

//******************************************************************
// @brief       深度ステンシルバッファとなるテクスチャを作成
// @param[in]   device        ID3D12Device*
// @param[in]   resourceName  リソースの名前
// @param[in]   width         深度ステンシルの幅
// @param[in]   height        深度ステンシルの高さ
// @param[in]   format        深度ステンシルビューのフォーマット
// @return      なし
//******************************************************************
void FrameBufferDX12::CreateDepthStencilTexture(ID3D12Device* device, const wchar_t* resourceName, UINT wight, UINT height, DXGI_FORMAT format)
{
	D3D12_HEAP_PROPERTIES d3d_heap_props{};
	d3d_heap_props.Type                 = D3D12_HEAP_TYPE_DEFAULT;
	d3d_heap_props.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3d_heap_props.CreationNodeMask     = 1;
	d3d_heap_props.VisibleNodeMask      = 1;

	D3D12_RESOURCE_DESC d3d_resource_desc{};
	d3d_resource_desc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3d_resource_desc.Alignment          = 0;
	d3d_resource_desc.Width              = wight;
	d3d_resource_desc.Height             = height;
	d3d_resource_desc.DepthOrArraySize   = 1;
	d3d_resource_desc.MipLevels          = 1;
	d3d_resource_desc.Format             = format;
	d3d_resource_desc.SampleDesc.Count   = 1;
	d3d_resource_desc.SampleDesc.Quality = 0;
	d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3d_resource_desc.Flags  = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;

	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = format;
	clearValue.DepthStencil.Depth   = 1.0f;
	clearValue.DepthStencil.Stencil = 0;

	HRESULT hr = device->CreateCommittedResource(
		&d3d_heap_props,
		D3D12_HEAP_FLAG_NONE,
		&d3d_resource_desc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS(d3d_dsv_resource.GetAddressOf()));
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	d3d_dsv_resource->SetName(resourceName);

	m_depthStencilTexture->InitFromD3DResource(d3d_dsv_resource.Get());
}

//******************************************************************
// @class     ディスクリプタヒープを取得
// @param[in] なし
// @return    なし
//******************************************************************
void FrameBufferDX12::CreateDescriptorHeap()
{
	Graphics& graphics = Graphics::Instance();

	rtv_descriptor = graphics.GetRenderTargetDescriptorHeap()->PopDescriptor();
	
	if (d3d_dsv_resource)
	{
		dsv_descriptor = graphics.GetDepthStencilDescriptorHeap()->PopDescriptor();
	}
}

//******************************************************************
// @class     ディスクリプタを作成する
// @param[in] device  ID3D12Device*
// @return    なし
//******************************************************************
void FrameBufferDX12::CreateDescriptor(ID3D12Device* device)
{
	device->CreateRenderTargetView(m_renderTargetTexture->Get(), nullptr, rtv_descriptor->GetCpuHandle());
	
	if (d3d_dsv_resource)
	{
		device->CreateDepthStencilView(d3d_dsv_resource.Get(), nullptr, dsv_descriptor->GetCpuHandle());
	}
}


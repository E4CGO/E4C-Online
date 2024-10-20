#include "Graphics.h"

#include <WICTextureLoader12.h>

#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/LightManager.h"

#include "TAKOEngine/Rendering/Shaders/PhongShader.h"
#include "TAKOEngine/Rendering/Shaders/ToonShader.h"
#include "TAKOEngine/Rendering/Shaders/SkydomeShader.h"
#include "TAKOEngine/Rendering/Shaders/ShadowMapShader.h"

#include "TAKOEngine/Rendering/Shaders/UVScrollShader.h"
#include "TAKOEngine/Rendering/Shaders/MaskShader.h"
#include "TAKOEngine/Rendering/Shaders/ColorGradingShader.h"
#include "TAKOEngine/Rendering/Shaders/GaussianBlurShader.h"
#include "TAKOEngine/Rendering/Shaders/LuminanceExtractionShader.h"
#include "TAKOEngine/Rendering/Shaders/FinalpassShader.h"
#include "TAKOEngine/Rendering/Shaders/LambertShader.h"
#include "TAKOEngine/Rendering/Shaders/DeferredLightingShader.h"

Graphics* Graphics::s_instance = nullptr;

Graphics::~Graphics()
{
	FinishDX12();
}

void Graphics::FinishDX12()
{
	if (isDX12Active)
	{
		// IMGUI終了
		m_imgui_renderer->FinalizeDX12();
	}

	CommandQueue* command_queues[] =
	{
		&m_graphics_queue,
		&m_resource_queue
	};
	for (CommandQueue* command_queue : command_queues)
	{
		WaitIdle(*command_queue);

		if (command_queue->fence_event != NULL)
		{
			CloseHandle(command_queue->fence_event);
		}
	}
	for (FrameResource& frame_resource : m_frame_resources)
	{
		if (frame_resource.rtv_descriptor != nullptr)
		{
			m_rtv_descriptor_heap->PushDescriptor(frame_resource.rtv_descriptor);
		}
		if (frame_resource.dsv_descriptor != nullptr)
		{
			m_dsv_descriptor_heap->PushDescriptor(frame_resource.dsv_descriptor);
		}
		if (frame_resource.cbv_descriptor != nullptr)
		{
			m_shader_resource_descriptor_heap->PushDescriptor(frame_resource.cbv_descriptor);
		}
		if (frame_resource.cb_scene_data != nullptr)
		{
			frame_resource.d3d_cbv_resource->Unmap(0, nullptr);
		}
	}

	for (int i = 0; i < static_cast<int>(SamplerState::EnumCount); i++)
	{
		if (m_sampler[i]->GetDescriptor() != nullptr)
		{
			m_sampler_descriptor_heap->PushDescriptor(m_sampler[i]->GetDescriptor());
		}
	}
}

// 初期化
void Graphics::Initalize(HWND hWnd, UINT buffer_count)
{
	_ASSERT_EXPR(s_instance == nullptr, "already instantiated");
	s_instance = this;

	HRESULT hr = S_OK;
	// 画面サイズ
	RECT rc;
	GetClientRect(hWnd, &rc);
	UINT screenWidth = rc.right - rc.left;
	UINT screenHeight = rc.bottom - rc.top;

	this->screenWidth = static_cast<float>(screenWidth);
	this->screenHeight = static_cast<float>(screenHeight);

	m_buffer_count = buffer_count;

#ifdef USEDX12

	UINT dxgi_factory_flags = 0;

	{
		Microsoft::WRL::ComPtr<ID3D12Debug> d3d_debug;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(d3d_debug.GetAddressOf()))))
		{
			d3d_debug->EnableDebugLayer();
		}

		dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
	}

	{
		hr = CreateDXGIFactory2(
			dxgi_factory_flags,
			IID_PPV_ARGS(m_dxgi_factory.GetAddressOf())
		);
		COMPLETION_CHECK

			Microsoft::WRL::ComPtr<IDXGIAdapter1> dxgi_adapter;

		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgi_factory6;
		if (SUCCEEDED(m_dxgi_factory->QueryInterface(IID_PPV_ARGS(&dxgi_factory6))))
		{
			UINT adapter_index = 0;
			while (1)
			{
				HRESULT hr = dxgi_factory6->EnumAdapterByGpuPreference(
					adapter_index,
					DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
					IID_PPV_ARGS(dxgi_adapter.GetAddressOf())
				);
				if (FAILED(hr))
				{
					break;
				}

				DXGI_ADAPTER_DESC1 dxgi_adapter_desc;
				dxgi_adapter->GetDesc1(&dxgi_adapter_desc);

				if (dxgi_adapter_desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					continue;
				}

				hr = D3D12CreateDevice(
					dxgi_adapter.Get(),
					D3D_FEATURE_LEVEL_11_1,
					__uuidof(ID3D12Device),
					nullptr
				);
				if (SUCCEEDED(hr))
				{
					break;
				}

				adapter_index++;
			}
		}

		if (dxgi_adapter == nullptr)
		{
			hr = m_dxgi_factory->EnumAdapters1(0, dxgi_adapter.GetAddressOf());
			if (FAILED(hr))
			{
				hr = m_dxgi_factory->EnumWarpAdapter(IID_PPV_ARGS(dxgi_adapter.GetAddressOf()));
				COMPLETION_CHECK
			}
		}

		{
			DXGI_ADAPTER_DESC1 dxgi_adapter_desc;
			dxgi_adapter->GetDesc1(&dxgi_adapter_desc);

			OutputDebugStringW(dxgi_adapter_desc.Description);
			OutputDebugStringW(L"\n");

			const D3D_FEATURE_LEVEL d3d_feature_levels[] =
			{
				D3D_FEATURE_LEVEL_12_1,
				D3D_FEATURE_LEVEL_12_0,
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0,
			};

			for (D3D_FEATURE_LEVEL d3d_feature_level : d3d_feature_levels)
			{
				hr = D3D12CreateDevice(
					dxgi_adapter.Get(),
					d3d_feature_level,
					IID_PPV_ARGS(m_d3d_device.GetAddressOf())
				);

				if (SUCCEEDED(hr))
				{
					break;
				}
				COMPLETION_CHECK
					m_d3d_device->SetName(L"Device");
			}

			{
				D3D12_COMMAND_QUEUE_DESC d3d_command_queue_desc{};
				d3d_command_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
				d3d_command_queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
				d3d_command_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
				d3d_command_queue_desc.NodeMask = 0;

				hr = m_d3d_device->CreateCommandQueue(
					&d3d_command_queue_desc,
					IID_PPV_ARGS(m_graphics_queue.d3d_command_queue.GetAddressOf())
				);
				COMPLETION_CHECK
					m_graphics_queue.d3d_command_queue->SetName(L"GraphicsCommandQueue");

				hr = m_d3d_device->CreateFence(
					0,
					D3D12_FENCE_FLAG_NONE,
					IID_PPV_ARGS(&m_graphics_queue.d3d_fence)
				);
				COMPLETION_CHECK
					m_graphics_queue.d3d_fence->SetName(L"GraphicFence");

				m_graphics_queue.fence_event = CreateEvent(
					nullptr,
					FALSE,
					FALSE,
					nullptr
				);
			}
		}

		{
			DXGI_SWAP_CHAIN_DESC dxgi_swap_chain_desc = {};
			dxgi_swap_chain_desc.BufferCount = buffer_count;
			dxgi_swap_chain_desc.BufferDesc.Width = screenWidth;
			dxgi_swap_chain_desc.BufferDesc.Height = screenHeight;
			dxgi_swap_chain_desc.BufferDesc.Format = RenderTargetFormat;
			dxgi_swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			dxgi_swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
			dxgi_swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			dxgi_swap_chain_desc.OutputWindow = hWnd;
			dxgi_swap_chain_desc.SampleDesc.Count = 1;
			dxgi_swap_chain_desc.SampleDesc.Quality = 0;
			dxgi_swap_chain_desc.Windowed = true;
			dxgi_swap_chain_desc.Flags = SyncInterval == 0 ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

			Microsoft::WRL::ComPtr<IDXGISwapChain> dxgi_swap_chain;
			hr = m_dxgi_factory->CreateSwapChain(
				m_graphics_queue.d3d_command_queue.Get(),
				&dxgi_swap_chain_desc,
				dxgi_swap_chain.GetAddressOf()
			);
			COMPLETION_CHECK

				hr = dxgi_swap_chain->QueryInterface(IID_PPV_ARGS(m_dxgi_swap_chain.GetAddressOf()));
			COMPLETION_CHECK
		}

		m_rtv_descriptor_heap = std::make_unique<DescriptorHeap>(m_d3d_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 10);
		m_dsv_descriptor_heap = std::make_unique<DescriptorHeap>(m_d3d_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1000);
		m_shader_resource_descriptor_heap = std::make_unique<DescriptorHeap>(m_d3d_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1000);
		m_sampler_descriptor_heap = std::make_unique<DescriptorHeap>(m_d3d_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 1000);

		m_frame_resources.resize(buffer_count);
		for (UINT i = 0; i < buffer_count; ++i)
		{
			FrameResource& frame_resource = m_frame_resources.at(i);

			{
				hr = m_dxgi_swap_chain->GetBuffer(
					i,
					IID_PPV_ARGS(frame_resource.d3d_rtv_resource.GetAddressOf())
				);
				COMPLETION_CHECK
					frame_resource.d3d_rtv_resource->SetName(L"BackBuffer");

				frame_resource.rtv_descriptor = m_rtv_descriptor_heap->PopDescriptor();

				D3D12_RENDER_TARGET_VIEW_DESC d3d_rtv_desc;
				d3d_rtv_desc.Format = RenderTargetFormat;
				d3d_rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
				d3d_rtv_desc.Texture2D.MipSlice = 0;
				d3d_rtv_desc.Texture2D.PlaneSlice = 0;

				m_d3d_device->CreateRenderTargetView(
					frame_resource.d3d_rtv_resource.Get(),
					&d3d_rtv_desc,
					frame_resource.rtv_descriptor->GetCpuHandle()
				);
			}

			{
				D3D12_HEAP_PROPERTIES d3d_heap_props{};
				d3d_heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
				d3d_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
				d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
				d3d_heap_props.CreationNodeMask = 1;
				d3d_heap_props.VisibleNodeMask = 1;

				D3D12_RESOURCE_DESC d3d_resource_desc{};
				d3d_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				d3d_resource_desc.Alignment = 0;
				d3d_resource_desc.Width = screenWidth;
				d3d_resource_desc.Height = screenHeight;
				d3d_resource_desc.DepthOrArraySize = 1;
				d3d_resource_desc.MipLevels = 1;
				d3d_resource_desc.Format = DXGI_FORMAT_D32_FLOAT;
				d3d_resource_desc.SampleDesc.Count = 1;
				d3d_resource_desc.SampleDesc.Quality = 0;
				d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
				d3d_resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

				D3D12_CLEAR_VALUE d3d_dsv_clear_value{};
				d3d_dsv_clear_value.Format = DepthStencilFormat;
				d3d_dsv_clear_value.DepthStencil.Depth = 1.0f;
				d3d_dsv_clear_value.DepthStencil.Stencil = 0;

				HRESULT hr = m_d3d_device->CreateCommittedResource(
					&d3d_heap_props,
					D3D12_HEAP_FLAG_NONE,
					&d3d_resource_desc,
					D3D12_RESOURCE_STATE_DEPTH_WRITE,
					&d3d_dsv_clear_value,
					IID_PPV_ARGS(frame_resource.d3d_dsv_resource.GetAddressOf())
				);
				COMPLETION_CHECK

					frame_resource.d3d_dsv_resource->SetName(L"DepthStencilView");

				frame_resource.dsv_descriptor = m_dsv_descriptor_heap->PopDescriptor();

				D3D12_DEPTH_STENCIL_VIEW_DESC d3d_dsv_desc{};
				d3d_dsv_desc.Format = DepthStencilFormat;
				d3d_dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				d3d_dsv_desc.Flags = D3D12_DSV_FLAG_NONE;

				m_d3d_device->CreateDepthStencilView(
					frame_resource.d3d_dsv_resource.Get(),
					&d3d_dsv_desc,
					frame_resource.dsv_descriptor->GetCpuHandle()
				);
			}

			{
				D3D12_HEAP_PROPERTIES d3d_head_props{};
				d3d_head_props.Type = D3D12_HEAP_TYPE_UPLOAD;
				d3d_head_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
				d3d_head_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
				d3d_head_props.CreationNodeMask = 1;
				d3d_head_props.VisibleNodeMask = 1;

				D3D12_RESOURCE_DESC d3d_resource_desc{};
				d3d_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
				d3d_resource_desc.Alignment = 0;
				d3d_resource_desc.Width = ((sizeof(CbScene)) + 255) & ~255;
				d3d_resource_desc.Height = 1;
				d3d_resource_desc.DepthOrArraySize = 1;
				d3d_resource_desc.MipLevels = 1;
				d3d_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
				d3d_resource_desc.SampleDesc.Count = 1;
				d3d_resource_desc.SampleDesc.Quality = 0;
				d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
				d3d_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

				hr = m_d3d_device->CreateCommittedResource(
					&d3d_head_props,
					D3D12_HEAP_FLAG_NONE,
					&d3d_resource_desc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(frame_resource.d3d_cbv_resource.GetAddressOf())
				);
				COMPLETION_CHECK
					frame_resource.d3d_cbv_resource->SetName(L"SceneConstatBuffer");

				frame_resource.cbv_descriptor = m_shader_resource_descriptor_heap->PopDescriptor();

				D3D12_CONSTANT_BUFFER_VIEW_DESC d3d_cbv_desc;
				d3d_cbv_desc.BufferLocation = frame_resource.d3d_cbv_resource->GetGPUVirtualAddress();
				d3d_cbv_desc.SizeInBytes = static_cast<UINT>(d3d_resource_desc.Width);
				m_d3d_device->CreateConstantBufferView(
					&d3d_cbv_desc,
					frame_resource.cbv_descriptor->GetCpuHandle()
				);

				hr = frame_resource.d3d_cbv_resource->Map(0, nullptr, reinterpret_cast<void**>(&frame_resource.cb_scene_data));
				COMPLETION_CHECK
			}

			{
				hr = m_d3d_device->CreateCommandAllocator(
					D3D12_COMMAND_LIST_TYPE_DIRECT,
					IID_PPV_ARGS(frame_resource.d3d_command_allocator.GetAddressOf())
				);
				COMPLETION_CHECK
					frame_resource.d3d_command_allocator->SetName(L"GraphicsCommandAllocator");
			}

			{
				hr = m_d3d_device->CreateCommandList(
					0,
					D3D12_COMMAND_LIST_TYPE_DIRECT,
					frame_resource.d3d_command_allocator.Get(),
					nullptr,
					IID_PPV_ARGS(frame_resource.d3d_command_list.GetAddressOf())
				);
				COMPLETION_CHECK
					frame_resource.d3d_command_list->Close();
				frame_resource.d3d_command_list->SetName(L"GraphicsCommandList");
			}
		}

		{
			{
				D3D12_COMMAND_QUEUE_DESC d3d_command_queue_desc{};
				d3d_command_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
				d3d_command_queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
				d3d_command_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
				d3d_command_queue_desc.NodeMask = 0;

				hr = m_d3d_device->CreateCommandQueue(
					&d3d_command_queue_desc,
					IID_PPV_ARGS(m_resource_queue.d3d_command_queue.GetAddressOf())
				);
				COMPLETION_CHECK
					m_resource_queue.d3d_command_queue->SetName(L"ResourceCommandQueue");

				hr = m_d3d_device->CreateFence(
					0,
					D3D12_FENCE_FLAG_NONE,
					IID_PPV_ARGS(&m_resource_queue.d3d_fence)
				);
				COMPLETION_CHECK
					m_resource_queue.d3d_fence->SetName(L"ResourceFence");

				m_resource_queue.fence_event = CreateEvent(
					nullptr,
					FALSE,
					FALSE,
					nullptr
				);
			}

			{
				hr = m_d3d_device->CreateCommandAllocator(
					D3D12_COMMAND_LIST_TYPE_DIRECT,
					IID_PPV_ARGS(m_d3d_resource_command_allocator.GetAddressOf())
				);
				COMPLETION_CHECK
					m_d3d_resource_command_allocator->SetName(L"ResourceCommandAllocator");
			}

			{
				hr = m_d3d_device->CreateCommandList(
					0,
					D3D12_COMMAND_LIST_TYPE_DIRECT,
					m_d3d_resource_command_allocator.Get(),
					nullptr,
					IID_PPV_ARGS(m_d3d_resource_command_list.GetAddressOf())
				);
				COMPLETION_CHECK
					m_d3d_resource_command_list->SetName(L"ResourceCommandList");
			}
		}

		// IMGUI
		{
			if (isDX12Active)
			{
				m_imgui_renderer = std::make_unique<ImGuiRenderer>(hWnd, m_d3d_device.Get(), RenderTargetFormat, BufferCount, m_shader_resource_descriptor_heap);
			}
		}
	}

#endif // USEDX12

	// ADAPTER
	IDXGIFactory* factory;
	CreateDXGIFactory(IID_PPV_ARGS(&factory));
	IDXGIAdapter* adapter;
	for (UINT adapter_index = 0; S_OK == factory->EnumAdapters(adapter_index, &adapter); ++adapter_index) {
		DXGI_ADAPTER_DESC adapter_desc;
		adapter->GetDesc(&adapter_desc);
		if (adapter_desc.VendorId == 0x1002/*AMD*/ || adapter_desc.VendorId == 0x10DE/*NVIDIA*/)
		{
			break;
		}
		adapter->Release();
	}
	if (adapter == nullptr)
	{
		factory->EnumAdapters(0, &adapter);
		DXGI_ADAPTER_DESC adapter_desc;
		adapter->GetDesc(&adapter_desc);
	}
	factory->Release();

	// デバイス&スワップチェーン生成
	{
		UINT createDeviceFlags = 0;
#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
		};

		// スワップチェーン設定
		DXGI_SWAP_CHAIN_DESC swapchainDesc;
		{
			swapchainDesc.BufferDesc.Width = screenWidth;
			swapchainDesc.BufferDesc.Height = screenHeight;
			swapchainDesc.BufferDesc.RefreshRate.Numerator = 60;
			swapchainDesc.BufferDesc.RefreshRate.Denominator = 1;

			swapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapchainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			swapchainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			swapchainDesc.SampleDesc.Count = 1;
			swapchainDesc.SampleDesc.Quality = 0;
			swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapchainDesc.BufferCount = 1;
			swapchainDesc.OutputWindow = hWnd;
			swapchainDesc.Windowed = TRUE;
			swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			swapchainDesc.Flags = 0;
		}

		D3D_FEATURE_LEVEL featureLevel;

		// デバイス&スワップチェーン生成
		hr = D3D11CreateDeviceAndSwapChain(
			// nullptr,l/
			adapter,
			//D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_UNKNOWN,
			nullptr,
			createDeviceFlags,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			&swapchainDesc,
			swapchain.GetAddressOf(),
			device.GetAddressOf(),
			&featureLevel,
			immediateContext.GetAddressOf()
		);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	adapter->Release();

	// フレームバッファ作成
	frameBuffers[static_cast<int>(FrameBufferId::Display)]      = std::make_unique<FrameBuffer>(device.Get(), swapchain.Get());
	frameBuffers[static_cast<int>(FrameBufferId::Scene)]        = std::make_unique<FrameBuffer>(device.Get(), screenWidth, screenHeight);
	frameBuffers[static_cast<int>(FrameBufferId::Luminance)]    = std::make_unique<FrameBuffer>(device.Get(), screenWidth / 2, screenHeight / 2);
	frameBuffers[static_cast<int>(FrameBufferId::GaussianBlur)] = std::make_unique<FrameBuffer>(device.Get(), screenWidth / 2, screenHeight / 2);
	frameBuffers[static_cast<int>(FrameBufferId::Normal)]       = std::make_unique<FrameBuffer>(device.Get(), screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
	frameBuffers[static_cast<int>(FrameBufferId::Position)]     = std::make_unique<FrameBuffer>(device.Get(), screenWidth, screenHeight, DXGI_FORMAT_R32G32B32A32_FLOAT);

	// レンダーステート作成
	renderState       = std::make_unique<RenderState>(device.Get());
	m_renderStateDX12 = std::make_unique<RenderStateDX12>();

	//サンプラーステート作成
	m_sampler[static_cast<int>(SamplerState::PointWrap)]    = std::make_unique<SamplerManager>(SamplerState::PointWrap);
	m_sampler[static_cast<int>(SamplerState::PointClamp)]   = std::make_unique<SamplerManager>(SamplerState::PointClamp);
	m_sampler[static_cast<int>(SamplerState::LinearWrap)]   = std::make_unique<SamplerManager>(SamplerState::LinearWrap);
	m_sampler[static_cast<int>(SamplerState::LinearClamp)]  = std::make_unique<SamplerManager>(SamplerState::LinearClamp);
	m_sampler[static_cast<int>(SamplerState::LinearBorder)] = std::make_unique<SamplerManager>(SamplerState::LinearBorder);
	m_sampler[static_cast<int>(SamplerState::ShadowMap)]    = std::make_unique<SamplerManager>(SamplerState::ShadowMap);

	// ギズモ生成
	gizmos = std::make_unique<Gizmos>(device.Get());

	// モデルシェーダー生成
	modelShaders[static_cast<int>(ModelShaderId::Phong)]     = std::make_unique<PhongShader>(device.Get());
	modelShaders[static_cast<int>(ModelShaderId::Toon)]      = std::make_unique<ToonShader>(device.Get());
	modelShaders[static_cast<int>(ModelShaderId::Skydome)]   = std::make_unique<SkydomeShader>(device.Get());
	modelShaders[static_cast<int>(ModelShaderId::ShadowMap)] = std::make_unique<ShadowMapShader>(device.Get());

	// DX12のモデルシェーダー生成
	dx12_modelshaders[static_cast<int>(ModelShaderDX12Id::Lambert)] = std::make_unique<LambertShader>(m_d3d_device.Get());
	dx12_modelshaders[static_cast<int>(ModelShaderDX12Id::Phong)]   = std::make_unique<PhongShaderDX12>(m_d3d_device.Get());
	dx12_modelshaders[static_cast<int>(ModelShaderDX12Id::Toon)]    = std::make_unique<ToonShaderDX12>(m_d3d_device.Get());

	// スプライトシェーダー生成
	spriteShaders[static_cast<int>(SpriteShaderId::Default)]             = std::make_unique<DefaultSpriteShader>(device.Get());
	spriteShaders[static_cast<int>(SpriteShaderId::UVScroll)]            = std::make_unique<UVScrollShader>(device.Get());
	spriteShaders[static_cast<int>(SpriteShaderId::Mask)]                = std::make_unique<MaskShader>(device.Get());
	spriteShaders[static_cast<int>(SpriteShaderId::ColorGrading)]        = std::make_unique<ColorGradingShader>(device.Get());
	spriteShaders[static_cast<int>(SpriteShaderId::GaussianBlur)]        = std::make_unique<GaussianBlurShader>(device.Get());
	spriteShaders[static_cast<int>(SpriteShaderId::LuminanceExtraction)] = std::make_unique<LuminanceExtractionShader>(device.Get());
	spriteShaders[static_cast<int>(SpriteShaderId::Finalpass)]           = std::make_unique<FinalpassShader>(device.Get());
	spriteShaders[static_cast<int>(SpriteShaderId::Deferred)]            = std::make_unique<DeferredLightingShader>(device.Get());

	// レンダラ
	debugRenderer = std::make_unique<DebugRenderer>(device.Get());
	lineRenderer  = std::make_unique<LineRenderer>(device.Get(), 1024);

	//スキニング
	m_skinning_pipeline = std::make_unique<SkinningPipeline>(m_d3d_device.Get());
}

void Graphics::Present(UINT syncInterval)
{
	swapchain->Present(syncInterval, 0);
}

DirectX::XMFLOAT3 Graphics::GetScreenPosition(
	const DirectX::XMFLOAT3 worldPosition,
	const D3D11_VIEWPORT& viewport,
	const  DirectX::XMMATRIX& View,
	const  DirectX::XMMATRIX& Projection,
	const DirectX::XMMATRIX World
)
{
	DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(
		DirectX::XMVectorSet(worldPosition.x, worldPosition.y, worldPosition.z, 0),
		viewport.TopLeftX,
		viewport.TopLeftY,
		viewport.Width,
		viewport.Height,
		viewport.MinDepth,
		viewport.MaxDepth,
		Projection,
		View,
		World
	);
	DirectX::XMFLOAT3 screenPosition;
	DirectX::XMStoreFloat3(&screenPosition, ScreenPosition);
	return screenPosition;
}
DirectX::XMFLOAT3 Graphics::GetScreenPosition(const DirectX::XMFLOAT3 worldPosition)
{
	// ビューボード
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	immediateContext.Get()->RSGetViewports(&numViewports, &viewport);

	// 変換行列
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&Camera::Instance().GetView());
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&Camera::Instance().GetProjection());
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	return GetScreenPosition(worldPosition, viewport, View, Projection, World);
}

void Graphics::WaitIdle()
{
	WaitIdle(m_graphics_queue);
	WaitIdle(m_resource_queue);
}

void Graphics::WaitIdle(CommandQueue& command_queue)
{
	command_queue.d3d_command_queue->Signal(command_queue.d3d_fence.Get(), ++command_queue.fence_value);
	if (command_queue.d3d_fence->GetCompletedValue() < command_queue.fence_value)
	{
		// コマンド完了時にイベントが発生するように設定
		command_queue.d3d_fence->SetEventOnCompletion(command_queue.fence_value, command_queue.fence_event);

		// イベントが発生するまで待つ
		WaitForSingleObject(command_queue.fence_event, INFINITE);
	}
}

void Graphics::Execute()
{
	// コマンド終了まで待つ
	WaitIdle(m_graphics_queue);

	UINT frame_buffer_index = m_dxgi_swap_chain->GetCurrentBackBufferIndex();
	FrameResource& frame_resource = m_frame_resources.at(frame_buffer_index);

	// コマンド実行
	ID3D12CommandList* d3d_command_lists[] =
	{
		frame_resource.d3d_command_list.Get()
	};

	m_graphics_queue.d3d_command_queue->ExecuteCommandLists(_countof(d3d_command_lists), d3d_command_lists);

	// 画面表示
	m_dxgi_swap_chain->Present(SyncInterval, SyncInterval == 0 ? DXGI_PRESENT_ALLOW_TEARING : 0);
}

ID3D12GraphicsCommandList* Graphics::Begin()
{
	UINT frame_buffer_index = m_dxgi_swap_chain->GetCurrentBackBufferIndex();
	FrameResource& frame_resource = m_frame_resources.at(frame_buffer_index);

	// コマンドのリセット
	frame_resource.d3d_command_allocator->Reset();
	frame_resource.d3d_command_list->Reset(frame_resource.d3d_command_allocator.Get(), nullptr);

	// ディスクリプタヒープをあらかじめ設定
	ID3D12DescriptorHeap* d3d_descriptor_heaps[] =
	{
		m_shader_resource_descriptor_heap->GetD3DDescriptorHeap(),
		m_sampler_descriptor_heap->GetD3DDescriptorHeap()
	};
	frame_resource.d3d_command_list->SetDescriptorHeaps(_countof(d3d_descriptor_heaps), d3d_descriptor_heaps);

	// レンダーターゲット設定
	{
		// ビューポートの設定
		D3D12_VIEWPORT d3d_viewport;
		d3d_viewport.TopLeftX = 0;
		d3d_viewport.TopLeftY = 0;
		d3d_viewport.Width = this->screenWidth;
		d3d_viewport.Height = this->screenHeight;
		d3d_viewport.MinDepth = 0.0f;
		d3d_viewport.MaxDepth = 1.0f;
		frame_resource.d3d_command_list->RSSetViewports(1, &d3d_viewport);

		// シザーの設定
		D3D12_RECT scissor_rect;
		scissor_rect.left = 0;
		scissor_rect.top = 0;
		scissor_rect.right = static_cast<LONG>(this->screenWidth);
		scissor_rect.bottom = static_cast<LONG>(this->screenHeight);
		frame_resource.d3d_command_list->RSSetScissorRects(1, &scissor_rect);

		// 状態遷移バリアを張る
		D3D12_RESOURCE_BARRIER d3d_resource_barrier{};
		d3d_resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		d3d_resource_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		d3d_resource_barrier.Transition.pResource = frame_resource.d3d_rtv_resource.Get();
		d3d_resource_barrier.Transition.Subresource = 0;
		d3d_resource_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		d3d_resource_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		frame_resource.d3d_command_list->ResourceBarrier(1, &d3d_resource_barrier);

		// レンダーターゲット設定
		FLOAT clear_color[4] = { 0, 0, 1, 1 };
		D3D12_CPU_DESCRIPTOR_HANDLE d3d_rtv_handle = frame_resource.rtv_descriptor->GetCpuHandle();
		D3D12_CPU_DESCRIPTOR_HANDLE d3d_dsv_handle = frame_resource.dsv_descriptor->GetCpuHandle();
		frame_resource.d3d_command_list->OMSetRenderTargets(1, &d3d_rtv_handle, false, &d3d_dsv_handle);
		frame_resource.d3d_command_list->ClearRenderTargetView(d3d_rtv_handle, clear_color, 0, nullptr);
		frame_resource.d3d_command_list->ClearDepthStencilView(d3d_dsv_handle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	}
	return frame_resource.d3d_command_list.Get();
}

void Graphics::End()
{
	UINT frame_buffer_index = m_dxgi_swap_chain->GetCurrentBackBufferIndex();
	FrameResource& frame_resource = m_frame_resources.at(frame_buffer_index);

	// 状態遷移バリアを張る
	{
		D3D12_RESOURCE_BARRIER d3d_resource_barrier{};
		d3d_resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		d3d_resource_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		d3d_resource_barrier.Transition.pResource = frame_resource.d3d_rtv_resource.Get();
		d3d_resource_barrier.Transition.Subresource = 0;
		d3d_resource_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		d3d_resource_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

		frame_resource.d3d_command_list->ResourceBarrier(1, &d3d_resource_barrier);
	}

	frame_resource.d3d_command_list->Close();
}

//TODO : UpdataConstantBuffer
const Descriptor* Graphics::UpdateSceneConstantBuffer(const Camera& camera, const DirectX::XMFLOAT3& light_direction)
{
	LightManager& ligtManager = LightManager::Instance();

	UINT       frame_buffer_index = m_dxgi_swap_chain->GetCurrentBackBufferIndex();
	FrameResource& frame_resource = m_frame_resources.at(frame_buffer_index);

	DirectX::XMMATRIX View           = DirectX::XMLoadFloat4x4(&camera.GetView());
	DirectX::XMMATRIX Projection     = DirectX::XMLoadFloat4x4(&camera.GetProjection());
	DirectX::XMMATRIX ViewProjection = DirectX::XMMatrixMultiply(View, Projection);
	DirectX::XMStoreFloat4x4(&frame_resource.cb_scene_data->view_projection, ViewProjection);

	// カメラ
	frame_resource.cb_scene_data->camera_position.x = camera.GetEye().x;
	frame_resource.cb_scene_data->camera_position.y = camera.GetEye().y;
	frame_resource.cb_scene_data->camera_position.z = camera.GetEye().z;

	// ライト情報
	frame_resource.cb_scene_data->ambientLightColor = ligtManager.GetAmbientColor();

	for (Light* light : ligtManager.GetAllLight())
	{
		switch (light->GetLightType())
		{
		case	LightType::Directional:
		{
			frame_resource.cb_scene_data->directionalLightData.direction.x = light->GetDirection().x;
			frame_resource.cb_scene_data->directionalLightData.direction.y = light->GetDirection().y;
			frame_resource.cb_scene_data->directionalLightData.direction.z = light->GetDirection().z;
			frame_resource.cb_scene_data->directionalLightData.direction.w = 0.0f;
			frame_resource.cb_scene_data->directionalLightData.color = light->GetColor();
			break;
		}
		case	LightType::Point:
		{
			if (frame_resource.cb_scene_data->pointLightCount >= PointLightMax) break;
			
			frame_resource.cb_scene_data->pointLightData[frame_resource.cb_scene_data->pointLightCount].position.x = light->GetPosition().x;
			frame_resource.cb_scene_data->pointLightData[frame_resource.cb_scene_data->pointLightCount].position.y = light->GetPosition().y;
			frame_resource.cb_scene_data->pointLightData[frame_resource.cb_scene_data->pointLightCount].position.z = light->GetPosition().z;
			frame_resource.cb_scene_data->pointLightData[frame_resource.cb_scene_data->pointLightCount].position.w = 1.0f;
			frame_resource.cb_scene_data->pointLightData[frame_resource.cb_scene_data->pointLightCount].color = light->GetColor();
			frame_resource.cb_scene_data->pointLightData[frame_resource.cb_scene_data->pointLightCount].range = light->GetRange();
			++frame_resource.cb_scene_data->pointLightCount;
			break;
		}
		case	LightType::Spot:
		{
			if (frame_resource.cb_scene_data->spotLightCount >= SpotLightMax) break;

			frame_resource.cb_scene_data->spotLightData[frame_resource.cb_scene_data->spotLightCount].position.x = light->GetPosition().x;
			frame_resource.cb_scene_data->spotLightData[frame_resource.cb_scene_data->spotLightCount].position.y = light->GetPosition().y;
			frame_resource.cb_scene_data->spotLightData[frame_resource.cb_scene_data->spotLightCount].position.z = light->GetPosition().z;
			frame_resource.cb_scene_data->spotLightData[frame_resource.cb_scene_data->spotLightCount].position.w = 1.0f;
			frame_resource.cb_scene_data->spotLightData[frame_resource.cb_scene_data->spotLightCount].direction.x = light->GetDirection().x;
			frame_resource.cb_scene_data->spotLightData[frame_resource.cb_scene_data->spotLightCount].direction.y = light->GetDirection().y;
			frame_resource.cb_scene_data->spotLightData[frame_resource.cb_scene_data->spotLightCount].direction.z = light->GetDirection().z;
			frame_resource.cb_scene_data->spotLightData[frame_resource.cb_scene_data->spotLightCount].direction.w = 0.0f;
			frame_resource.cb_scene_data->spotLightData[frame_resource.cb_scene_data->spotLightCount].color = light->GetColor();
			frame_resource.cb_scene_data->spotLightData[frame_resource.cb_scene_data->spotLightCount].range = light->GetRange();
			frame_resource.cb_scene_data->spotLightData[frame_resource.cb_scene_data->spotLightCount].innerCorn = light->GetInnerCorn();
			frame_resource.cb_scene_data->spotLightData[frame_resource.cb_scene_data->spotLightCount].outerCorn = light->GetOuterCorn();
			++frame_resource.cb_scene_data->spotLightCount;
			break;
		}
		}
	}

	return frame_resource.cbv_descriptor;
}

HRESULT Graphics::LoadTexture(const char* filename, ID3D12Resource** d3d_resource)
{
	// マルチバイト文字からワイド文字へ変換
	wchar_t wfilename[256];
	::MultiByteToWideChar(CP_ACP, 0, filename, -1, wfilename, 256);

	// テクスチャファイル読み込み
	std::unique_ptr<uint8_t[]> image;
	D3D12_SUBRESOURCE_DATA d3d_subresource_data;
	HRESULT hr = DirectX::LoadWICTextureFromFile(
		m_d3d_device.Get(),
		wfilename,
		d3d_resource,
		image,
		d3d_subresource_data);
	if (FAILED(hr))
	{
		return hr;
	}

	// 読み込んだ画像をリソースにコピー
	D3D12_RESOURCE_DESC d3d_resource_desc = (*d3d_resource)->GetDesc();
	hr = CopyImage(
		reinterpret_cast<const BYTE*>(d3d_subresource_data.pData),
		static_cast<UINT>(d3d_resource_desc.Width),
		d3d_resource_desc.Height,
		d3d_resource_desc.Format,
		*d3d_resource
	);

	return hr;
}

HRESULT Graphics::CreateTexture(const BYTE* pixels, UINT width, UINT height, DXGI_FORMAT format, ID3D12Resource** d3d_resource)
{
	D3D12_HEAP_PROPERTIES d3d_heap_props = {};
	d3d_heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3d_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3d_heap_props.CreationNodeMask = 1;
	d3d_heap_props.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC d3d_resource_desc = {};
	d3d_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3d_resource_desc.Alignment = 0;
	d3d_resource_desc.Width = width;
	d3d_resource_desc.Height = height;
	d3d_resource_desc.DepthOrArraySize = 1;
	d3d_resource_desc.MipLevels = 1;
	d3d_resource_desc.Format = format;
	d3d_resource_desc.SampleDesc.Count = 1;
	d3d_resource_desc.SampleDesc.Quality = 0;
	d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3d_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	HRESULT hr = m_d3d_device->CreateCommittedResource(
		&d3d_heap_props,
		D3D12_HEAP_FLAG_NONE,
		&d3d_resource_desc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(d3d_resource)
	);
	if (FAILED(hr))
	{
		return hr;
	}

	return CopyImage(pixels, width, height, format, *d3d_resource);
}

HRESULT Graphics::CreateDummyTexture(ID3D12Resource** d3d_resource)
{
	const UINT width = 8;
	const UINT height = 8;
	UINT pixels[width * height];
	::memset(pixels, 0xFF, sizeof(pixels));

	// ヒーププロパティの設定
	D3D12_HEAP_PROPERTIES d3d_heap_props = {};
	d3d_heap_props.Type = D3D12_HEAP_TYPE_CUSTOM;
	d3d_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	d3d_heap_props.CreationNodeMask = 1;
	d3d_heap_props.VisibleNodeMask = 1;

	// リソースの設定
	D3D12_RESOURCE_DESC d3d_resource_desc = {};
	d3d_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3d_resource_desc.Alignment = 0;
	d3d_resource_desc.Width = width;
	d3d_resource_desc.Height = height;
	d3d_resource_desc.DepthOrArraySize = 1;
	d3d_resource_desc.MipLevels = 1;
	d3d_resource_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3d_resource_desc.SampleDesc.Count = 1;
	d3d_resource_desc.SampleDesc.Quality = 0;
	d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3d_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// リソースの生成
	HRESULT hr = m_d3d_device->CreateCommittedResource(
		&d3d_heap_props,
		D3D12_HEAP_FLAG_NONE,
		&d3d_resource_desc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(d3d_resource)
	);
	if (FAILED(hr))
	{
		return hr;
	}

	// イメージをテクスチャに書き込み
	return (*d3d_resource)->WriteToSubresource(
		0,
		nullptr,
		pixels,
		sizeof(UINT) * width,
		sizeof(UINT) * width * height
	);
}

HRESULT Graphics::CopyBuffer(ID3D12Resource* d3d_src_resource, ID3D12Resource* d3d_dst_resource)
{
	D3D12_RESOURCE_BARRIER d3d_resource_barrier = {};
	d3d_resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3d_resource_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3d_resource_barrier.Transition.pResource = d3d_dst_resource;
	d3d_resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	d3d_resource_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
	d3d_resource_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
	m_d3d_resource_command_list->ResourceBarrier(1, &d3d_resource_barrier);

	m_d3d_resource_command_list->CopyResource(d3d_dst_resource, d3d_src_resource);

	d3d_resource_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	d3d_resource_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	m_d3d_resource_command_list->ResourceBarrier(1, &d3d_resource_barrier);

	m_d3d_resource_command_list->Close();

	ID3D12CommandList* d3d_command_lists[] =
	{
		m_d3d_resource_command_list.Get()
	};
	m_resource_queue.d3d_command_queue->ExecuteCommandLists(_countof(d3d_command_lists), d3d_command_lists);

	WaitIdle(m_resource_queue);

	m_d3d_resource_command_allocator->Reset();
	m_d3d_resource_command_list->Reset(m_d3d_resource_command_allocator.Get(), nullptr);

	return S_OK;
}

HRESULT Graphics::CopyImage(const BYTE* pixels, UINT width, UINT height, DXGI_FORMAT format, ID3D12Resource* d3d_resource)
{
	HRESULT hr = S_OK;

	// アップロード用テクスチャを作成
	UINT bpp = BitsPerPixel(format);
	UINT rowPitch = (width * bpp) >> 3;
	UINT uploadPitch = (rowPitch + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u);
	UINT uploadSize = height * uploadPitch;

	// ヒーププロパティの設定
	D3D12_HEAP_PROPERTIES d3d_heap_props = {};
	d3d_heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
	d3d_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3d_heap_props.CreationNodeMask = 1;
	d3d_heap_props.VisibleNodeMask = 1;

	// リソースの設定
	D3D12_RESOURCE_DESC d3d_resource_desc = {};
	d3d_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	d3d_resource_desc.Alignment = 0;
	d3d_resource_desc.Width = uploadSize;
	d3d_resource_desc.Height = 1;
	d3d_resource_desc.DepthOrArraySize = 1;
	d3d_resource_desc.MipLevels = 1;
	d3d_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
	d3d_resource_desc.SampleDesc.Count = 1;
	d3d_resource_desc.SampleDesc.Quality = 0;
	d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	d3d_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// リソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> d3d_upload_resource;
	hr = m_d3d_device->CreateCommittedResource(
		&d3d_heap_props,
		D3D12_HEAP_FLAG_NONE,
		&d3d_resource_desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(d3d_upload_resource.GetAddressOf())
	);
	if (FAILED(hr))
	{
		return hr;
	}

	// アップロード用バッファにイメージをコピー
	void* mapped = nullptr;
	D3D12_RANGE d3d_range = {};
	d3d_range.Begin = 0;
	d3d_range.End = uploadSize;
	hr = d3d_upload_resource->Map(0, &d3d_range, &mapped);
	if (FAILED(hr))
	{
		return hr;
	}
	for (UINT y = 0; y < height; y++)
	{
		memcpy((void*)((uintptr_t)mapped + y * uploadPitch), pixels + y * rowPitch, rowPitch);
	}
	d3d_upload_resource->Unmap(0, &d3d_range);

	// アップロード用バッファからテクスチャリソースにイメージをコピー
	D3D12_TEXTURE_COPY_LOCATION d3d_src_texture_copy_location = {};
	d3d_src_texture_copy_location.pResource = d3d_upload_resource.Get();
	d3d_src_texture_copy_location.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	d3d_src_texture_copy_location.PlacedFootprint.Footprint.Format = format;
	d3d_src_texture_copy_location.PlacedFootprint.Footprint.Width = width;
	d3d_src_texture_copy_location.PlacedFootprint.Footprint.Height = height;
	d3d_src_texture_copy_location.PlacedFootprint.Footprint.Depth = 1;
	d3d_src_texture_copy_location.PlacedFootprint.Footprint.RowPitch = uploadPitch;

	D3D12_TEXTURE_COPY_LOCATION d3d_dst_texture_copy_location = {};
	d3d_dst_texture_copy_location.pResource = d3d_resource;
	d3d_dst_texture_copy_location.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	d3d_dst_texture_copy_location.SubresourceIndex = 0;

	D3D12_RESOURCE_BARRIER d3d_resource_barrier = {};
	d3d_resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3d_resource_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3d_resource_barrier.Transition.pResource = d3d_resource;
	d3d_resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	d3d_resource_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	d3d_resource_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	m_d3d_resource_command_list->CopyTextureRegion(&d3d_dst_texture_copy_location, 0, 0, 0, &d3d_src_texture_copy_location, nullptr);
	m_d3d_resource_command_list->ResourceBarrier(1, &d3d_resource_barrier);
	m_d3d_resource_command_list->Close();

	// コマンド実行
	ID3D12CommandList* d3d_command_lists[] =
	{
		m_d3d_resource_command_list.Get()
	};
	m_resource_queue.d3d_command_queue->ExecuteCommandLists(_countof(d3d_command_lists), d3d_command_lists);

	// コマンド終了まで待つ
	WaitIdle(m_resource_queue);

	// コマンドリストクリア
	m_d3d_resource_command_allocator->Reset();
	m_d3d_resource_command_list->Reset(m_d3d_resource_command_allocator.Get(), nullptr);

	return S_OK;
}

UINT Graphics::BitsPerPixel(DXGI_FORMAT fmt)
{
	switch (static_cast<int>(fmt))
	{
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
		return 128;

	case DXGI_FORMAT_R32G32B32_TYPELESS:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		return 96;

	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SINT:
	case DXGI_FORMAT_R32G32_TYPELESS:
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
	case DXGI_FORMAT_Y416:
	case DXGI_FORMAT_Y210:
	case DXGI_FORMAT_Y216:
		return 64;

	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
	case DXGI_FORMAT_R11G11B10_FLOAT:
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_R16G16_TYPELESS:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R16G16_SINT:
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
	case DXGI_FORMAT_B8G8R8A8_TYPELESS:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
	case DXGI_FORMAT_B8G8R8X8_TYPELESS:
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
	case DXGI_FORMAT_AYUV:
	case DXGI_FORMAT_Y410:
	case DXGI_FORMAT_YUY2:
		return 32;

	case DXGI_FORMAT_P010:
	case DXGI_FORMAT_P016:
		return 24;

	case DXGI_FORMAT_R8G8_TYPELESS:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R8G8_SINT:
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R16_SINT:
	case DXGI_FORMAT_B5G6R5_UNORM:
	case DXGI_FORMAT_B5G5R5A1_UNORM:
	case DXGI_FORMAT_A8P8:
	case DXGI_FORMAT_B4G4R4A4_UNORM:
		return 16;

	case DXGI_FORMAT_NV12:
	case DXGI_FORMAT_420_OPAQUE:
	case DXGI_FORMAT_NV11:
		return 12;

	case DXGI_FORMAT_R8_TYPELESS:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_SINT:
	case DXGI_FORMAT_A8_UNORM:
	case DXGI_FORMAT_AI44:
	case DXGI_FORMAT_IA44:
	case DXGI_FORMAT_P8:
		return 8;

	case DXGI_FORMAT_R1_UNORM:
		return 1;

	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		return 4;

	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
	case DXGI_FORMAT_BC6H_TYPELESS:
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
	case DXGI_FORMAT_BC7_TYPELESS:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		return 8;

	default:
		return 0;
	}
}
#pragma once

#include <wrl.h>
#include <memory>
#include <mutex>
#include <d3d11.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#include "TAKOEngine/Rendering/FrameBuffer.h"
#include "TAKOEngine/Rendering/RenderState.h"
#include "TAKOEngine/Rendering/Gizmos.h"
#include "TAKOEngine/Rendering/Shaders/ModelShader.h"
#include "TAKOEngine/Rendering/Shaders/SpriteShader.h"
#include "TAKOEngine/Rendering/DebugRenderer.h"
#include "TAKOEngine/Rendering/LineRenderer.h"
#include "TAKOEngine/Rendering/Descriptor.h"
#include "TAKOEngine/Rendering/ConstantBuffer.h"

enum class ModelShaderId
{
	Phong,
	Toon,
	Skydome,
	ShadowMap,

	EnumCount
};

enum class SpriteShaderId
{
	Default,
	UVScroll,
	Mask,
	ColorGrading,
	GaussianBlur,
	LuminanceExtraction,
	Finalpass,
	Deferred,

	EnumCount
};

enum class FrameBufferId
{
	Display,      //ポストエフェクト等
	Scene,        //シーン描画
	Luminance,
	GaussianBlur,
	
	//Deferred Rendering用
	Normal,       //法線
	Position,     //座標系

	EnumCount
};

// 蝙ら峩蜷梧悄髢馴囈險ｭ螳�
static const int SyncInterval = 0;

// 繝輔か繝ｼ繝槭ャ繝�
static const DXGI_FORMAT RenderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
static const DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D32_FLOAT;

// 繧ｰ繝ｩ繝輔ぅ繝�繧ｯ繧ｹ
class Graphics
{
public:
	Graphics() = default;
	~Graphics();

	// 繧､繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ
	static Graphics& Instance()
	{
		static Graphics instance;
		return instance;
	}

	bool isDX12Active = true;
	bool isDX11Active = false;

	// 蛻晄悄蛹�
	void Initalize(HWND hWnd, UINT buffer_count);
	// 逕ｻ髱｢陦ｨ遉ｺ
	void Present(UINT syncInterval);
	// 繝�繝舌う繧ｹ蜿門ｾ�
	ID3D11Device* GetDevice() { return device.Get(); };
	// 繝�繝舌う繧ｹ繧ｳ繝ｳ繝�繧ｭ繧ｹ繝亥叙蠕�
	ID3D11DeviceContext* GetDeviceContext() { return immediateContext.Get(); }
	// 繧ｹ繧ｯ繝ｪ繝ｼ繝ｳ蟷�蜿門ｾ�
	float GetScreenWidth() const { return screenWidth; }
	// 繧ｹ繧ｯ繝ｪ繝ｼ繝ｳ鬮倥＆蜿門ｾ�
	float GetScreenHeight() const { return screenHeight; }
	// 繝輔Ξ繝ｼ繝�繝舌ャ繝輔ぃ蜿門ｾ�
	FrameBuffer* GetFrameBuffer(FrameBufferId frameBufferId) { return frameBuffers[static_cast<int>(frameBufferId)].get(); }
	// 繝ｬ繝ｳ繝繝ｼ繧ｹ繝�繝ｼ繝亥叙蠕�
	RenderState* GetRenderState() { return renderState.get(); }
	// 繧ｮ繧ｺ繝｢蜿門ｾ�
	Gizmos* GetGizmos() { return gizmos.get(); }
	// 繝溘Η繝ｼ繝�繝�繧ｯ繧ｹ蜿門ｾ�
	std::mutex& GetMutex() { return mutex; }

	// 繝｢繝�繝ｫ繧ｷ繧ｧ繝ｼ繝繝ｼ蜿門ｾ�
	ModelShader* GetModelShader(ModelShaderId shaderId) { return modelShaders[static_cast<int>(shaderId)].get(); }
	// 繧ｹ繝励Λ繧､繝医す繧ｧ繝ｼ繝繝ｼ蜿門ｾ�
	SpriteShader* GetSpriteShader(SpriteShaderId shaderId) { return spriteShaders[static_cast<int>(shaderId)].get(); }
	// 繝�繝舌ャ繧ｰ繝ｬ繝ｳ繝繝ｩ蜿門ｾ�
	DebugRenderer* GetDebugRenderer() const { return debugRenderer.get(); }
	// 繝ｩ繧､繝ｳ繝ｬ繝ｳ繝繝ｩ蜿門ｾ�
	LineRenderer* GetLineRenderer() const { return lineRenderer.get(); }

	// 繝舌ャ繝輔ぃ謨ｰ蜿門ｾ�
	UINT GetBufferCount() const { return m_buffer_count; }

	// 繝舌ャ繝輔ぃ繧､繝ｳ繝�繝�繧ｯ繧ｹ蜿門ｾ�
	UINT GetCurrentBufferIndex() const { return m_dxgi_swap_chain->GetCurrentBackBufferIndex(); }

	// 繝�繝舌う繧ｹ蜿門ｾ�
	ID3D12Device* GetDeviceDX12() const { return m_d3d_device.Get(); }

	// 繝�繧｣繧ｹ繧ｯ繝ｪ繝励ち繝偵�ｼ繝怜叙蠕�
	DescriptorHeap* GetShaderResourceDescriptorHeap() const { return m_shader_resource_descriptor_heap.get(); }
	DescriptorHeap* GetSamplerDescriptorHeap() const { return m_sampler_descriptor_heap.get(); }

	DirectX::XMFLOAT3 GetScreenPosition(
		const DirectX::XMFLOAT3 worldPosition,
		const D3D11_VIEWPORT& viewport,
		const  DirectX::XMMATRIX& View,
		const  DirectX::XMMATRIX& Projection,
		const DirectX::XMMATRIX World
	);
	DirectX::XMFLOAT3 GetScreenPosition(const DirectX::XMFLOAT3 worldPosition);

	struct CommandQueue
	{
		Microsoft::WRL::ComPtr<ID3D12CommandQueue>			d3d_command_queue;
		Microsoft::WRL::ComPtr<ID3D12Fence>					d3d_fence;
		HANDLE												fence_event = NULL;
		UINT64												fence_value = 0;
	};

	void WaitIdle();

	// 謠冗判繧ｳ繝槭Φ繝牙ｮ溯｡悟ｮ御ｺ�縺ｾ縺ｧ蠕�縺､
	void WaitIdle(CommandQueue& command_queue);

	// 謠冗判螳溯｡�
	void Execute();

	// 謠冗判髢句ｧ�
	ID3D12GraphicsCommandList* Begin();

	// 謠冗判邨ゆｺ�
	void End();

	void FinishDX12();

	Descriptor* UpdateSceneConstantBuffer(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT3& light_direction);

	Shader* GetShader() const { return m_shader.get(); }

	// 繝�繧ｯ繧ｹ繝√Ε隱ｭ縺ｿ霎ｼ縺ｿ
	HRESULT LoadTexture(const char* filename, ID3D12Resource** d3d_resource);

	// 繝�繧ｯ繧ｹ繝√Ε菴懈��
	HRESULT CreateTexture(const BYTE* pixels, UINT width, UINT height, DXGI_FORMAT format, ID3D12Resource** d3d_resource);

	// 繝繝溘�ｼ繝�繧ｯ繧ｹ繝√Ε逕滓��
	HRESULT CreateDummyTexture(ID3D12Resource** d3d_resource);

	// 繝舌ャ繝輔ぃ繧ｳ繝斐�ｼ
	HRESULT CopyBuffer(ID3D12Resource* d3d_src_resource, ID3D12Resource* d3d_dst_resource);

private:
	// 繧､繝｡繝ｼ繧ｸ繧ｳ繝斐�ｼ
	HRESULT CopyImage(const BYTE* pixels, UINT width, UINT height, DXGI_FORMAT format, ID3D12Resource* resource);

	static UINT BitsPerPixel(DXGI_FORMAT fmt);

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;

	float screenWidth;
	float screenHeight;
	std::unique_ptr<FrameBuffer> frameBuffers[static_cast<int>(FrameBufferId::EnumCount)];
	std::unique_ptr<RenderState> renderState;
	std::unique_ptr<Gizmos> gizmos;

	std::unique_ptr<ModelShader> modelShaders[static_cast<int>(ModelShaderId::EnumCount)];
	std::unique_ptr<SpriteShader> spriteShaders[static_cast<int>(SpriteShaderId::EnumCount)];

	std::unique_ptr<DebugRenderer>					debugRenderer;
	std::unique_ptr<LineRenderer>					lineRenderer;

	std::mutex mutex;	// 繝溘Η繝ｼ繝�繝�繧ｯ繧ｹ

	static Graphics* s_instance;

	struct FrameResource
	{
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		d3d_command_allocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	d3d_command_list;
		Microsoft::WRL::ComPtr<ID3D12Resource>				d3d_rtv_resource;
		Microsoft::WRL::ComPtr<ID3D12Resource>				d3d_dsv_resource;
		Microsoft::WRL::ComPtr<ID3D12Resource>				d3d_cbv_resource;
		Descriptor* rtv_descriptor = nullptr;
		Descriptor* dsv_descriptor = nullptr;
		Descriptor* cbv_descriptor = nullptr;
		CbScene* cb_scene_data = nullptr;
	};

	Microsoft::WRL::ComPtr<ID3D12Device>				m_d3d_device;
	Microsoft::WRL::ComPtr<IDXGIFactory4>				m_dxgi_factory;
	Microsoft::WRL::ComPtr<IDXGISwapChain4>				m_dxgi_swap_chain;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		m_d3d_resource_command_allocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_d3d_resource_command_list;
	CommandQueue										m_resource_queue;
	CommandQueue										m_graphics_queue;
	std::vector<FrameResource>							m_frame_resources;
	std::unique_ptr<DescriptorHeap>						m_rtv_descriptor_heap;
	std::unique_ptr<DescriptorHeap>						m_dsv_descriptor_heap;
	std::unique_ptr<DescriptorHeap>						m_shader_resource_descriptor_heap;
	std::unique_ptr<DescriptorHeap>						m_sampler_descriptor_heap;

	std::unique_ptr<Shader>								m_shader;
	std::unique_ptr<SkinningPipeline>					m_skinning_pipeline;

	UINT	m_buffer_count = 0;

	float m_screen_width;
	float m_screen_height;
};

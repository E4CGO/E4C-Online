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

	EnumCount
};

enum class FrameBufferId
{
	Display,
	Scene,
	Luminance,
	GaussianBlur,

	EnumCount
};

// 垂直同期間隔設定
static const int SyncInterval = 0;

// フォーマット
static const DXGI_FORMAT RenderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
static const DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D32_FLOAT;

// グラフィックス
class Graphics
{
public:
	Graphics() = default;
	~Graphics();

	// インスタンス
	static Graphics& Instance()
	{
		static Graphics instance;
		return instance;
	}

	// 初期化
	void Initalize(HWND hWnd, UINT buffer_count);
	// 画面表示
	void Present(UINT syncInterval);
	// デバイス取得
	ID3D11Device* GetDevice() { return device.Get(); };
	// デバイスコンテキスト取得
	ID3D11DeviceContext* GetDeviceContext() { return immediateContext.Get(); }
	// スクリーン幅取得
	float GetScreenWidth() const { return screenWidth; }
	// スクリーン高さ取得
	float GetScreenHeight() const { return screenHeight; }
	// フレームバッファ取得
	FrameBuffer* GetFrameBuffer(FrameBufferId frameBufferId) { return frameBuffers[static_cast<int>(frameBufferId)].get(); }
	// レンダーステート取得
	RenderState* GetRenderState() { return renderState.get(); }
	// ギズモ取得
	Gizmos* GetGizmos() { return gizmos.get(); }
	// ミューテックス取得
	std::mutex& GetMutex() { return mutex; }

	// モデルシェーダー取得
	ModelShader* GetModelShader(ModelShaderId shaderId) { return modelShaders[static_cast<int>(shaderId)].get(); }
	// スプライトシェーダー取得
	SpriteShader* GetSpriteShader(SpriteShaderId shaderId) { return spriteShaders[static_cast<int>(shaderId)].get(); }
	// デバッグレンダラ取得
	DebugRenderer* GetDebugRenderer() const { return debugRenderer.get(); }
	// ラインレンダラ取得
	LineRenderer* GetLineRenderer() const { return lineRenderer.get(); }

	// バッファ数取得
	UINT GetBufferCount() const { return m_buffer_count; }

	// バッファインデックス取得
	UINT GetCurrentBufferIndex() const { return m_dxgi_swap_chain->GetCurrentBackBufferIndex(); }

	// デバイス取得
	ID3D12Device* GetDeviceDX12() const { return m_d3d_device.Get(); }

	// ディスクリプタヒープ取得
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

	// 描画コマンド実行完了まで待つ
	void WaitIdle(CommandQueue& command_queue);

	// 描画実行
	void Execute();

	// 描画開始
	ID3D12GraphicsCommandList* Begin();

	// 描画終了
	void End();

	Descriptor* UpdateSceneConstantBuffer(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT3& light_direction);

	Shader* GetShader() const { return m_shader.get(); }
	
	// テクスチャ読み込み
	HRESULT LoadTexture(const char* filename, ID3D12Resource** d3d_resource);

	// テクスチャ作成
	HRESULT CreateTexture(const BYTE* pixels, UINT width, UINT height, DXGI_FORMAT format, ID3D12Resource** d3d_resource);

	// ダミーテクスチャ生成
	HRESULT CreateDummyTexture(ID3D12Resource** d3d_resource);

	// バッファコピー
	HRESULT CopyBuffer(ID3D12Resource* d3d_src_resource, ID3D12Resource* d3d_dst_resource);

private:
	// イメージコピー
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

	std::mutex mutex;	// ミューテックス

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

	UINT	m_buffer_count = 0;

	float m_screen_width;
	float m_screen_height;
};

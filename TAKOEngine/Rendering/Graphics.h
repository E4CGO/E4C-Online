//! @file Grahics.h
//! @note

#ifndef __GRAHICS_GRAHICS_H__
#define __GRAHICS_GRAHICS_H__

#include <wrl.h>
#include <mutex>
#include <d2d1_3.h>
#include <dwrite.h>
#include <d3d11on12.h>
#include <dxgi1_6.h>
#include <d3d11.h>
#include <d3d12.h>

#include "TAKOEngine/Rendering/FrameBuffer.h"
#include "TAKOEngine/Rendering/FrameBufferManager.h"
#include "TAKOEngine/Rendering/RenderState.h"
#include "TAKOEngine/Rendering/Gizmos.h"
#include "TAKOEngine/Rendering/Shaders/ModelShader.h"
#include "TAKOEngine/Rendering/Shaders/ModelShaderDX12.h"
#include "TAKOEngine/Rendering/Shaders/SpriteShader.h"
#include "TAKOEngine/Rendering/Shaders/SpriteShaderDX12.h"
#include "TAKOEngine/Rendering/DebugRenderer/DebugRenderer.h"
#include "TAKOEngine/Rendering/LineRenderer.h"
#include "TAKOEngine/Rendering/Descriptor.h"
#include "TAKOEngine/Rendering/ConstantBuffer.h"
#include "TAKOEngine/Tool/ImGuiRenderer.h"
#include "TAKOEngine/Editor/Camera/CameraManager.h"
#include "TAKOEngine/Rendering/ParticleRenderer.h"
#include "TAKOEngine/Rendering/ShadowMapRender.h"

#define MAX_BUFFER_COUNT (2)

enum class ModelShaderId
{
	Phong,
	Toon,
	Skydome,
	ShadowMap,
	Plane,
	Portal,
	PortalSquare,
	Billboard,
	Fireball,
	Lambert,

	EnumCount
};

enum class ModelShaderDX12Id
{
	Lambert,
	LambertInstancing,
	Phong,
	PhongInstancing,
	Toon,
	ToonInstancing,
	Skydome,
	shadowMap,

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

enum class SpriteShaderDX12Id
{
	Default,
	LuminanceExtraction,
	GaussianBlur,
	ColorGrading,
	Finalpass,
	Particle,

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

enum class FrameBufferDX12Id
{
	Scene,        //シーン描画
	Luminance,
	GaussianBlur,

	//Deferred Rendering用
	Normal,       //法線
	Position,     //座標系

	EnumCount
};

// 垂直同期間隔設定
static const int SyncInterval = 0;

// フォーマット
static const DXGI_FORMAT RenderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
static const DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D32_FLOAT;

//*******************************************************
// @class Graphics
// @brief グラフィックスエンジン
// @par
//*******************************************************
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

	bool isDX12Active = false;
	bool isDX11Active = false;

	struct CommandQueue
	{
		Microsoft::WRL::ComPtr<ID3D12CommandQueue>			d3d_command_queue;
		Microsoft::WRL::ComPtr<ID3D12Fence>					d3d_fence;
		HANDLE												fence_event = NULL;
		UINT64												fence_value = 0;
	};

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
	// DX12のフレームバッファ取得
	FrameBufferDX12* GetFramBufferDX12(FrameBufferDX12Id frameBufferId) { return dx12_frameBuffers[static_cast<int>(frameBufferId)].get(); }
	// DX12のフレームバッファマネージャー
	FrameBufferManager* GetFrameBufferManager() { return m_framebufferManager.get(); }

	IDWriteFactory* GetDWriteFactory() { return m_dWriteFactory.Get(); }
	ID3D11On12Device* GetD3D11On12Device() { return m_d3d11On12Device.Get(); }
	ID2D1DeviceContext2* GetD2D1DeviceContext() { return m_d2dDeviceContext.Get(); }
	ID3D11DeviceContext* GetD3D112DDeviceContext() { return m_d3d11DeviceContext.Get(); }
	Microsoft::WRL::ComPtr<ID3D11Resource> GetD3D11BackBuffer(int i) { return m_wrappedBackBuffers[i]; }
	ID2D1Bitmap1* GetD2D1RenderTargets(int i) { return m_d2dRenderTargets[i].Get(); }

	// シャドウマップ取得
	ShadowMapRenderDX12* GetShadowRenderer() { return m_shadowMapRenderer.get(); }

	// レンダーステート取得
	RenderState* GetRenderState() { return renderState.get(); }
	//DX12のレンダーステート
	RenderStateDX12* GetRenderStateDX12() { return m_renderStateDX12.get(); }
	// ギズモ取得
	Gizmos* GetGizmos() { return gizmos.get(); }
	// ミューテックス取得
	std::mutex& GetMutex() { return mutex; }
	// モデルシェーダー取得
	ModelShader* GetModelShader(ModelShaderId shaderId) { return modelShaders[static_cast<int>(shaderId)].get(); }
	//DirextX12のモデルシェーダー取得
	ModelShaderDX12* GetModelShaderDX12(ModelShaderDX12Id shaderId) { return dx12_modelshaders[static_cast<int>(shaderId)].get(); }
	// スプライトシェーダー取得
	SpriteShader* GetSpriteShader(SpriteShaderId shaderId) { return spriteShaders[static_cast<int>(shaderId)].get(); }
	// DX12のスプライトシェーダー取得
	SpriteShaderDX12* GetSpriteShaderDX12(SpriteShaderDX12Id shaderId) { return dx12_spriteShaders[static_cast<int>(shaderId)].get(); }
	// デバッグレンダラ取得
	DebugRenderer* GetDebugRenderer() const { return debugRenderer.get(); }
	// ラインレンダラ取得
	LineRenderer* GetLineRenderer() const { return lineRenderer.get(); }

	//スキニング取得
	SkinningPipeline* GetSkinningPipeline() const { return m_skinning_pipeline.get(); }

	// パーティクル取得
	ParticleCompute* GetParticleCompute() const { return m_compute.get(); }

	// ImGUIンレンダラ取得
	ImGuiRenderer* GetImGUIRenderer() const { return m_imgui_renderer.get(); }

	// バッファ数取得
	UINT GetBufferCount() const { return m_buffer_count; }

	// バッファインデックス取得
	UINT GetCurrentBufferIndex() const { return m_dxgi_swap_chain->GetCurrentBackBufferIndex(); }

	// デバイス取得
	ID3D12Device* GetDeviceDX12() const { return m_d3d_device.Get(); }
	CommandQueue GetCommandQueue() const { return m_graphics_queue; }

	// ディスクリプタヒープ取得
	DescriptorHeap* GetShaderResourceDescriptorHeap() const { return m_shader_resource_descriptor_heap.get(); }
	DescriptorHeap* GetSamplerDescriptorHeap() const { return m_sampler_descriptor_heap.get(); }
	DescriptorHeap* GetRenderTargetDescriptorHeap() const { return m_rtv_descriptor_heap.get(); }
	DescriptorHeap* GetDepthStencilDescriptorHeap() const { return m_dsv_descriptor_heap.get(); }

	DirectX::XMFLOAT3 GetScreenPosition(
		const DirectX::XMFLOAT3 worldPosition,
		const D3D11_VIEWPORT& viewport,
		const  DirectX::XMMATRIX& View,
		const  DirectX::XMMATRIX& Projection,
		const DirectX::XMMATRIX World);
	DirectX::XMFLOAT3 GetScreenPosition(
		const DirectX::XMFLOAT3 worldPosition,
		const D3D12_VIEWPORT& viewport,
		const  DirectX::XMMATRIX& View,
		const  DirectX::XMMATRIX& Projection,
		const DirectX::XMMATRIX World);
	DirectX::XMFLOAT3 GetScreenPosition(const DirectX::XMFLOAT3 worldPosition);

	void WaitIdle();

	// 描画コマンド実行完了まで待つ
	void WaitIdle(CommandQueue& command_queue);

	// 描画実行
	void Execute();

	// 描画開始
	void BeginRender();

	// 描画終了
	void End();

	void FinishDX12();

	const Descriptor* UpdateSceneConstantBuffer(const Camera* camera);

	// テクスチャ読み込み
	HRESULT LoadTexture(const char* filename, ID3D12Resource** d3d_resource);
	HRESULT LoadCubeTexture(const std::wstring& filename, ID3D12Resource** d3d_resource);

	// テクスチャ作成
	HRESULT CreateTexture(const BYTE* pixels, UINT width, UINT height, DXGI_FORMAT format, ID3D12Resource** d3d_resource);

	// ダミーテクスチャ生成
	HRESULT CreateDummyTexture(ID3D12Resource** d3d_resource);

	// バッファコピー
	HRESULT CopyBuffer(ID3D12Resource* d3d_src_resource, ID3D12Resource* d3d_dst_resource);

	//Sampler取得
	SamplerManager* GetSampler(SamplerState state) { return m_sampler[static_cast<int>(state)].get(); }

	// 現在のフレームバッファのレンダリングターゲットビューを取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentFrameBuffuerRTV() const
	{
		return rtv_descriptor[frame_buffer_index]->GetCpuHandle();
	}

	// フレームバッファへの描画時に使用されているデプスステンシルビューを取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentFrameBuffuerDSV() const
	{
		return dsv_descriptor->GetCpuHandle();
	}

	// ビューポートを取得
	D3D12_VIEWPORT GetViewPort()
	{
		return m_viewport;
	}

	void SetDX12Render(bool isRender) { isDX12Active = isRender; }
	void SetDX11Render(bool isRender) { isDX11Active = isRender; }

private:
	// イメージコピー
	HRESULT CopyImage(const BYTE* pixels, UINT width, UINT height, DXGI_FORMAT format, ID3D12Resource* resource);

	// キューブマップ用イメージコピー
	HRESULT CopyImageForCubeMap(const D3D12_SUBRESOURCE_DATA* subresources, UINT width, UINT height, DXGI_FORMAT format, ID3D12Resource* d3d_resource);

	static UINT BitsPerPixel(DXGI_FORMAT fmt);

	// フレームバッファ用のレンダリングターゲットビューを作成
	void CreateRTVForFameBuffer();

	// フレームバッファ用の深度ステンシルビューを作成
	void CreateDSVForFrameBuffer(UINT frameBufferWidth, UINT frameBufferHeight);

	// フレームバッファ用のコンスタントバッファを作成
	void CreateConstantBuffer();

	// コマンドリストとコマンドアロケーターの作成
	void CreateCommand();

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;

	float screenWidth;
	float screenHeight;
	std::unique_ptr<FrameBuffer> frameBuffers[static_cast<int>(FrameBufferId::EnumCount)];
	std::unique_ptr<FrameBufferDX12> dx12_frameBuffers[static_cast<int>(FrameBufferDX12Id::EnumCount)];
	std::unique_ptr<FrameBufferManager> m_framebufferManager;
	std::unique_ptr<RenderState> renderState;
	std::unique_ptr<RenderStateDX12> m_renderStateDX12;
	std::unique_ptr<Gizmos> gizmos;

	std::unique_ptr<ModelShader> modelShaders[static_cast<int>(ModelShaderId::EnumCount)];
	std::unique_ptr<ModelShaderDX12> dx12_modelshaders[static_cast<int>(ModelShaderDX12Id::EnumCount)];
	std::unique_ptr<SpriteShader> spriteShaders[static_cast<int>(SpriteShaderId::EnumCount)];
	std::unique_ptr<SpriteShaderDX12> dx12_spriteShaders[static_cast<int>(SpriteShaderDX12Id::EnumCount)];

	std::unique_ptr<DebugRenderer>					debugRenderer;
	std::unique_ptr<LineRenderer>					lineRenderer;
	//スキニング
	std::unique_ptr<SkinningPipeline>	m_skinning_pipeline;

	// パーティクル
	std::unique_ptr<ParticleCompute> m_compute;

	std::unique_ptr<ShadowMapRenderDX12> m_shadowMapRenderer;

	std::mutex mutex;	// ミューテックス

	static Graphics* s_instance;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		d3d_command_allocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	d3d_command_list;
	Microsoft::WRL::ComPtr<ID3D12Resource>				d3d_rtv_resource[MAX_BUFFER_COUNT];
	Microsoft::WRL::ComPtr<ID3D12Resource>				d3d_dsv_resource;
	Microsoft::WRL::ComPtr<ID3D12Resource>				d3d_cbv_resource;
	const Descriptor* rtv_descriptor[MAX_BUFFER_COUNT] = { nullptr, nullptr };
	const Descriptor* dsv_descriptor = nullptr;
	const Descriptor* cbv_descriptor = nullptr;
	CbScene* cb_scene_data = nullptr;

	D3D12_VIEWPORT m_viewport;	//ビューポート
	UINT frame_buffer_index = 0;

	Microsoft::WRL::ComPtr<ID3D12Device>				m_d3d_device;
	Microsoft::WRL::ComPtr<IDXGIFactory4>				m_dxgi_factory;
	Microsoft::WRL::ComPtr<IDXGISwapChain4>				m_dxgi_swap_chain;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		m_d3d_resource_command_allocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_d3d_resource_command_list;
	CommandQueue										m_resource_queue;
	CommandQueue										m_graphics_queue;

	std::unique_ptr<DescriptorHeap>						m_rtv_descriptor_heap;
	std::unique_ptr<DescriptorHeap>						m_dsv_descriptor_heap;
	std::shared_ptr<DescriptorHeap>						m_shader_resource_descriptor_heap;
	std::unique_ptr<DescriptorHeap>						m_sampler_descriptor_heap;

	std::unique_ptr<SamplerManager> m_sampler[static_cast<int>(SamplerState::EnumCount)];

	UINT	m_buffer_count = 0;

	float m_screen_width;
	float m_screen_height;

	std::unique_ptr<ImGuiRenderer>						m_imgui_renderer;

	// DWrite ファクトリー
	Microsoft::WRL::ComPtr<IDWriteFactory> m_dWriteFactory;
	// D2D ファクトリー
	Microsoft::WRL::ComPtr<ID2D1Factory3> m_d2dFactory;
	// D2D デバイス
	Microsoft::WRL::ComPtr<ID2D1Device2> m_d2dDevice;
	// D2Dのために D11デバイス
	Microsoft::WRL::ComPtr<ID3D11On12Device> m_d3d11On12Device;
	// D2Dのデバイスコンテクスト
	Microsoft::WRL::ComPtr<ID2D1DeviceContext2> m_d2dDeviceContext;
	// D2DのためにD11デバイスコンテクスト
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3d11DeviceContext;
	// 画像バファー
	Microsoft::WRL::ComPtr<ID3D11Resource> m_wrappedBackBuffers[MAX_BUFFER_COUNT];
	// 画面バファー
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_d2dRenderTargets[MAX_BUFFER_COUNT];
	// 画像バファ設定
	D2D1_BITMAP_PROPERTIES1 bitmapProperties;
};

#endif // !__GRAHICS_GRAHICS_H__
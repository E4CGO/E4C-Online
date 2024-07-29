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
	Display,      //�|�X�g�G�t�F�N�g��
	Scene,        //�V�[���`��
	Luminance,
	GaussianBlur,
	
	//Deferred Rendering�p
	Normal,       //�@��
	Position,     //���W�n

	EnumCount
};

// ���������Ԋu�ݒ�
static const int SyncInterval = 0;

// �t�H�[�}�b�g
static const DXGI_FORMAT RenderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
static const DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D32_FLOAT;

// �O���t�B�b�N�X
class Graphics
{
public:
	Graphics() = default;
	~Graphics();

	// �C���X�^���X
	static Graphics& Instance()
	{
		static Graphics instance;
		return instance;
	}

	// ������
	void Initalize(HWND hWnd, UINT buffer_count);
	// ��ʕ\��
	void Present(UINT syncInterval);
	// �f�o�C�X�擾
	ID3D11Device* GetDevice() { return device.Get(); };
	// �f�o�C�X�R���e�L�X�g�擾
	ID3D11DeviceContext* GetDeviceContext() { return immediateContext.Get(); }
	// �X�N���[�����擾
	float GetScreenWidth() const { return screenWidth; }
	// �X�N���[�������擾
	float GetScreenHeight() const { return screenHeight; }
	// �t���[���o�b�t�@�擾
	FrameBuffer* GetFrameBuffer(FrameBufferId frameBufferId) { return frameBuffers[static_cast<int>(frameBufferId)].get(); }
	// �����_�[�X�e�[�g�擾
	RenderState* GetRenderState() { return renderState.get(); }
	// �M�Y���擾
	Gizmos* GetGizmos() { return gizmos.get(); }
	// �~���[�e�b�N�X�擾
	std::mutex& GetMutex() { return mutex; }

	// ���f���V�F�[�_�[�擾
	ModelShader* GetModelShader(ModelShaderId shaderId) { return modelShaders[static_cast<int>(shaderId)].get(); }
	// �X�v���C�g�V�F�[�_�[�擾
	SpriteShader* GetSpriteShader(SpriteShaderId shaderId) { return spriteShaders[static_cast<int>(shaderId)].get(); }
	// �f�o�b�O�����_���擾
	DebugRenderer* GetDebugRenderer() const { return debugRenderer.get(); }
	// ���C�������_���擾
	LineRenderer* GetLineRenderer() const { return lineRenderer.get(); }

	// �o�b�t�@���擾
	UINT GetBufferCount() const { return m_buffer_count; }

	// �o�b�t�@�C���f�b�N�X�擾
	UINT GetCurrentBufferIndex() const { return m_dxgi_swap_chain->GetCurrentBackBufferIndex(); }

	// �f�o�C�X�擾
	ID3D12Device* GetDeviceDX12() const { return m_d3d_device.Get(); }

	// �f�B�X�N���v�^�q�[�v�擾
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

	// �`��R�}���h���s�����܂ő҂�
	void WaitIdle(CommandQueue& command_queue);

	// �`����s
	void Execute();

	// �`��J�n
	ID3D12GraphicsCommandList* Begin();

	// �`��I��
	void End();

	Descriptor* UpdateSceneConstantBuffer(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT3& light_direction);

	Shader* GetShader() const { return m_shader.get(); }
	
	// �X�v���C�g�V�F�[�_�[�擾
	SpriteShader* GetShader(SpriteShaderId id) const { return spriteShaders[static_cast<int>(id)].get(); }

	// �e�N�X�`���ǂݍ���
	HRESULT LoadTexture(const char* filename, ID3D12Resource** d3d_resource);

	// �e�N�X�`���쐬
	HRESULT CreateTexture(const BYTE* pixels, UINT width, UINT height, DXGI_FORMAT format, ID3D12Resource** d3d_resource);

	// �_�~�[�e�N�X�`������
	HRESULT CreateDummyTexture(ID3D12Resource** d3d_resource);

	// �o�b�t�@�R�s�[
	HRESULT CopyBuffer(ID3D12Resource* d3d_src_resource, ID3D12Resource* d3d_dst_resource);

private:
	// �C���[�W�R�s�[
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

	std::mutex mutex;	// �~���[�e�b�N�X

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

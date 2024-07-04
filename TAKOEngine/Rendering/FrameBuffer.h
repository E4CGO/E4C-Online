#pragma once

#include <wrl.h>
#include <d3d11.h>

class FrameBuffer
{
public:
	FrameBuffer(ID3D11Device* device, IDXGISwapChain* swapchain, DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT);
	FrameBuffer(ID3D11Device* device, UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

	// �J���[�}�b�v�擾
	ID3D11ShaderResourceView* GetColorMap() const { return colorMap.Get(); }
	const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetShaderResourceView() const { return colorMap; }

	// �N���A
	void Clear(ID3D11DeviceContext* dc, float r, float g, float b, float a);
	void Clears(ID3D11DeviceContext* dc, int rtvNum, ID3D11RenderTargetView* rtv[], float r, float g, float b, float a);

	//�����_�[�^�[�Q�b�g�ݒ�	
	void SetRenderTarget(ID3D11DeviceContext* dc);
	void SetRenderTargets(ID3D11DeviceContext* dc, int rtvNum, ID3D11RenderTargetView* rtv[]);

	//�r���[�|�[�g�ݒ�
	void SetViewport(UINT width, UINT height);

	// �����_�[�^�[�Q�b�g�r���[�擾
	ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }

	// �f�v�X�X�e���V���r���[�擾
	ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }

	//�r���[�|�[�g�擾
	const D3D11_VIEWPORT& GetViewport() { return viewport; }

private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	D3D11_VIEWPORT viewport;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> colorMap;
	D3D11_TEXTURE2D_DESC texture2dDesc;
};

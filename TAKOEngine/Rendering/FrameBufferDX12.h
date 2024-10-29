//! @file FrameBufferDX12.h
//! @note

#ifndef __GRAHICS_FRAME_BUFFERDX12_H__
#define __GRAHICS_FRAME_BUFFERDX12_H__

#include <dxgi1_6.h>
#include "Descriptor.h"
#include "FrameBufferTexture.h"

//*******************************************************
// @class FrameBufferDX12
// @brief �����_�[�^�Q�b�g�쐬
// @par   
//*******************************************************
class FrameBufferDX12
{
public:
	FrameBufferDX12(ID3D12Device* device, const wchar_t* resourceName[2], UINT wight, UINT height, DXGI_FORMAT rtv_format = DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT dsv_format = DXGI_FORMAT_D32_FLOAT);
	~FrameBufferDX12();

	// �f�v�X�X�e���V���o�b�t�@�����݂��Ă��邩����
	bool IsExsitDepthStencilBuffer() const
	{
		return d3d_dsv_resource;
	}

	// �����_�[�^�[�Q�b�g�̃N���A�J���[
	const float* GetRTVClearColor() const
	{
		return m_rtvClearColor;
	}
	
	// �[�x�X�e���V���̃N���A�J���[
	float GetDSVClearValue() const
	{
		return m_dsvClearValue;
	}

	// �����_�[�^�[�Q�b�g�ƂȂ�e�N�X�`�����擾
	FrameBufferTexture& GetRenderTargetTexture()
	{
		return m_renderTargetTexture;
	}

	// �����_�[�^�[�Q�b�g�̃t�H�[�}�b�g���擾
	DXGI_FORMAT GetRTVBufferFormat() const { return m_renderTargetTexture.GetFormat(); }

	//�e�N�X�`�����擾
	inline int GetWidth() const { return m_width; }

	//�e�N�X�`���̍����擾
	inline int GetHeight() const { return m_height; }

	// �����_�[�^�[�Q�b�g�̃f�B�X�N���v�^�擾
	const Descriptor* GetRTVDescriptor() { return rtv_descriptor; }

	// �[�x�X�e���V���o�b�t�@�̃f�B�X�N���v�^�擾
	const Descriptor* GetDSVDescriptor() { return dsv_descriptor; }

private:
	// �����_�[�^�[�Q�b�g�ƂȂ�e�N�X�`�����쐬����
	void CreateRenderTargetTexture(ID3D12Device* device, const wchar_t* resourceName, UINT wight, UINT height, DXGI_FORMAT format);

	// �[�x�X�e���V���o�b�t�@�ƂȂ�e�N�X�`�����쐬
	void CreateDepthStencilTexture(ID3D12Device* device, const wchar_t* resourceName, UINT wight, UINT height, DXGI_FORMAT format);

	// �f�B�X�N���v�^�q�[�v���擾
	void CreateDescriptorHeap();

	// �f�B�X�N���v�^���쐬����
	void CreateDescriptor(ID3D12Device* device);

private:
	FrameBufferTexture m_renderTargetTexture;
	FrameBufferTexture m_depthStencilTexture;
	Microsoft::WRL::ComPtr<ID3D12Resource> d3d_rtv_resource;
	Microsoft::WRL::ComPtr<ID3D12Resource> d3d_dsv_resource;
	const Descriptor* rtv_descriptor = nullptr;
	const Descriptor* dsv_descriptor = nullptr;

	int m_width  = 0;	//�����_�����O�^�[�Q�b�g�̕�
	int m_height = 0;	//�����_�����O�^�[�Q�b�g�̍���
	float m_rtvClearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };	//�����_�����O�^�[�Q�b�g�r���[�̃N���A�J���[
	float m_dsvClearValue = 1.0f;							//DSV�̃N���A�J���[
};

#endif // !__GRAHICS_FRAME_BUFFERDX12_H__
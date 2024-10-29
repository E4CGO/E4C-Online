//! @file FrameBufferManager.h
//! @note 

#ifndef __GRAHICS_FRAME_BUFFER_MANAGER_H__
#define __GRAHICS_FRAME_BUFFER_MANAGER_H__

#include "FrameBufferDX12.h"

//*******************************************************
// @class FrameBufferManager
// @brief FrameBuffer�Ǘ��N���X
// @par   
//*******************************************************
class FrameBufferManager
{
public:
	FrameBufferManager() {};

	// ������
	void Init(ID3D12GraphicsCommandList* commandList);

	// �R�}���h���X�g�����
	void Close();

	// �R�}���h���X�g�����Z�b�g
	void Reset(ID3D12CommandAllocator* commandAllocator, ID3D12PipelineState* pipelineState); 

	// �����_�[�^�[�Q�b�g�ݒ�
	void SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle);
	void SetRenderTarget(FrameBufferDX12* renderTarget);

	// �������̃����_�[�^�[�Q�b�g�ݒ�
	void SetRenderTargets(UINT numRTV, FrameBufferDX12* renderTarget[]);

	//�����_�[�^�[�Q�b�g�ƃf�v�X�X�e���V���r���[���N���A
	void Clear(FrameBufferDX12* renderTarget);

	// �������̃����_�[�^�[�Q�b�g�ƃf�v�X�X�e���V���r���[���N���A
	void Clears(UINT numRTV, FrameBufferDX12* renderTarget[]);

	// �����_�[�^�[�Q�b�g�Ƃ��Ďg�p�\�ɂȂ�܂ő҂�
	void WaitUntilToPossibleSetRenderTarget(FrameBufferDX12* renderTarget);

	// �������̃����_�[�^�[�Q�b�g���g�p�\�ɂȂ�܂ő҂�
	void WaitUntilToPossibleSetRenderTargets(UINT numRTV, FrameBufferDX12* renderTarget[]);

	// �����_�[�^�[�Q�b�g�ւ̕`�����ݑ҂�
	void WaitUntilFinishDrawingToRenderTarget(FrameBufferDX12* renderTarget);

	// �������̃����_�[�^�[�Q�b�g�̕`�����ݑ҂�
	void WaitUntilFinishDrawingToRenderTargets(UINT numRTV, FrameBufferDX12* renderTarget[]);

	// �r���[�|�[�g�ƃV�U�����O��`���Z�b�g�Őݒ�
	void SetViewportAndScissor(D3D12_VIEWPORT& viewport);

	// �f�v�X�X�e���V���r���[���N���A
	void ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, float clearValue);

	// �����_�[�^�[�Q�b�g�̃N���A
	void ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, const float* clearColor);

	// �����_�[�^�[�Q�b�g�Ƃ��Ďg�p�\�ɂȂ�܂ő҂�
	void WaitUntilToPossibleSetRenderTarget(ID3D12Resource* renderTarget);

	// �����_�[�^�[�Q�b�g�ւ̕`�����ݑ҂�
	void WaitUntilFinishDrawingToRenderTarget(ID3D12Resource* renderTarget);

	// �R�}���h���X�g�擾
	ID3D12GraphicsCommandList* GetCommandList() { return m_commandList.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
};

#endif // !__GRAHICS_FRAME_BUFFER_MANAGER_H__
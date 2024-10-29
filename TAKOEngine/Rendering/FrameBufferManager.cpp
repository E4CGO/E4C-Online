//! @file FrameBufferManager.h
//! @note 

#include "FrameBufferManager.h"

//******************************************************************
// @class     ������
// @param[in] commandList�@�R�}���h���X�g
// @return    �Ȃ�
//****************************************************************** 
void FrameBufferManager::Init(ID3D12GraphicsCommandList* commandList)
{
	m_commandList = commandList;
}

//******************************************************************
// @class     �R�}���h���X�g�����
// @param[in] �Ȃ�
// @return    �Ȃ�
//****************************************************************** 
void FrameBufferManager::Close()
{
	m_commandList->Close();
}

//******************************************************************
// @class      �R�}���h���X�g�����Z�b�g
// @param[in] commandAllocator ID3D12CommandAllocator*
// @param[in] pipelineState    ID3D12PipelineState*
// @return    �Ȃ�
//****************************************************************** 
void FrameBufferManager::Reset(ID3D12CommandAllocator* commandAllocator, ID3D12PipelineState* pipelineState)
{
	m_commandList->Reset(commandAllocator, pipelineState);
}

//******************************************************************
// @class     �����_�[�^�[�Q�b�g�ݒ�
// @param[in] renderTarget      �����_�[�^�[�Q�b�g
// @return    �Ȃ�
//****************************************************************** 
void FrameBufferManager::SetRenderTarget(FrameBufferDX12* renderTarget)
{
	// �r���[�|�[�g�̐ݒ�
	D3D12_VIEWPORT d3d_viewport;
	d3d_viewport.TopLeftX = 0;
	d3d_viewport.TopLeftY = 0;
	d3d_viewport.Width    = static_cast<float>(renderTarget->GetWidth());
	d3d_viewport.Height   = static_cast<float>(renderTarget->GetHeight());
	d3d_viewport.MinDepth = 0.0f;
	d3d_viewport.MaxDepth = 1.0f;
	SetViewportAndScissor(d3d_viewport);

	FrameBufferDX12* rtArray[] = { renderTarget };
	SetRenderTargets(1, rtArray);
}

//******************************************************************
// @class     �����_�[�^�[�Q�b�g�ݒ�
// @param[in] rtvHandle      �����_�[�^�[�Q�b�g�n���h��
// @param[in] dsvHandle      �f�v�X�X�e���V���n���h��
// @return    �Ȃ�
//****************************************************************** 
void FrameBufferManager::SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle)
{
	m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
}

//******************************************************************
// @class     �������̃����_�[�^�[�Q�b�g�ݒ�
// @param[in] numRTV              �����_�[�^�[�Q�b�g��
// @param[in] renderTarget[]      �����_�[�^�[�Q�b�g�̔z��
// @return    �Ȃ�
//******************************************************************
void FrameBufferManager::SetRenderTargets(UINT numRTV, FrameBufferDX12* renderTarget[])
{
	// �r���[�|�[�g�̐ݒ�
	D3D12_VIEWPORT d3d_viewport;
	d3d_viewport.TopLeftX = 0;
	d3d_viewport.TopLeftY = 0;
	d3d_viewport.Width   = static_cast<float>(renderTarget[0]->GetWidth());
	d3d_viewport.Height  = static_cast<float>(renderTarget[0]->GetHeight());
	d3d_viewport.MinDepth = 0.0f;
	d3d_viewport.MaxDepth = 1.0f;
	SetViewportAndScissor(d3d_viewport);

	D3D12_CPU_DESCRIPTOR_HANDLE rtDSHandleTbl[32]{};
	for (UINT rtNo = 0; rtNo < numRTV; rtNo++)
	{
		rtDSHandleTbl[rtNo] = renderTarget[rtNo]->GetRTVDescriptor()->GetCpuHandle();
	}

	if (renderTarget[0]->IsExsitDepthStencilBuffer())
	{
		// �[�x�o�b�t�@������
		D3D12_CPU_DESCRIPTOR_HANDLE dsDS = renderTarget[0]->GetDSVDescriptor()->GetCpuHandle();
		m_commandList->OMSetRenderTargets(numRTV, rtDSHandleTbl, FALSE, &dsDS);
	}
	else
	{
		// �[�x�o�b�t�@���Ȃ�
		m_commandList->OMSetRenderTargets(numRTV, rtDSHandleTbl, FALSE, nullptr);
	}
}

//******************************************************************
// @class      �����_�[�^�[�Q�b�g�ƃf�v�X�X�e���V���r���[���N���A
// @param[in]  renderTarget      �����_�[�^�[�Q�b�g
// @return     �Ȃ�
//******************************************************************
void FrameBufferManager::Clear(FrameBufferDX12* renderTarget)
{
	FrameBufferDX12* rtArray[] = { renderTarget };
	Clears(1, rtArray);
}

//******************************************************************
// @class     �������̃����_�[�^�[�Q�b�g�ƃf�v�X�X�e���V���r���[���N���A
// @param[in] numRTV              �����_�[�^�[�Q�b�g��
// @param[in] renderTarget[]      �����_�[�^�[�Q�b�g�̔z��
// @return    �Ȃ�
//******************************************************************
void FrameBufferManager::Clears(UINT numRTV, FrameBufferDX12* renderTarget[])
{
	if (renderTarget[0]->IsExsitDepthStencilBuffer())
	{
		ClearDepthStencilView(renderTarget[0]->GetDSVDescriptor()->GetCpuHandle(), renderTarget[0]->GetDSVClearValue());
	}

	for (UINT i = 0; i < numRTV; i++)
	{
		ClearRenderTargetView(renderTarget[i]->GetRTVDescriptor()->GetCpuHandle(), renderTarget[i]->GetRTVClearColor());
	}
}

//******************************************************************
// @class     �����_�[�^�[�Q�b�g�Ƃ��Ďg�p�\�ɂȂ�܂ő҂�
// @param[in] renderTarget  �����_�[�^�[�Q�b�g
// @return    �Ȃ�
//******************************************************************
void FrameBufferManager::WaitUntilToPossibleSetRenderTarget(FrameBufferDX12* renderTarget)
{
	WaitUntilToPossibleSetRenderTarget(renderTarget->GetRenderTargetTexture().Get());
}

//******************************************************************
// @class     �������̃����_�[�^�[�Q�b�g���g�p�\�ɂȂ�܂ő҂�
// @param[in] numRTV              �����_�[�^�[�Q�b�g��
// @param[in] renderTarget[]      �����_�[�^�[�Q�b�g�̔z��
// @return    �Ȃ�
//******************************************************************
void FrameBufferManager::WaitUntilToPossibleSetRenderTargets(UINT numRTV, FrameBufferDX12* renderTarget[])
{
	for (UINT i = 0; i < numRTV; i++)
	{
		WaitUntilToPossibleSetRenderTarget(renderTarget[i]);
	}
}

//******************************************************************
// @class     �����_�[�^�[�Q�b�g�ւ̕`�����ݑ҂�
// @param[in] renderTarget  �����_�[�^�[�Q�b�g
// @return    �Ȃ�
//******************************************************************
void FrameBufferManager::WaitUntilFinishDrawingToRenderTarget(FrameBufferDX12* renderTarget)
{
	WaitUntilFinishDrawingToRenderTarget(renderTarget->GetRenderTargetTexture().Get());
}

//******************************************************************
// @class     �������̃����_�[�^�[�Q�b�g�̕`�����ݑ҂�
// @param[in] numRTV              �����_�[�^�[�Q�b�g��
// @param[in] renderTarget[]      �����_�[�^�[�Q�b�g�̔z��
// @return    �Ȃ�
//******************************************************************
void FrameBufferManager::WaitUntilFinishDrawingToRenderTargets(UINT numRTV, FrameBufferDX12* renderTarget[])
{
	for (UINT i = 0; i < numRTV; i++)
	{
		WaitUntilFinishDrawingToRenderTarget(renderTarget[i]);
	}
}

//******************************************************************
// @class      �f�v�X�X�e���V���r���[���N���A
// @param[in]  dsvHandle  �f�v�X�X�e���V���r���[��CPU�n���h��
// @param[in]  clearValue �f�v�X�X�e���V���r���[�̃N���A�J���[
// @return     �Ȃ�
//******************************************************************
void FrameBufferManager::ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, float clearValue)
{
	m_commandList->ClearDepthStencilView(
		dsvHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		clearValue,
		0,
		0,
		nullptr);
}

//******************************************************************
// @class      �����_�[�^�[�Q�b�g���N���A
// @param[in]  dsvHandle  �����_�[�^�[�Q�b�g��CPU�n���h��
// @param[in]  clearValue �����_�[�^�[�Q�b�g�̃N���A�J���[
// @return     �Ȃ�
//******************************************************************
void FrameBufferManager::ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, const float* clearColor)
{
	m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
}

//******************************************************************
// @class     �r���[�|�[�g�ƃV�U�����O��`���Z�b�g�Őݒ�
// @param[in] viewport  �r���[�|�[�g
// @return    �Ȃ�
//******************************************************************
void FrameBufferManager::SetViewportAndScissor(D3D12_VIEWPORT& viewport)
{
	D3D12_RECT scissorRect;
	scissorRect.top    = 0;
	scissorRect.left   = 0;
	scissorRect.right  = static_cast<LONG>(viewport.Width);
	scissorRect.bottom = static_cast<LONG>(viewport.Height);
	m_commandList->RSSetScissorRects(1, &scissorRect);

	m_commandList->RSSetViewports(1, &viewport);
}

//******************************************************************
// @class     �����_�[�^�[�Q�b�g�Ƃ��Ďg�p�\�ɂȂ�܂ő҂�
// @param[in] renderTarget  ID3D12Resource*
// @return    �Ȃ�
//******************************************************************
void FrameBufferManager::WaitUntilToPossibleSetRenderTarget(ID3D12Resource* renderTarget)
{
	// ��ԑJ�ڃo���A�𒣂�
	D3D12_RESOURCE_BARRIER d3d_resource_barrier{};
	d3d_resource_barrier.Type  = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3d_resource_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3d_resource_barrier.Transition.pResource   = renderTarget;
	d3d_resource_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	d3d_resource_barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3d_resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; 
	
	m_commandList->ResourceBarrier(1, &d3d_resource_barrier);
}

//******************************************************************
// @class     �����_�[�^�[�Q�b�g�ւ̕`�����ݑ҂�
// @param[in] renderTarget  ID3D12Resource*
// @return    �Ȃ�
//******************************************************************
void FrameBufferManager::WaitUntilFinishDrawingToRenderTarget(ID3D12Resource* renderTarget)
{
	// ��ԑJ�ڃo���A�𒣂�
	D3D12_RESOURCE_BARRIER d3d_resource_barrier{};
	d3d_resource_barrier.Type  = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3d_resource_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3d_resource_barrier.Transition.pResource   = renderTarget;
	d3d_resource_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3d_resource_barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
	d3d_resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	
	m_commandList->ResourceBarrier(1, &d3d_resource_barrier);
}
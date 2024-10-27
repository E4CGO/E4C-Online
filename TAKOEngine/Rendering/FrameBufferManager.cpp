//! @file FrameBufferManager.h
//! @note 

#include "FrameBufferManager.h"

//******************************************************************
// @class     初期化
// @param[in] commandList　コマンドリスト
// @return    なし
//****************************************************************** 
void FrameBufferManager::Init(ID3D12GraphicsCommandList* commandList)
{
	m_commandList = commandList;
}

//******************************************************************
// @class     コマンドリストを閉じる
// @param[in] なし
// @return    なし
//****************************************************************** 
void FrameBufferManager::Close()
{
	m_commandList->Close();
}

//******************************************************************
// @class      コマンドリストをリセット
// @param[in] commandAllocator ID3D12CommandAllocator*
// @param[in] pipelineState    ID3D12PipelineState*
// @return    なし
//****************************************************************** 
void FrameBufferManager::Reset(ID3D12CommandAllocator* commandAllocator, ID3D12PipelineState* pipelineState)
{
	m_commandList->Reset(commandAllocator, pipelineState);
}

//******************************************************************
// @class     レンダーターゲット設定
// @param[in] renderTarget      レンダーターゲット
// @return    なし
//****************************************************************** 
void FrameBufferManager::SetRenderTarget(FrameBufferDX12* renderTarget)
{
	// ビューポートの設定
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
// @class     レンダーターゲット設定
// @param[in] rtvHandle      レンダーターゲットハンドル
// @param[in] dsvHandle      デプスステンシルハンドル
// @return    なし
//****************************************************************** 
void FrameBufferManager::SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle)
{
	m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
}

//******************************************************************
// @class     複数枚のレンダーターゲット設定
// @param[in] numRTV              レンダーターゲット数
// @param[in] renderTarget[]      レンダーターゲットの配列
// @return    なし
//******************************************************************
void FrameBufferManager::SetRenderTargets(UINT numRTV, FrameBufferDX12* renderTarget[])
{
	// ビューポートの設定
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
		// 深度バッファがある
		D3D12_CPU_DESCRIPTOR_HANDLE dsDS = renderTarget[0]->GetDSVDescriptor()->GetCpuHandle();
		m_commandList->OMSetRenderTargets(numRTV, rtDSHandleTbl, FALSE, &dsDS);
	}
	else
	{
		// 深度バッファがない
		m_commandList->OMSetRenderTargets(numRTV, rtDSHandleTbl, FALSE, nullptr);
	}
}

//******************************************************************
// @class      レンダーターゲットとデプスステンシルビューをクリア
// @param[in]  renderTarget      レンダーターゲット
// @return     なし
//******************************************************************
void FrameBufferManager::Clear(FrameBufferDX12* renderTarget)
{
	FrameBufferDX12* rtArray[] = { renderTarget };
	Clears(1, rtArray);
}

//******************************************************************
// @class     複数枚のレンダーターゲットとデプスステンシルビューをクリア
// @param[in] numRTV              レンダーターゲット数
// @param[in] renderTarget[]      レンダーターゲットの配列
// @return    なし
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
// @class     レンダーターゲットとして使用可能になるまで待つ
// @param[in] renderTarget  レンダーターゲット
// @return    なし
//******************************************************************
void FrameBufferManager::WaitUntilToPossibleSetRenderTarget(FrameBufferDX12* renderTarget)
{
	WaitUntilToPossibleSetRenderTarget(renderTarget->GetRenderTargetTexture().Get());
}

//******************************************************************
// @class     複数枚のレンダーターゲットが使用可能になるまで待つ
// @param[in] numRTV              レンダーターゲット数
// @param[in] renderTarget[]      レンダーターゲットの配列
// @return    なし
//******************************************************************
void FrameBufferManager::WaitUntilToPossibleSetRenderTargets(UINT numRTV, FrameBufferDX12* renderTarget[])
{
	for (UINT i = 0; i < numRTV; i++)
	{
		WaitUntilToPossibleSetRenderTarget(renderTarget[i]);
	}
}

//******************************************************************
// @class     レンダーターゲットへの描き込み待ち
// @param[in] renderTarget  レンダーターゲット
// @return    なし
//******************************************************************
void FrameBufferManager::WaitUntilFinishDrawingToRenderTarget(FrameBufferDX12* renderTarget)
{
	WaitUntilFinishDrawingToRenderTarget(renderTarget->GetRenderTargetTexture().Get());
}

//******************************************************************
// @class     複数枚のレンダーターゲットの描き込み待ち
// @param[in] numRTV              レンダーターゲット数
// @param[in] renderTarget[]      レンダーターゲットの配列
// @return    なし
//******************************************************************
void FrameBufferManager::WaitUntilFinishDrawingToRenderTargets(UINT numRTV, FrameBufferDX12* renderTarget[])
{
	for (UINT i = 0; i < numRTV; i++)
	{
		WaitUntilFinishDrawingToRenderTarget(renderTarget[i]);
	}
}

//******************************************************************
// @class      デプスステンシルビューをクリア
// @param[in]  dsvHandle  デプスステンシルビューのCPUハンドル
// @param[in]  clearValue デプスステンシルビューのクリアカラー
// @return     なし
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
// @class      レンダーターゲットをクリア
// @param[in]  dsvHandle  レンダーターゲットのCPUハンドル
// @param[in]  clearValue レンダーターゲットのクリアカラー
// @return     なし
//******************************************************************
void FrameBufferManager::ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, const float* clearColor)
{
	m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
}

//******************************************************************
// @class     ビューポートとシザリング矩形をセットで設定
// @param[in] viewport  ビューポート
// @return    なし
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
// @class     レンダーターゲットとして使用可能になるまで待つ
// @param[in] renderTarget  ID3D12Resource*
// @return    なし
//******************************************************************
void FrameBufferManager::WaitUntilToPossibleSetRenderTarget(ID3D12Resource* renderTarget)
{
	// 状態遷移バリアを張る
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
// @class     レンダーターゲットへの描き込み待ち
// @param[in] renderTarget  ID3D12Resource*
// @return    なし
//******************************************************************
void FrameBufferManager::WaitUntilFinishDrawingToRenderTarget(ID3D12Resource* renderTarget)
{
	// 状態遷移バリアを張る
	D3D12_RESOURCE_BARRIER d3d_resource_barrier{};
	d3d_resource_barrier.Type  = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3d_resource_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3d_resource_barrier.Transition.pResource   = renderTarget;
	d3d_resource_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3d_resource_barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
	d3d_resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	
	m_commandList->ResourceBarrier(1, &d3d_resource_barrier);
}
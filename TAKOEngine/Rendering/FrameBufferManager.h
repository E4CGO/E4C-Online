//! @file FrameBufferManager.h
//! @note 

#ifndef __GRAHICS_FRAME_BUFFER_MANAGER_H__
#define __GRAHICS_FRAME_BUFFER_MANAGER_H__

#include "FrameBufferDX12.h"

//*******************************************************
// @class FrameBufferManager
// @brief FrameBuffer管理クラス
// @par   
//*******************************************************
class FrameBufferManager
{
public:
	FrameBufferManager() {};

	// 初期化
	void Init(ID3D12GraphicsCommandList* commandList);

	// コマンドリストを閉じる
	void Close();

	// コマンドリストをリセット
	void Reset(ID3D12CommandAllocator* commandAllocator, ID3D12PipelineState* pipelineState); 

	// レンダーターゲット設定
	void SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle);
	void SetRenderTarget(FrameBufferDX12* renderTarget);

	// 複数枚のレンダーターゲット設定
	void SetRenderTargets(UINT numRTV, FrameBufferDX12* renderTarget[]);

	//レンダーターゲットとデプスステンシルビューをクリア
	void Clear(FrameBufferDX12* renderTarget);

	// 複数枚のレンダーターゲットとデプスステンシルビューをクリア
	void Clears(UINT numRTV, FrameBufferDX12* renderTarget[]);

	// レンダーターゲットとして使用可能になるまで待つ
	void WaitUntilToPossibleSetRenderTarget(FrameBufferDX12* renderTarget);

	// 複数枚のレンダーターゲットが使用可能になるまで待つ
	void WaitUntilToPossibleSetRenderTargets(UINT numRTV, FrameBufferDX12* renderTarget[]);

	// レンダーターゲットへの描き込み待ち
	void WaitUntilFinishDrawingToRenderTarget(FrameBufferDX12* renderTarget);

	// 複数枚のレンダーターゲットの描き込み待ち
	void WaitUntilFinishDrawingToRenderTargets(UINT numRTV, FrameBufferDX12* renderTarget[]);

	// ビューポートとシザリング矩形をセットで設定
	void SetViewportAndScissor(D3D12_VIEWPORT& viewport);

	// デプスステンシルビューをクリア
	void ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, float clearValue);

	// レンダーターゲットのクリア
	void ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, const float* clearColor);

	// レンダーターゲットとして使用可能になるまで待つ
	void WaitUntilToPossibleSetRenderTarget(ID3D12Resource* renderTarget);

	// レンダーターゲットへの描き込み待ち
	void WaitUntilFinishDrawingToRenderTarget(ID3D12Resource* renderTarget);

	// コマンドリスト取得
	ID3D12GraphicsCommandList* GetCommandList() { return m_commandList.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
};

#endif // !__GRAHICS_FRAME_BUFFER_MANAGER_H__
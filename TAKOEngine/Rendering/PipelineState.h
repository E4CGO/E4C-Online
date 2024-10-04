#pragma once

#include <d3d12.h>

class PipelineState
{
public:
	~PipelineState()
	{
		if (m_pipelineState) m_pipelineState->Release();
	}

	//初期化
	void Init(ID3D12Device* device, D3D12_GRAPHICS_PIPELINE_STATE_DESC desc);
	void Init(ID3D12Device* device, D3D12_COMPUTE_PIPELINE_STATE_DESC desc);

	ID3D12PipelineState* Get()
	{
		return m_pipelineState;
	}

private:
	ID3D12PipelineState* m_pipelineState = nullptr;
};

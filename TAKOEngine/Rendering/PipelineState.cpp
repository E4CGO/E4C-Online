//! @file PipelineState.cpp
//! @note

#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/PipelineState.h"

//****************************************************
// @brief     ‰Šú‰»
// @param[in] device ID3D12Device*
// @param[in] desc   D3D12_GRAPHICS_PIPELINE_STATE_DESC    
// @return    ‚È‚µ
//****************************************************
void PipelineState::Init(ID3D12Device* device, D3D12_GRAPHICS_PIPELINE_STATE_DESC desc)
{
	HRESULT hr = device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&m_pipelineState));
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

//****************************************************
// @brief     ‰Šú‰»
// @param[in] device ID3D12Device*
// @param[in] desc   D3D12_COMPUTE_PIPELINE_STATE_DESC
// @return    ‚È‚µ
//****************************************************
void PipelineState::Init(ID3D12Device* device, D3D12_COMPUTE_PIPELINE_STATE_DESC desc)
{
	HRESULT hr = device->CreateComputePipelineState(&desc, IID_PPV_ARGS(&m_pipelineState));
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}
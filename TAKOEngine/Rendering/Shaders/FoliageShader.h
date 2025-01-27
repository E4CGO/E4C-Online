//! @file FoliageShader.h
//! @note

#ifndef __GRAHICS_FOLIAGE_SHADER_H__
#define __GRAHICS_FOLIAGE_SHADER_H__

#include "TAKOEngine/Rendering/Shaders/Shader.h"
#include "TAKOEngine/Rendering/Shaders/ModelShader.h"
#include "TAKOEngine/Rendering/Model/ModelResource.h"

#include "ModelShaderDX12.h"

//****************************************************
// @class FoliageShader
// @brief DirectX12の描画
// @par   
//****************************************************
class FoliageShader : public ModelShaderDX12
{
public:
	FoliageShader(ID3D12Device* device);
	~FoliageShader() override;

	void Finalize() override {};

	void Render(const RenderContextDX12& rc, const ModelDX12::Mesh& mesh) override;

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;

	SamplerManager* m_sampler = nullptr;
};

#endif // !__GRAHICS_FOLIAGE_SHADER_H__

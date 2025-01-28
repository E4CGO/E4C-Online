//! @file GrassShader.h
//! @note 

#ifndef __GRAPHICS_GRASS_SHADER_H__
#define __GRAPHICS_GRASS_SHADER_H__

#include "TAKOEngine/Rendering/Shaders/Shader.h"
#include "TAKOEngine/Rendering/Shaders/ModelShader.h"
#include "TAKOEngine/Rendering/Model/ModelResource.h"

#include "ModelShaderDX12.h"

//****************************************************
// @class GrassShader
// @brief DirectX12の描画テスト
// @par   草描画
//****************************************************
class GrassShader : public ModelShaderDX12
{
public:
	GrassShader(ID3D12Device* device);
	~GrassShader() override;

	void Finalize() override {};

	void Render(const RenderContextDX12& rc, const ModelDX12::Mesh& mesh) override;

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;
};

#endif // !__GRAPHICS_GRASS_SHADER_H__

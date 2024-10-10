//! @file LambertShader.h
//! @note

#ifndef __GRAPHICS_LAMBERT_SHADER_H__
#define __GRAPHICS_LAMBERT_SHADER_H__

#include <memory>
#include <d3d12.h>
#include <wrl.h>

#include "TAKOEngine/Rendering/Shaders/Shader.h"

//****************************************************
// @class LambertShader
// @brief DirectX12の描画テスト
// @par   [説明]
//****************************************************
class LambertShader : public ModelShaderDX12
{
public:
	LambertShader();
	~LambertShader() override;

	void Render(const RenderContextDX12& rc, ModelDX12* model) override;

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;

	SamplerManager* m_sampler;
};

#endif // !__GRAPHICS_LAMBERT_SHADER_H__
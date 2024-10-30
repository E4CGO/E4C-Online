//! @file LambertShader.h
//! @note

#ifndef __GRAPHICS_LAMBERT_SHADER_H__
#define __GRAPHICS_LAMBERT_SHADER_H__

#include <memory>
#include <d3d12.h>
#include <wrl.h>

#include "ModelShaderDX12.h"

//****************************************************
// @class LambertShader
// @brief DirectX12の描画テスト
// @par   PBRべース描画
//****************************************************
class LambertShader : public ModelShaderDX12
{
public:
	LambertShader(ID3D12Device* device, bool instancing = false);
	~LambertShader() override;

	void Render(const RenderContextDX12& rc, ModelDX12* model) override;

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;

	SamplerManager* m_sampler;
};

#endif // !__GRAPHICS_LAMBERT_SHADER_H__

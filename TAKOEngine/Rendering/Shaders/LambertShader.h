#pragma once

#include <memory>
#include <d3d12.h>
#include <wrl.h>

#include "TAKOEngine/Rendering/Shaders/Shader.h"

class LambertShader : public Shader
{
public:
	LambertShader();
	~LambertShader() override;

	void Begin(const RenderContext& rc) override;
	void Begin(const RenderContextDX12& rc) override;
	void Draw(const RenderContextDX12& rc, iModel* model) override;
	void Draw(const RenderContextDX12& rc, ModelDX12* model) override;
	void End(const RenderContext& rc) override;
	void End(const RenderContextDX12& rc) override;

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;
};

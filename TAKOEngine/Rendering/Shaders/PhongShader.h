//! @file PhongShader.h
//! @note 

#ifndef __GRAPHICS_PHONG_SHADER_H__
#define __GRAPHICS_PHONG_SHADER_H__

#include "ModelShader.h"
#include "ModelShaderDX12.h"

//*******************************************************
// @class PhongShader
// @brief PhongShaderのパラーメータ設定クラス
// @par   [説明]
// @par   Phong反射描画
//*******************************************************
class PhongShader : public ModelShader
{
public:
	PhongShader(ID3D11Device* device) : ModelShader(device, "Data/Shader/PhongVS.cso", "Data/Shader/PhongPS.cso") {};
	~PhongShader() override = default;
private:
	// レンダーステート設定
	void SetRenderState(const RenderContext& rc) override;
	
	// シェーダーリソースビュー設定
	void SetShaderResourceView(const ModelResource::Mesh& mesh, ID3D11DeviceContext*& dc) override;
};

//*******************************************************
// @class PhongShaderDX12
// @brief PhongShaderDX12のパラーメータ設定クラス
// @par   [説明]
// @par   DX12に対応したPhong反射描画
//*******************************************************
class PhongShaderDX12 : public ModelShaderDX12
{
public:
	PhongShaderDX12(ID3D12Device* device, bool instancing = false);
	~PhongShaderDX12() override;

	void Finalize() override {};

	void Render(const RenderContextDX12& rc, const ModelDX12::Mesh& mesh) override;

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;

	SamplerManager* m_sampler = nullptr;
};

#endif // !__GRAPHICS_PHONG_SHADER_H__
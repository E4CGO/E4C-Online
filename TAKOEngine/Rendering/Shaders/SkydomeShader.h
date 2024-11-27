//! @file SkydomeShader.h
//! @note

#ifndef __GRAHICS_SKYDOME_SHADER_H__
#define __GRAHICS_SKYDOME_SHADER_H__

#include "ModelShader.h"
#include "ModelShaderDX12.h"

//*******************************************************
// @class SkydomeShader
// @brief SkydomeShaderのパラーメータ設定クラス
// @par   [説明]
// @par   skydome描画
//*******************************************************
class SkydomeShader : public ModelShader
{
public:
	SkydomeShader(ID3D11Device* device) : ModelShader(device, "Data/Shader/SkydomeVS.cso", "Data/Shader/SkydomePS.cso") {};
	~SkydomeShader() override = default;
private:
	// レンダーステート設定
	void SetRenderState(const RenderContext& rc) override;

	// シェーダーリソースビュー設定
	void SetShaderResourceView(const ModelResource::Mesh& mesh, ID3D11DeviceContext*& dc) override;
};

//*******************************************************
// @class SkydomeShaderDX12
// @brief SkydomeShaderDX12のパラーメータ設定クラス
// @par   [説明]
// @par   DX12に対応したskydome描画
//*******************************************************
class SkydomeShaderDX12 : public ModelShaderDX12
{
public:
	SkydomeShaderDX12(ID3D12Device* device);
	~SkydomeShaderDX12() override;

	void Render(const RenderContextDX12& rc, iModel* model) override;

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;
};

#endif // !__GRAHICS_SKYDOME_SHADER_H__
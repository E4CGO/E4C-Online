//! @file ToonShader.h
//! @note 

#ifndef __GRAHICS_TOON_SHADER_H__
#define __GRAHICS_TOON_SHADER_H__

#include "ModelShader.h"
#include "ModelShaderDX12.h"

//***********************************************************
// @file   ToonShader
// @brief  ToonShaderのパラーメータ設定クラス
// @par    [説明]
// @par    セルルックな描画
//***********************************************************
class ToonShader : public ModelShader
{
private:
	const char* VertexShaderName = "Data/Shader/ToonVS.cso";
	const char* PixelShaderName = "Data/Shader/ToonPS.cso";

public:
	ToonShader(ID3D11Device* device);
	~ToonShader() override = default;

private:
	// レンダーステート設定
	void SetRenderState(const RenderContext& rc) override;
	
	// シェーダーリソースビュー設定
	void SetShaderResourceView(const ModelResource::Mesh& mesh, ID3D11DeviceContext*& dc) override;

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> toontexture;
};

//***********************************************************
// @file   ToonShaderDX12
// @brief  ToonShaderDX12のパラーメータ設定クラス
// @par    [説明]
// @par    DX12に対応したセルルックな描画
//***********************************************************
class ToonShaderDX12 : public ModelShaderDX12
{
public:
	ToonShaderDX12(ID3D12Device* device, bool instancing = false); 
	~ToonShaderDX12() override;

	void Render(const RenderContextDX12& rc, const ModelDX12::Mesh& mesh) override;

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;

	SamplerManager* m_sampler = nullptr;
};

#endif // !__GRAHICS_TOON_SHADER_H__
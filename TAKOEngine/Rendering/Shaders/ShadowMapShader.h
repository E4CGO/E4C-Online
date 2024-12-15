//! @file ShadowMapShader.h
//! @note

#ifndef __GRAHICS_SHADOW_MAP_SHADER_H__
#define __GRAHICS_SHADOW_MAP_SHADER_H__

#include "TAKOEngine/Rendering/MyRender.h"
#include "ModelShader.h"
#include "ModelShaderDX12.h"

//**********************************************************************
// @class ShadowMapShader
// @brief ShadowMapShaderパラメータの設定クラス
// @par   [説明]
// @par   影描画
//**********************************************************************
class ShadowMapShader : public ModelShader
{
public:
	ShadowMapShader(ID3D11Device* device) : ModelShader(device, "Data/Shader/ShadowMapVS.cso", NULL) {};
	~ShadowMapShader() override = default;

	void Begin(const RenderContext& rc) override;
	void Draw(const RenderContext& rc, const iModel* model, DirectX::XMFLOAT4 color = { 1, 1, 1, 1 }) override;

private:
	// レンダーステート設定
	void SetRenderState(const RenderContext& rc) override;

	// シェーダーリソースビュー設定
	void SetShaderResourceView(const ModelResource::Mesh& mesh, ID3D11DeviceContext*& dc) override {}

	struct CbScene
	{
		DirectX::XMFLOAT4X4 lightViewProjection;
	};
};

//**********************************************************************
// @class ShadowMapShaderDX12
// @brief ShadowMapShaderDX12パラメータの設定クラス
// @par   [説明]
// @par   影描画
//**********************************************************************
class ShadowMapShaderDX12 : public ModelShaderDX12
{
public:
	ShadowMapShaderDX12(ID3D12Device* device);
	~ShadowMapShaderDX12() override {};

	void Finalize() override {};

	void Render(const RenderContextDX12& rc, const ModelDX12::Mesh& mesh) override;

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;
};

#endif // !__GRAHICS_SHADOW_MAP_SHADER_H__
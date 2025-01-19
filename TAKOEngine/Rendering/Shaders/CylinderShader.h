#pragma once

#include "TAKOEngine/Rendering/Cylinder.h"
#include "TAKOEngine/Rendering/Shaders/ModelShader.h"

class CylinderShader : public ModelShader
{
public:
	CylinderShader(ID3D11Device* device, const char* vs, const char* ps);
	~CylinderShader() override = default;

	// 描画開始
	void Begin(const RenderContext& rc) override;

	// モデル描画
	void Draw(const RenderContext& rc, const ModelResource::Mesh& mesh) override;

	void End(const RenderContext& rc) override;

protected:

	// レンダーステート設定
	void SetRenderState(const RenderContext& rc) override;

	// シェーダーリソースビュー設定
	void SetShaderResourceView(const ModelResource::Mesh& mesh, ID3D11DeviceContext*& dc) override;
};

//*******************************************************
// @class ZoneShaderDX12
// @brief ZoneShaderDX12のパラーメータ設定クラス
// @par   [説明]
// @par   DX12に対応したzone描画
//*******************************************************
class ZoneShaderDX12 : public ModelShaderDX12
{
public:
	ZoneShaderDX12(ID3D12Device* device, const char* vertexShaderName = "Data/Shader/PhongDX12VS.cso", const char* pixelShaderName = "Data/Shader/ZoneDX12PS.cso");
	~ZoneShaderDX12() override = default;

	void Finalize() override {};

	void Render(const RenderContextDX12& rc, const ModelDX12::Mesh& mesh) override;

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;

	SamplerManager* m_sampler = nullptr;
};

class ElectricShaderDX12 : public ZoneShaderDX12
{
public:
	ElectricShaderDX12(ID3D12Device* device) : ZoneShaderDX12(device, "Data/Shader/PhongDX12VS.cso", "Data/Shader/ElectricDX12PS.cso") {}

	virtual ~ElectricShaderDX12() override = default;
};

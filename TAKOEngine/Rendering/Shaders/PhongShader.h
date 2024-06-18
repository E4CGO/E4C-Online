#pragma once

#include "ModelShader.h"

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
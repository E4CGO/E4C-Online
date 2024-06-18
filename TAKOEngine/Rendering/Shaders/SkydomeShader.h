#pragma once

#include "ModelShader.h"

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
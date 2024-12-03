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
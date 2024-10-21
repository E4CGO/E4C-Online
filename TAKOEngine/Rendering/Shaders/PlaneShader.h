#pragma once

#include "TAKOEngine/Rendering/Plane.h"
#include "TAKOEngine/Rendering/Shaders/ModelShader.h"

class PlaneShader : public ModelShader
{
public:
	PlaneShader(ID3D11Device* device, const char* vs, const char* ps);
	~PlaneShader() override = default;

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

	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;
};

class PortalShader : public PlaneShader
{
public:
	PortalShader(ID3D11Device* device, const char* vs, const char* ps) : PlaneShader(device, "Data/Shader/PortalVS.cso", "Data/Shader/PortalPS.cso") {}
};

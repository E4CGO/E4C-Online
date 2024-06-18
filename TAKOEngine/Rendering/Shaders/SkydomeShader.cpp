#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/GpuResourceUtils.h"
#include "TAKOEngine/Rendering/Shaders/SkydomeShader.h"

void SkydomeShader::SetRenderState(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	// レンダーステート設定
	const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Opaque), blend_factor, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullBack));
}

// 描画
void SkydomeShader::SetShaderResourceView(const ModelResource::Mesh& mesh, ID3D11DeviceContext*& dc)
{
	// シェーダーリソースビュー設定
	dc->PSSetShaderResources(0, 1, mesh.material->diffuseMap.GetAddressOf());
}
#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/GpuResourceUtils.h"
#include "TAKOEngine/Rendering/Shaders/PhongShader.h"

void PhongShader::SetRenderState(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	// レンダーステート設定
	const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Opaque), blend_factor, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));
}

// 描画
void PhongShader::SetShaderResourceView(const ModelResource::Mesh& mesh, ID3D11DeviceContext*& dc)
{
	// シェーダーリソースビュー設定
	ID3D11ShaderResourceView* srvs[] =
	{
		mesh.material->diffuseMap.Get(),
		mesh.material->normalMap.Get(),
	};
	dc->PSSetShaderResources(0, _countof(srvs), srvs);
}
//! @file PhongShader.cpp
//! @note 

#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/GpuResourceUtils.h"
#include "TAKOEngine/Rendering/Shaders/PhongShader.h"

//********************************************************
// @brief       レンダーステート設定
// @param[in]   rc  レンダーコンテキスト
// @return      なし
//********************************************************
void PhongShader::SetRenderState(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// レンダーステート設定
	const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Opaque), blend_factor, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));
}

//********************************************************
// @brief       シェーダーリソースビュー設定
// @param[in]   mash シェーダーリソースとして設定するメッシュデータ
// @param[in]   dc   デバイスコンテキスト
// @return      なし
//********************************************************
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

//***********************************************************
// @brief       コンストラクタ
// @param[in]   なし
// @return      なし
//***********************************************************
PhongShaderDX12::PhongShaderDX12()
{

}

//***********************************************************
// @brief       デストラクタ
// @param[in]   なし
// @return      なし
//***********************************************************
PhongShaderDX12::~PhongShaderDX12()
{
}

//***********************************************************
// @brief       描画
// @param[in]   rc     レンダーコンテキスト
// @param[in]   model  描画対象のモデルデータを指すポインタ
// @return      なし
//***********************************************************
void PhongShaderDX12::Render(const RenderContextDX12& rc, ModelDX12* model)
{

}

#include "PlaneShader.h"

#include "TAKOEngine/Rendering/GpuResourceUtils.h"
#include "TAKOEngine/Rendering/Shaders/ModelShader.h"

/**************************************************************************//**
	@brief		レンダリングステート設定で
	@param[in]    rc	レンダリングコンテクスト
*//***************************************************************************/
void PlaneShader::SetRenderState(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// レンダーステート設定
	const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Opaque), blend_factor, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));
}

/**************************************************************************//**
	@brief		バファ設定など
	@param[in]    rc	レンダリングコンテクスト
*//***************************************************************************/
void PlaneShader::Begin(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シェーダー設定
	dc->IASetInputLayout(inputLayout.Get());
	rc.deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	rc.deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);

	// 定数バッファ設定
	ID3D11Buffer* constantBuffers[] =
	{
		sceneConstantBuffer.Get(),
	};
	rc.deviceContext->VSSetConstantBuffers(0, _countof(constantBuffers), constantBuffers);
	rc.deviceContext->PSSetConstantBuffers(0, _countof(constantBuffers), constantBuffers);

	// サンプラステート
	ID3D11SamplerState* samplerStates[] =
	{
		rc.renderState->GetSamplerState(SamplerState::LinearWrap),
	};
	dc->PSSetSamplers(0, _countof(samplerStates), samplerStates);

	// レンダーステート設定
	SetRenderState(rc);

	// シーン用定数バッファ更新
	CbScene cbScene{};
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.camera->GetView());
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.camera->GetProjection());

	DirectX::XMStoreFloat4x4(&cbScene.viewProjection, V * P);

	cbScene.directionalLightData = rc.directionalLightData;
	cbScene.ambientLightColor = rc.ambientLightColor;
	memcpy_s(
		cbScene.pointLightData,
		sizeof(cbScene.pointLightData),
		rc.pointLightData,
		sizeof(rc.pointLightData)
	);
	cbScene.pointLightCount = rc.pointLightCount;
	memcpy_s(
		cbScene.spotLightData,
		sizeof(cbScene.spotLightData),
		rc.spotLightData,
		sizeof(rc.spotLightData)
	);
	cbScene.spotLightCount = rc.spotLightCount;

	const DirectX::XMFLOAT3& eye = rc.camera->GetEye();

	cbScene.cameraPosition.x = eye.x;
	cbScene.cameraPosition.y = eye.y;
	cbScene.cameraPosition.z = eye.z;

	cbScene.timerGlobal = rc.timerGlobal;
	cbScene.timerTick = rc.timerTick;

	dc->UpdateSubresource(sceneConstantBuffer.Get(), 0, 0, &cbScene, 0, 0);
}

/**************************************************************************//**
	@brief		テストキャラクター設定
	@param[in]    mesh	メッシュのマテリアル
	@param[in]    dc	コンテクスト
*//***************************************************************************/
void PlaneShader::SetShaderResourceView(const ModelResource::Mesh& mesh, ID3D11DeviceContext*& dc)
{
	// シェーダーリソースビュー設定
	ID3D11ShaderResourceView* srvs[] =
	{
		mesh.material->diffuseMap.Get()
	};
	dc->PSSetShaderResources(0, _countof(srvs), srvs);
}

/**************************************************************************//**
	@brief		ドローコール
	@param[in]    rc	レンダリングコンテクスト
	@param[in]    mesh	メッシュ
*//***************************************************************************/
void PlaneShader::Draw(const RenderContext& rc, const ModelResource::Mesh& mesh)
{
	UINT stride = sizeof(ModelResource::Vertex);
	UINT offset = 0;

	ID3D11DeviceContext* dc = rc.deviceContext;

	dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
	dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	SetShaderResourceView(mesh, dc);

	// 描画
	dc->DrawIndexed(6, 0, 0);
}

/**************************************************************************//**
	@brief		終了
	@param[in]    rc	レンダリングコンテクスト
*//***************************************************************************/
void PlaneShader::End(const RenderContext& rc)
{
}

/**************************************************************************//**
	@brief		バファ設定など
	@param[in]    rc	レンダリング
*//***************************************************************************/
void BillboardShader::Begin(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シェーダー設定
	dc->IASetInputLayout(inputLayout.Get());
	rc.deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	rc.deviceContext->GSSetShader(geometryShader.Get(), nullptr, 0);
	rc.deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);

	// 定数バッファ設定
	ID3D11Buffer* constantBuffers[] =
	{
		sceneConstantBuffer.Get(),
	};
	rc.deviceContext->VSSetConstantBuffers(0, _countof(constantBuffers), constantBuffers);
	rc.deviceContext->GSSetConstantBuffers(0, _countof(constantBuffers), constantBuffers);
	rc.deviceContext->PSSetConstantBuffers(0, _countof(constantBuffers), constantBuffers);

	// サンプラステート
	ID3D11SamplerState* samplerStates[] =
	{
		rc.renderState->GetSamplerState(SamplerState::LinearWrap),
	};
	dc->PSSetSamplers(0, _countof(samplerStates), samplerStates);

	// レンダーステート設定
	SetRenderState(rc);

	// シーン用定数バッファ更新
	CbScene cbScene{};
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.camera->GetView());
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.camera->GetProjection());

	DirectX::XMStoreFloat4x4(&cbScene.viewProjection, V * P);

	cbScene.directionalLightData = rc.directionalLightData;
	cbScene.ambientLightColor = rc.ambientLightColor;
	memcpy_s(
		cbScene.pointLightData,
		sizeof(cbScene.pointLightData),
		rc.pointLightData,
		sizeof(rc.pointLightData)
	);
	cbScene.pointLightCount = rc.pointLightCount;
	memcpy_s(
		cbScene.spotLightData,
		sizeof(cbScene.spotLightData),
		rc.spotLightData,
		sizeof(rc.spotLightData)
	);
	cbScene.spotLightCount = rc.spotLightCount;

	const DirectX::XMFLOAT3& eye = rc.camera->GetEye();

	cbScene.cameraPosition.x = eye.x;
	cbScene.cameraPosition.y = eye.y;
	cbScene.cameraPosition.z = eye.z;

	cbScene.timerGlobal = rc.timerGlobal;
	cbScene.timerTick = rc.timerTick;

	dc->UpdateSubresource(sceneConstantBuffer.Get(), 0, 0, &cbScene, 0, 0);
}

/**************************************************************************//**
	@brief		レンダリングステート設定で
	@param[in]    rc	レンダリングコンテクスト
*//***************************************************************************/
void BillboardShader::SetRenderState(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// レンダーステート設定
	const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Additive), blend_factor, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));
}

/**************************************************************************//**
	@brief		テストキャラクター設定
	@param[in]    mesh	メッシュのマテリアル
	@param[in]    dc	コンテクスト
*//***************************************************************************/
void BillboardShader::SetShaderResourceView(const ModelResource::Mesh& mesh, ID3D11DeviceContext*& dc)
{
	// シェーダーリソースビュー設定
	ID3D11ShaderResourceView* srvs[] =
	{
		mesh.material->diffuseMap.Get()
	};
	dc->PSSetShaderResources(0, _countof(srvs), srvs);
}

/**************************************************************************//**
	@brief		ドローコール
	@param[in]    rc	レンダリングコンテクスト
	@param[in]    mesh	メッシュ
*//***************************************************************************/
void BillboardShader::Draw(const RenderContext& rc, const ModelResource::Mesh& mesh)
{
	UINT stride = sizeof(ModelResource::Vertex);
	UINT offset = 0;

	ID3D11DeviceContext* dc = rc.deviceContext;

	dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
	dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	SetShaderResourceView(mesh, dc);

	// 描画
	dc->DrawIndexed(1, 0, 0);
}

/**************************************************************************//**
	@brief		終了
	@param[in]    rc	レンダリングコンテクスト
*//***************************************************************************/
void BillboardShader::End(const RenderContext& rc)
{
	rc.deviceContext->VSSetShader(nullptr, nullptr, 0);
	rc.deviceContext->PSSetShader(nullptr, nullptr, 0);
	rc.deviceContext->GSSetShader(nullptr, nullptr, 0);
	rc.deviceContext->IASetInputLayout(nullptr);
}
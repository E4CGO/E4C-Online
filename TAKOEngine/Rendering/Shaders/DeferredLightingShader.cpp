#include "DeferredLightingShader.h"
#include "TAKOEngine\Rendering\GpuResourceUtils.h"

DeferredLightingShader::DeferredLightingShader(ID3D11Device* device) 
	: SpriteShader(device, "Data/Shader/DeferredVS.cso", "Data/Shader/DeferredPS.cso")
{
	// シーン用バッファ
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(CbDeferred),
		constantBuffer.GetAddressOf());
}

void DeferredLightingShader::Draw(const RenderContext& rc, const Sprite* sprite)
{
	UpdateConstantBuffer(rc);

	UINT stride = sizeof(Sprite::Vertex);
	UINT offset = 0;
	
	ID3D11ShaderResourceView* srvs[] =
	{
		sprite->GetShaderResourceView().Get(),
		rc.deferredData.normal,
		rc.deferredData.position,
	};
	rc.deviceContext->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
	rc.deviceContext->IASetVertexBuffers(0, 1, sprite->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	rc.deviceContext->Draw(4, 0);
}

void DeferredLightingShader::End(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	dc->VSSetShader(nullptr, nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
	dc->IASetInputLayout(nullptr);

	ID3D11ShaderResourceView* srvs[] = { nullptr, nullptr, nullptr };
	dc->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
}

void DeferredLightingShader::UpdateConstantBuffer(const RenderContext& rc)
{
	CbDeferred cbDeferred{};
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.camera->GetView());
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.camera->GetProjection());
	DirectX::XMStoreFloat4x4(&cbDeferred.viewProjection, V * P);

	cbDeferred.directionalLightData = rc.directionalLightData;
	cbDeferred.ambientLightColor    = rc.ambientLightColor;

	memcpy_s(
		cbDeferred.pointLightData,
		sizeof(cbDeferred.pointLightData),
		rc.pointLightData,
		sizeof(rc.pointLightData));
	cbDeferred.pointLightCount = rc.pointLightCount;

	memcpy_s(
		cbDeferred.spotLightData,
		sizeof(cbDeferred.spotLightData),
		rc.spotLightData,
		sizeof(rc.spotLightData));
	cbDeferred.spotLightCount = rc.spotLightCount;

	const DirectX::XMFLOAT3& eye = rc.camera->GetEye();
	cbDeferred.cameraPosition.x = eye.x;
	cbDeferred.cameraPosition.y = eye.y;
	cbDeferred.cameraPosition.z = eye.z;
	rc.deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &cbDeferred, 0, 0);
}
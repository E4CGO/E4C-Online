#include "FinalpassShader.h"

#include "TAKOEngine/Rendering/GpuResourceUtils.h"

FinalpassShader::FinalpassShader(ID3D11Device* device) : SpriteShader(device, "Data/Shader/SpriteVS.cso", "Data/Shader/FinalpassPS.cso")
{
	// シーン用バッファ
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(CBFinalpass),
		constantBuffer.GetAddressOf()
	);
}

void FinalpassShader::UpdateConstantBuffer(const RenderContext& rc)
{
	CBFinalpass cbFinalpass;
	cbFinalpass.hueShift = rc.colorGradingData.hueShift;
	cbFinalpass.saturation = rc.colorGradingData.saturation;
	cbFinalpass.brightness = rc.colorGradingData.brightness;
	rc.deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &cbFinalpass, 0, 0);
}

void FinalpassShader::Draw(const RenderContext& rc, const Sprite* sprite)
{
	UpdateConstantBuffer(rc);

	UINT stride = sizeof(Sprite::Vertex);
	UINT offset = 0;
	rc.deviceContext->IASetVertexBuffers(0, 1, sprite->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	ID3D11ShaderResourceView* srvs[] =
	{
		sprite->GetShaderResourceView().Get(),
		rc.finalpassnData.bloomTexture,
	};
	rc.deviceContext->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
	rc.deviceContext->Draw(4, 0);
}
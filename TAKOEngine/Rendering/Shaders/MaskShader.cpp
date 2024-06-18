#include "MaskShader.h"

#include "TAKOEngine/Rendering/GpuResourceUtils.h"

MaskShader::MaskShader(ID3D11Device* device) : SpriteShader(device, "Data/Shader/SpriteVS.cso", "Data/Shader/MaskPS.cso")
{
	// シーン用バッファ
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(CbMask),
		constantBuffer.GetAddressOf()
	);
}

void MaskShader::UpdateConstantBuffer(const RenderContext& rc)
{
	//	定数バッファの中身を更新します
	CbMask cbMask;
	cbMask.dissolveThreshold = rc.maskData.dissolveThreshold;
	cbMask.edgeThreshold = rc.maskData.edgeThreshold;
	cbMask.edgeColor = rc.maskData.edgeColor;
	rc.deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &cbMask, 0, 0);
}
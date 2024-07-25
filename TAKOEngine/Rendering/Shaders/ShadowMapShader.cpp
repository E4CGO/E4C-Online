#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/GpuResourceUtils.h"
#include "TAKOEngine/Rendering/Shaders/ShadowMapShader.h"

void ShadowMapShader::Begin(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シェーダー設定
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
	dc->IASetInputLayout(inputLayout.Get());

	// 定数バッファ設定
	ID3D11Buffer* constantBuffers[] =
	{
		sceneConstantBuffer.Get(),
		nullptr,
		skeletonConstantBuffer.Get(),
		nullptr
	};
	dc->VSSetConstantBuffers(0, _countof(constantBuffers), constantBuffers);

	//レンダーステート設定
	SetRenderState(rc);

	// シーン用定数バッファ更新
	CbScene cbScene{};
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.shadowMapData.view);
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.shadowMapData.projection);
	DirectX::XMStoreFloat4x4(&cbScene.lightViewProjection, V * P);
	dc->UpdateSubresource(sceneConstantBuffer.Get(), 0, 0, &cbScene, 0, 0);
}

void ShadowMapShader::Draw(const RenderContext& rc, const Model* model, DirectX::XMFLOAT4 color)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	const ModelResource* resource = model->GetResource();
	const std::vector<Model::Node>& nodes = model->GetNodes();
	
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		// 頂点バッファ設定
		UINT stride = sizeof(ModelResource::Vertex);
		UINT offset = 0;
		dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// スケルトン用定数バッファ更新
		CbSkeleton cbSkeleton{};
		if (mesh.bones.size() > 0)
		{
			for (size_t i = 0; i < mesh.bones.size(); ++i)
			{
				DirectX::XMMATRIX worldTransform  = DirectX::XMLoadFloat4x4(&nodes.at(mesh.bones.at(i).nodeIndex).worldTransform);
				DirectX::XMMATRIX offsetTransform = DirectX::XMLoadFloat4x4(&mesh.bones.at(i).offsetTransform);
				DirectX::XMMATRIX boneTransform   = offsetTransform * worldTransform;
				DirectX::XMStoreFloat4x4(&cbSkeleton.boneTransforms[i], boneTransform);
			}
		}
		else
		{
			cbSkeleton.boneTransforms[0] = nodes.at(mesh.nodeIndex).worldTransform;
		}
		rc.deviceContext->UpdateSubresource(skeletonConstantBuffer.Get(), 0, 0, &cbSkeleton, 0, 0);

		// 描画
		dc->DrawIndexed(static_cast<UINT>(mesh.indices.size()), 0, 0);
	}
}

// レンダーステート設定
void ShadowMapShader::SetRenderState(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// レンダーステート設定
	const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Transparency), blend_factor, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(nullptr, 0);
	dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullBack));
}
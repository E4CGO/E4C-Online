#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/GpuResourceUtils.h"
#include "TAKOEngine/Rendering/Shaders/ShadowMapShader.h"

void ShadowMapShader::Begin(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// �V�F�[�_�[�ݒ�
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
	dc->IASetInputLayout(inputLayout.Get());

	// �萔�o�b�t�@�ݒ�
	ID3D11Buffer* constantBuffers[] =
	{
		sceneConstantBuffer.Get(),
		nullptr,
		skeletonConstantBuffer.Get(),
		nullptr
	};
	dc->VSSetConstantBuffers(0, _countof(constantBuffers), constantBuffers);

	//�����_�[�X�e�[�g�ݒ�
	SetRenderState(rc);

	// �V�[���p�萔�o�b�t�@�X�V
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
		// ���_�o�b�t�@�ݒ�
		UINT stride = sizeof(ModelResource::Vertex);
		UINT offset = 0;
		dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// �X�P���g���p�萔�o�b�t�@�X�V
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

		// �`��
		dc->DrawIndexed(static_cast<UINT>(mesh.indices.size()), 0, 0);
	}
}

// �����_�[�X�e�[�g�ݒ�
void ShadowMapShader::SetRenderState(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// �����_�[�X�e�[�g�ݒ�
	const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Transparency), blend_factor, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(nullptr, 0);
	dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullBack));
}
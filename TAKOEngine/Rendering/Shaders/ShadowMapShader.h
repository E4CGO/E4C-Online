#pragma once

#include "TAKOEngine/Rendering/MyRender.h"
#include "ModelShader.h"

class ShadowMapShader : public ModelShader
{
public:
	ShadowMapShader(ID3D11Device* device) : ModelShader(device, "Data/Shader/ShadowMapVS.cso", NULL) {};
	~ShadowMapShader() override = default;

	void Begin(const RenderContext& rc) override;
	void Draw(const RenderContext& rc, const Model* model, DirectX::XMFLOAT4 color = { 1, 1, 1, 1 }) override;

private:
	// �����_�[�X�e�[�g�ݒ�
	void SetRenderState(const RenderContext& rc) override;

	// �V�F�[�_�[���\�[�X�r���[�ݒ�
	void SetShaderResourceView(const ModelResource::Mesh& mesh, ID3D11DeviceContext*& dc) override {}

	struct CbScene
	{
		DirectX::XMFLOAT4X4 lightViewProjection;
	};
};
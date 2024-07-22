#pragma once

#include <memory>
#include <vector>

#include "TAKOEngine\Rendering\MyRender.h"
#include "TAKOEngine\Rendering\RenderContext.h"
#include "TAKOEngine\Rendering\FrameBuffer.h"
#include "TAKOEngine\Rendering\Model.h"
#include "TAKOEngine\Rendering\Light.h"

// �[�x�X�e���V���o�b�t�@
class DepthStencil
{
public:
	DepthStencil(UINT width, UINT height);
	~DepthStencil() {}

	// �V�F�[�_�[���\�[�X�r���[�擾
	const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetShaderResourceView() { return shaderResourceView; }

	// �[�x�X�e���V���r���[�擾
	const Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDepthStencilView() { return depthStencilView; }

	// �e�N�X�`���̏��擾
	D3D11_TEXTURE2D_DESC GetTexture2dDesc() { return texture2dDesc; }

	// �e�N�X�`�����擾
	inline int GetWidth() const { return texture2dDesc.Width; }

	// �e�N�X�`�������擾
	inline int GetHeight() const { return texture2dDesc.Height; }

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		depthStencilView;
	D3D11_TEXTURE2D_DESC								texture2dDesc;
};

namespace myRenderer
{
	namespace shadow
	{
		class ShadowMapRender
		{
		public:
			//������
			void Initialize();

			//�V���h�E�}�b�v�ɕ`�悷�郂�f����o�^
			void ModelRegister(Model* model);

			//�V���h�E�}�b�v�Ɏg�p���郉�C�g���擾
			void SetShadowLight(Light* light);

			//�S�폜
			void Clear();

			//�`��
			void Render();

			//GUI
			void DrawDebugGUI();

			// �V���h�E�}�b�v����RenderContext�ɐς�
			ShadowMapData GetShadowMapData();

		private:
			std::unique_ptr<DepthStencil>	depthStencil[NUM_SHADOW_MAP];	// �V���h�E�}�b�v�p�[�x�o�b�t�@
			std::vector<Model*> m_models;
			Light* light = nullptr;

			//���C�g�r���[�v���W�F�N�V�����s��
			DirectX::XMFLOAT4X4 lightViewProjection[NUM_SHADOW_MAP] = {};

			DirectX::XMFLOAT4X4 view = {};
			DirectX::XMFLOAT4X4 projection = {};

			//�[�x�X�e���V���ݒ�p
			D3D11_TEXTURE2D_DESC texture2dDesc;
			UINT width = 1024;
		};

	}
}
 
#pragma once

#include <vector>
#include <d3d12.h>
#include <DirectXMath.h>

#include "TAKOEngine/Rendering/Descriptor.h"
#include "TAKOEngine/Rendering/RenderContext.h"
#include "TAKOEngine/Rendering/Model.h"
#include "TAKOEngine/Rendering/ModelDX12.h"
#include "TAKOEngine/Rendering/Sprite.h"

// �����_�[�R���e�L�X�g
struct RenderContextDX12
{
	ID3D12GraphicsCommandList* d3d_command_list = nullptr;
	Descriptor* scene_cbv_descriptor = nullptr;

	DirectX::XMFLOAT4X4			view;
	DirectX::XMFLOAT4X4			projection;
	DirectX::XMFLOAT4			light_direction;
};

class Shader
{
public:
	Shader() {}
	virtual ~Shader() {}

	// �`��J�n
	virtual void Begin(const RenderContext& rc) = 0;
	virtual void Begin(const RenderContextDX12& rc) = 0;
	virtual void Draw(const RenderContextDX12& rc, Model* model) = 0;
	virtual void Draw(const RenderContextDX12& rc, ModelDX12* model) = 0;

	// �`��I��
	virtual void End(const RenderContext& rc)
	{
		ID3D11DeviceContext* dc = rc.deviceContext;
		dc->VSSetShader(nullptr, nullptr, 0);
		dc->PSSetShader(nullptr, nullptr, 0);
		dc->IASetInputLayout(nullptr);
		
		ID3D11ShaderResourceView* srvs[] = { nullptr, nullptr };
		dc->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
		ID3D11ShaderResourceView* shadowSrvs[] = { nullptr, nullptr, nullptr, nullptr };
		dc->PSSetShaderResources(10, ARRAYSIZE(shadowSrvs), shadowSrvs);
	}
	virtual void End(const RenderContextDX12& rc) = 0;

protected:
	void LoadShaderFile(const char* filename, std::vector<BYTE>& data);
};

#pragma once

#include "TAKOEngine/Rendering/Sprite.h"
#include "TAKOEngine/Rendering/Shaders/Shader.h"

class SpriteShader : public Shader
{
public:
	SpriteShader(ID3D11Device* device, const char* vs, const char* ps);
	virtual ~SpriteShader() override = default;

	// •`‰æŠJŽn
	virtual void Begin(const RenderContext& rc) override;
	void Begin(const RenderContextDX12& rc) override;
	void Draw(const RenderContextDX12& rc, Model* model) override;
	void Draw(const RenderContextDX12& rc, ModelDX12* model) override;
	void End(const RenderContext& rc) override;
	void End(const RenderContextDX12& rc) override;

	// ƒ‚ƒfƒ‹•`‰æ
	virtual void Draw(const RenderContext& rc, const Sprite* sprite);

protected:
	virtual void UpdateConstantBuffer(const RenderContext& rc) {};
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer>				constantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;
};

class DefaultSpriteShader : public SpriteShader
{
public:
	DefaultSpriteShader(ID3D11Device* device) : SpriteShader(device, "Data/Shader/SpriteVS.cso", "Data/Shader/SpritePS.cso") {}
};
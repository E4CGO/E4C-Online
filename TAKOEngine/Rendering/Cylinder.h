#pragma once

#include <array>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "TAKOEngine/Rendering/RenderContext.h"
#include "TAKOEngine/Physics/Collider/Collider.h"
#include "TAKOEngine/Rendering/Sprite.h"

#include "GameObject/ModelObject.h"

class Cylinder : public ModelObject
{
public:
	Cylinder(ID3D11Device* device, const char* filename, float scaling = 1.0f);
	virtual ~Cylinder()
	{
		if (mesh.material != nullptr)
		{
			delete mesh.material;
			mesh.material = nullptr;
		}
	}

	virtual void Update(float elapsedTime) override;

	virtual void Render(const RenderContext& rc) override;

public:
	// アクセサ
	// 位置取得
	const DirectX::XMFLOAT3& GetPosition() const { return position; }
	// 位置設定
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }
	// 回転取得
	const DirectX::XMFLOAT3& GetAngle() const { return angle; }
	// 回転設定
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }
	// スケール取得
	const DirectX::XMFLOAT3& GetScale() const { return scale; }
	// スケール設定
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
protected:

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	ModelResource::Mesh mesh;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;

	DirectX::XMFLOAT2 textureSize = {};
};
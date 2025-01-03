//! @file Plane.h
//! @note

#ifndef __INCLUDED_PLANE_H__
#define __INCLUDED_PLANE_H__

#include <array>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "TAKOEngine/Rendering/RenderContext.h"
#include "TAKOEngine/Physics/Collider.h"
#include "TAKOEngine/Rendering/Sprite.h"
#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"

#include "GameObject/ModelObject.h"

/**************************************************************************//**
	@class  Plane
	@brief	板作るクラス
	@par    [説明]
		UEみたい板オブジェクト
*//***************************************************************************/
class Plane : public ModelObject
{
public:
	Plane(ID3D11Device* device, const char* filename, float scaling = 1.0f, std::array<DirectX::XMFLOAT3, 4> positions =
		{
			DirectX::XMFLOAT3{1.0f, 1.0f, 0.0f},
			DirectX::XMFLOAT3{-1.0f, 1.0f, 0.0f},
			DirectX::XMFLOAT3{1.0f, -1.0f, 0.0f},
			DirectX::XMFLOAT3{-1.0f, -1.0f, 0.0f},
		});
	virtual ~Plane()
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

/**************************************************************************//**
	@class　Billboard
	@brief	看板作るクラス
	@par    [説明]
		いつもカメラに向いているUEみたい板オブジェクト
*//***************************************************************************/
class Billboard : public ModelObject
{
public:

	Billboard(ID3D11Device* device, const char* filename, float scaling = 1.0f, DirectX::XMFLOAT3 position = { 0.f, 0.f, 0.f });
	virtual ~Billboard()
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

/**************************************************************************//**
	@class	Fireball
	@brief	魔法使い火球を作るクラス
	@par    [説明]
		魔法使い攻撃
*//***************************************************************************/
class Fireball : public Billboard
{
public:
	Fireball(ID3D11Device* device, const char* filename, float scaling = 1.0f, DirectX::XMFLOAT3 position = { 0.f, 0.f, 0.f });
	virtual ~Fireball()
	{
		if (mesh.material != nullptr)
		{
			delete mesh.material;
			mesh.material = nullptr;
		}
	}
};

#endif //!__INCLUDED_PLANE_H__

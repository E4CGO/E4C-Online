#pragma once

#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "RenderContext.h"

class Gizmos
{
public:
	Gizmos(ID3D11Device* device);
	~Gizmos() {}

	// 箱描画
	void DrawBox(
		const DirectX::XMFLOAT3& position,
		const DirectX::XMFLOAT3& angle,
		const DirectX::XMFLOAT3& size,
		const DirectX::XMFLOAT4& color
	);

	// 球メッシュ作成
	void DrawShpere(
		const DirectX::XMFLOAT3& position,
		float radius,
		const DirectX::XMFLOAT4& color
	);

	void Render(const RenderContext& rc);
private:
	struct Mesh
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
		UINT vertexCount;
	};

	struct Instance
	{
		Mesh* mesh;
		DirectX::XMFLOAT4X4 worldTransform;
		DirectX::XMFLOAT4 color;
	};
	struct CbMesh
	{
		DirectX::XMFLOAT4X4 worldViewProjection;
		DirectX::XMFLOAT4 color;
	};

	// メッシュ生成
	void CreateMesh(ID3D11Device* device, const std::vector<DirectX::XMFLOAT3>& vertices, Mesh& mesh);

	// 箱メッシュっ作成
	void CreateBoxMesh(ID3D11Device* device, float width, float height, float depth);

	// 球メッシュ作成
	void CreateSphereMesh(ID3D11Device* device, float raduis, int subdivisions);
private:
	Mesh boxMesh;
	std::vector<Instance> instances;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

	Mesh sphereMesh;
};

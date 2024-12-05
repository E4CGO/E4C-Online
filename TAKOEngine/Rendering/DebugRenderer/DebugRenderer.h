//! @file DebugRenderer.h
//! @note 

#ifndef __GRAHICS_DEBUG_RENDERER_H__
#define __GRAHICS_DEBUG_RENDERER_H__

#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <d3d12.h>
#include <DirectXMath.h>

#include "TAKOEngine\Rendering\RenderContext.h"
#include "TAKOEngine\Rendering\Descriptor.h"

//*********************************************************
// @file   DebugRenderer
// @brief  デバック描画
// @par    
//*********************************************************
class DebugRenderer
{
public:
	DebugRenderer(ID3D11Device* device);
	~DebugRenderer() {}

public:
	// 描画実行
	void Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

	// 球描画
	void SetSphere(const DirectX::XMFLOAT3& center, float radius, const DirectX::XMFLOAT4& color);
	void SetSphere(const std::vector<DirectX::XMFLOAT3>& centers, float radius, const DirectX::XMFLOAT4& color);

	// 円柱描画
	void DrawCylinder(const DirectX::XMFLOAT3& position, float radius, float height, const DirectX::XMFLOAT4& color);

	// 立方体描画
	void DrawCube(const DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT4& color);

	// カプセル描画
	void DrawCapsule(const DirectX::XMFLOAT3& position, float radius, float height, const DirectX::XMFLOAT4& color);

private:
	// 球メッシュ作成
	void CreateSphereMesh(ID3D11Device* device, float radius, int slices, int stacks);

	// 円柱メッシュ作成
	void CreateCylinderMesh(ID3D11Device* device, float radius1, float radius2, float start, float height, int slices, int stacks);

	// 立方体メッシュ作成
	void CreateCubeMesh(ID3D11Device* device);
private:
	struct CbMesh
	{
		DirectX::XMFLOAT4X4	wvp;
		DirectX::XMFLOAT4	color;
	};

	struct Sphere
	{
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT3	center;
		float				radius;
	};

	struct Cylinder
	{
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT3	position;
		float				radius;
		float				height;
	};

	struct Cube
	{
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT3	scale;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer>			sphereVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			cylinderVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			cubeVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			constantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState;

	std::vector<Sphere>		spheres;
	std::vector<Cylinder>	cylinders;
	std::vector<Cube>	cubes;

	UINT	sphereVertexCount = 0;
	UINT	cylinderVertexCount = 0;
	UINT	cubeVertexCount = 0;
};

#endif // !__GRAHICS_DEBUG_RENDERER_H__
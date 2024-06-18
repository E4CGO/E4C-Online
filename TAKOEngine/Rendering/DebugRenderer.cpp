#include <stdio.h>
#include <memory>
#include "Misc.h"
#include "DebugRenderer.h"
#include "GpuResourceUtils.h"

DebugRenderer::DebugRenderer(ID3D11Device* device)
{
	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	// 頂点シェーダー
	GpuResourceUtils::LoadVertexShader(
		device,
		"Data/Shader/DebugVS.cso",
		inputElementDesc,
		_countof(inputElementDesc),
		inputLayout.GetAddressOf(),
		vertexShader.GetAddressOf()
	);

	// ピクセルシェーダー
	GpuResourceUtils::LoadPixelShader(
		device,
		"Data/Shader/DebugPS.cso",
		pixelShader.GetAddressOf()
	);

	// 定数バッファ
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(CbMesh),
		constantBuffer.GetAddressOf()
	);

	// ブレンドステート
	{
		D3D11_BLEND_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = false;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = device->CreateBlendState(&desc, blendState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// 深度ステンシルステート
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		HRESULT hr = device->CreateDepthStencilState(&desc, depthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// ラスタライザーステート
	{
		D3D11_RASTERIZER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.FrontCounterClockwise = true;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = true;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.AntialiasedLineEnable = false;

		HRESULT hr = device->CreateRasterizerState(&desc, rasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// 球メッシュ作成
	CreateSphereMesh(device, 1.0f, 16, 16);

	// 円柱メッシュ作成
	CreateCylinderMesh(device, 1.0f, 1.0f, 0.0f, 1.0f, 16, 1);
	// 立方体メッシュ作成
	CreateCubeMesh(device);
}

// 描画開始
void DebugRenderer::Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	// シェーダー設定
	context->VSSetShader(vertexShader.Get(), nullptr, 0);
	context->PSSetShader(pixelShader.Get(), nullptr, 0);
	context->IASetInputLayout(inputLayout.Get());

	// 定数バッファ設定
	context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	// レンダーステート設定
	const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	context->OMSetBlendState(blendState.Get(), blendFactor, 0xFFFFFFFF);
	context->OMSetDepthStencilState(depthStencilState.Get(), 0);
	context->RSSetState(rasterizerState.Get());

	// ビュープロジェクション行列作成
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX VP = V * P;

	// プリミティブ設定
	UINT stride = sizeof(DirectX::XMFLOAT3);
	UINT offset = 0;
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// 球描画
	context->IASetVertexBuffers(0, 1, sphereVertexBuffer.GetAddressOf(), &stride, &offset);
	for (const Sphere& sphere : spheres)
	{
		// ワールドビュープロジェクション行列作成
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(sphere.radius, sphere.radius, sphere.radius);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(sphere.center.x, sphere.center.y, sphere.center.z);
		DirectX::XMMATRIX W = S * T;
		DirectX::XMMATRIX WVP = W * VP;

		// 定数バッファ更新
		CbMesh cbMesh;
		cbMesh.color = sphere.color;
		DirectX::XMStoreFloat4x4(&cbMesh.wvp, WVP);

		context->UpdateSubresource(constantBuffer.Get(), 0, 0, &cbMesh, 0, 0);
		context->Draw(sphereVertexCount, 0);
	}
	spheres.clear();

	// 円柱描画
	context->IASetVertexBuffers(0, 1, cylinderVertexBuffer.GetAddressOf(), &stride, &offset);
	for (const Cylinder& cylinder : cylinders)
	{
		// ワールドビュープロジェクション行列作成
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(cylinder.radius, cylinder.height, cylinder.radius);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(cylinder.position.x, cylinder.position.y, cylinder.position.z);
		DirectX::XMMATRIX W = S * T;
		DirectX::XMMATRIX WVP = W * VP;

		// 定数バッファ更新
		CbMesh cbMesh;
		cbMesh.color = cylinder.color;
		DirectX::XMStoreFloat4x4(&cbMesh.wvp, WVP);

		context->UpdateSubresource(constantBuffer.Get(), 0, 0, &cbMesh, 0, 0);
		context->Draw(cylinderVertexCount, 0);
	}
	cylinders.clear();

	// 立方体描画
	context->IASetVertexBuffers(0, 1, cubeVertexBuffer.GetAddressOf(), &stride, &offset);
	for (const Cube& cube : cubes)
	{
		// ワールドビュープロジェクション行列作成
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(cube.scale.x, cube.scale.y, cube.scale.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(cube.position.x, cube.position.y, cube.position.z);
		DirectX::XMMATRIX W = S * T;
		DirectX::XMMATRIX WVP = W * VP;

		// 定数バッファ更新
		CbMesh cbMesh;
		cbMesh.color = cube.color;
		DirectX::XMStoreFloat4x4(&cbMesh.wvp, WVP);

		context->UpdateSubresource(constantBuffer.Get(), 0, 0, &cbMesh, 0, 0);
		context->Draw(cubeVertexCount, 0);
	}
	cubes.clear();
}

// 球描画
void DebugRenderer::DrawSphere(const DirectX::XMFLOAT3& center, float radius, const DirectX::XMFLOAT4& color)
{
	Sphere sphere;
	sphere.center = center;
	sphere.radius = radius;
	sphere.color = color;
	spheres.emplace_back(sphere);
}

// 円柱描画
void DebugRenderer::DrawCylinder(const DirectX::XMFLOAT3& position, float radius, float height, const DirectX::XMFLOAT4& color)
{
	Cylinder cylinder;
	cylinder.position = position;
	cylinder.radius = radius;
	cylinder.height = height;
	cylinder.color = color;
	cylinders.emplace_back(cylinder);
}

// 立方体描画
void DebugRenderer::DrawCube(const DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT4& color)
{
	Cube cube;
	cube.position = position;
	cube.scale = scale;
	cube.color = color;
	cubes.emplace_back(cube);
}

// 球メッシュ作成
void DebugRenderer::CreateSphereMesh(ID3D11Device* device, float radius, int slices, int stacks)
{
	sphereVertexCount = stacks * slices * 2 + slices * stacks * 2;
	std::unique_ptr<DirectX::XMFLOAT3[]> vertices = std::make_unique<DirectX::XMFLOAT3[]>(sphereVertexCount);

	float phiStep = DirectX::XM_PI / stacks;
	float thetaStep = DirectX::XM_2PI / slices;

	DirectX::XMFLOAT3* p = vertices.get();

	for (int i = 0; i < stacks; ++i)
	{
		float phi = i * phiStep;
		float y = radius * cosf(phi);
		float r = radius * sinf(phi);

		for (int j = 0; j < slices; ++j)
		{
			float theta = j * thetaStep;
			p->x = r * sinf(theta);
			p->y = y;
			p->z = r * cosf(theta);
			p++;

			theta += thetaStep;

			p->x = r * sinf(theta);
			p->y = y;
			p->z = r * cosf(theta);
			p++;
		}
	}

	thetaStep = DirectX::XM_2PI / stacks;
	for (int i = 0; i < slices; ++i)
	{
		DirectX::XMMATRIX M = DirectX::XMMatrixRotationY(i * thetaStep);
		for (int j = 0; j < stacks; ++j)
		{
			float theta = j * thetaStep;
			DirectX::XMVECTOR V1 = DirectX::XMVectorSet(radius * sinf(theta), radius * cosf(theta), 0.0f, 1.0f);
			DirectX::XMVECTOR P1 = DirectX::XMVector3TransformCoord(V1, M);
			DirectX::XMStoreFloat3(p++, P1);

			int n = (j + 1) % stacks;
			theta += thetaStep;

			DirectX::XMVECTOR V2 = DirectX::XMVectorSet(radius * sinf(theta), radius * cosf(theta), 0.0f, 1.0f);
			DirectX::XMVECTOR P2 = DirectX::XMVector3TransformCoord(V2, M);
			DirectX::XMStoreFloat3(p++, P2);
		}
	}

	// 頂点バッファ
	{
		D3D11_BUFFER_DESC desc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		desc.ByteWidth = static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * sphereVertexCount);
		desc.Usage = D3D11_USAGE_IMMUTABLE;	// D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		subresourceData.pSysMem = vertices.get();
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;

		HRESULT hr = device->CreateBuffer(&desc, &subresourceData, sphereVertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

// 円柱メッシュ作成
void DebugRenderer::CreateCylinderMesh(ID3D11Device* device, float radius1, float radius2, float start, float height, int slices, int stacks)
{
	cylinderVertexCount = 2 * slices * (stacks + 1) + 2 * slices;
	std::unique_ptr<DirectX::XMFLOAT3[]> vertices = std::make_unique<DirectX::XMFLOAT3[]>(cylinderVertexCount);

	DirectX::XMFLOAT3* p = vertices.get();

	float stackHeight = height / stacks;
	float radiusStep = (radius2 - radius1) / stacks;

	// vertices of ring
	float dTheta = DirectX::XM_2PI / slices;

	for (int i = 0; i < slices; ++i)
	{
		int n = (i + 1) % slices;

		float c1 = cosf(i * dTheta);
		float s1 = sinf(i * dTheta);

		float c2 = cosf(n * dTheta);
		float s2 = sinf(n * dTheta);

		for (int j = 0; j <= stacks; ++j)
		{
			float y = start + j * stackHeight;
			float r = radius1 + j * radiusStep;

			p->x = r * c1;
			p->y = y;
			p->z = r * s1;
			p++;

			p->x = r * c2;
			p->y = y;
			p->z = r * s2;
			p++;
		}

		p->x = radius1 * c1;
		p->y = start;
		p->z = radius1 * s1;
		p++;

		p->x = radius2 * c1;
		p->y = start + height;
		p->z = radius2 * s1;
		p++;
	}

	// 頂点バッファ
	{
		D3D11_BUFFER_DESC desc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		desc.ByteWidth = static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * cylinderVertexCount);
		desc.Usage = D3D11_USAGE_IMMUTABLE;	// D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		subresourceData.pSysMem = vertices.get();
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;

		HRESULT hr = device->CreateBuffer(&desc, &subresourceData, cylinderVertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

// 立方体メッシュ作成
void DebugRenderer::CreateCubeMesh(ID3D11Device* device)
{
	cubeVertexCount = 24;
	std::unique_ptr<DirectX::XMFLOAT3[]> vertices = std::make_unique<DirectX::XMFLOAT3[]>(cubeVertexCount);

	DirectX::XMFLOAT3* p = vertices.get();
	p[0] = { +0.5f, +0.5f, -0.5f };
	p[1] = { -0.5f, +0.5f, -0.5f };
	p[2] = { -0.5f, +0.5f, -0.5f };
	p[3] = { -0.5f, -0.5f, -0.5f };
	p[4] = { -0.5f, -0.5f, -0.5f };
	p[5] = { +0.5f, -0.5f, -0.5f };

	p[6] = { +0.5f, +0.5f, +0.5f };
	p[7] = { +0.5f, +0.5f, -0.5f };
	p[8] = { +0.5f, +0.5f, -0.5f };
	p[9] = { +0.5f, -0.5f, -0.5f };
	p[10] = { +0.5f, -0.5f, -0.5f };
	p[11] = { +0.5f, -0.5f, +0.5f };

	p[12] = { -0.5f, +0.5f, +0.5f };
	p[13] = { +0.5f, +0.5f, +0.5f };
	p[14] = { +0.5f, +0.5f, +0.5f };
	p[15] = { +0.5f, -0.5f, +0.5f };
	p[16] = { +0.5f, -0.5f, +0.5f };
	p[17] = { -0.5f, -0.5f, +0.5f };

	p[18] = { -0.5f, +0.5f, -0.5f };
	p[19] = { -0.5f, +0.5f, +0.5f };
	p[20] = { -0.5f, +0.5f, +0.5f };
	p[21] = { -0.5f, -0.5f, +0.5f };
	p[22] = { -0.5f, -0.5f, +0.5f };
	p[23] = { -0.5f, -0.5f, -0.5f };

	// 頂点バッファ
	{
		D3D11_BUFFER_DESC desc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		desc.ByteWidth = static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * cubeVertexCount);
		desc.Usage = D3D11_USAGE_IMMUTABLE;	// D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		subresourceData.pSysMem = vertices.get();
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;

		HRESULT hr = device->CreateBuffer(&desc, &subresourceData, cubeVertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}
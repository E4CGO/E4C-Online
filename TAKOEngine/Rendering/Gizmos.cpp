#include "Misc.h"
#include "GpuResourceUtils.h"
#include "Gizmos.h"

// コンストラクタ
Gizmos::Gizmos(ID3D11Device* device)
{
	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// 頂点シェーダー
	GpuResourceUtils::LoadVertexShader(
		device,
		"Data/Shader/GizmosVs.cso",
		inputElementDesc,
		_countof(inputElementDesc),
		inputLayout.GetAddressOf(),
		vertexShader.GetAddressOf()
	);

	// ピクセルシェーダー
	GpuResourceUtils::LoadPixelShader(
		device,
		"Data/Shader/GizmosPS.cso",
		pixelShader.GetAddressOf()
	);

	// 定数バッファ
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(CbMesh),
		constantBuffer.GetAddressOf()
	);

	// 箱メッシュ生成
	CreateBoxMesh(device, 0.5f, 0.5f, 0.5f);
	CreateSphereMesh(device, 1.0f, 32);
}

// 箱描画
void Gizmos::DrawBox(
	const DirectX::XMFLOAT3& position,
	const DirectX::XMFLOAT3& angle,
	const DirectX::XMFLOAT3& size,
	const DirectX::XMFLOAT4& color
)
{
	Instance& instance = instances.emplace_back();
	instance.mesh = &boxMesh;
	instance.color = color;

	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(size.x, size.y, size.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMStoreFloat4x4(&instance.worldTransform, S * R * T);
}

void Gizmos::DrawShpere(
	const DirectX::XMFLOAT3& position,
	float radius,
	const DirectX::XMFLOAT4& color
)
{
	Instance& instance = instances.emplace_back();
	instance.mesh = &sphereMesh;
	instance.color = color;

	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(radius, radius, radius);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMStoreFloat4x4(&instance.worldTransform, S * T);
}

// メッシュ生成
void Gizmos::CreateMesh(ID3D11Device* device, const std::vector<DirectX::XMFLOAT3>& vertices, Mesh& mesh)
{
	D3D11_BUFFER_DESC desc = {  };
	desc.ByteWidth = static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * vertices.size());
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pSysMem = vertices.data();
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&desc, &subresourceData, mesh.vertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	mesh.vertexCount = static_cast<UINT>(vertices.size());
}

// 箱メッシュ生成
void Gizmos::CreateBoxMesh(ID3D11Device* device, float width, float height, float depth)
{
	DirectX::XMFLOAT3 position[8] = {
		//top
		{ -width, height, -depth },
		{  width, height, -depth },
		{  width, height,  depth },
		{ -width, height,  depth },
		// bottom
		{ -width, -height, -depth },
		{  width, -height, -depth },
		{  width, -height, depth },
		{ -width, -height, depth },
	};

	std::vector<DirectX::XMFLOAT3> vertices;
	vertices.resize(32);

	// top
	vertices.emplace_back(position[0]);
	vertices.emplace_back(position[1]);
	vertices.emplace_back(position[1]);
	vertices.emplace_back(position[2]);
	vertices.emplace_back(position[2]);
	vertices.emplace_back(position[3]);
	vertices.emplace_back(position[3]);
	vertices.emplace_back(position[0]);
	// bottom
	vertices.emplace_back(position[4]);
	vertices.emplace_back(position[5]);
	vertices.emplace_back(position[5]);
	vertices.emplace_back(position[6]);
	vertices.emplace_back(position[6]);
	vertices.emplace_back(position[7]);
	vertices.emplace_back(position[7]);
	vertices.emplace_back(position[4]);
	//side
	vertices.emplace_back(position[0]);
	vertices.emplace_back(position[4]);
	vertices.emplace_back(position[1]);
	vertices.emplace_back(position[5]);
	vertices.emplace_back(position[2]);
	vertices.emplace_back(position[6]);
	vertices.emplace_back(position[3]);
	vertices.emplace_back(position[7]);

	// メッシュ生成
	CreateMesh(device, vertices, boxMesh);
}

// 球メッシュ作成
void Gizmos::CreateSphereMesh(ID3D11Device* device, float radius, int subdivisions)
{
	float step = DirectX::XM_2PI / subdivisions;
	std::vector<DirectX::XMFLOAT3> vertices;

	// XZ平面
	for (int i = 0; i < subdivisions; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			float theta = step * ((i + j) % subdivisions);

			DirectX::XMFLOAT3& p = vertices.emplace_back();
			p.x = sinf(theta) * radius;
			p.y = 0.0f;
			p.z = cosf(theta) * radius;
		}
	}

	// XY平面
	for (int i = 0; i < subdivisions; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			float theta = step * ((i + j) % subdivisions);

			DirectX::XMFLOAT3& p = vertices.emplace_back();
			p.x = sinf(theta) * radius;
			p.y = cosf(theta) * radius;
			p.z = 0.0f;
		}
	}
	// YZ平面
	for (int i = 0; i < subdivisions; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			float theta = step * ((i + j) % subdivisions);

			DirectX::XMFLOAT3& p = vertices.emplace_back();
			p.x = 0.0f;
			p.y = sinf(theta) * radius;
			p.z = cosf(theta) * radius;
		}
	}

	// メッシュ生成
	CreateMesh(device, vertices, sphereMesh);
}

// 描画実行
void Gizmos::Render(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シェーダー設定
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(), nullptr, 0);
	dc->IASetInputLayout(inputLayout.Get());

	// 定数バッファ設定
	dc->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	// レンダーステート設定
	const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Opaque), blendFactor, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));

	// ビュープロジェクション行列作成
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.camera->GetView());
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.camera->GetProjection());
	DirectX::XMMATRIX VP = V * P;

	// プリミティブ設定
	UINT stride = sizeof(DirectX::XMFLOAT3);
	UINT offest = 0;
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	for (const Instance& instance : instances)
	{
		// 頂点バッファ設定
		dc->IASetVertexBuffers(0, 1, instance.mesh->vertexBuffer.GetAddressOf(), &stride, &offest);
		// ワールドビュープロジェクション行列作成
		DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&instance.worldTransform);
		DirectX::XMMATRIX WVP = W * VP;

		// 定数バッファ更新
		CbMesh cbMesh;
		DirectX::XMStoreFloat4x4(&cbMesh.worldViewProjection, WVP);
		cbMesh.color = instance.color;

		dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &cbMesh, 0, 0);

		// 描画
		dc->Draw(instance.mesh->vertexCount, 0);
	}
	instances.clear();
}
#include "Plane.h"
#include "Misc.h"
#include "GpuResourceUtils.h"
#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Rendering/Shaders/PlaneShader.h"

// コンストラクタ
Plane::Plane(ID3D11Device* device, const char* filename, float scaling, std::array<DirectX::XMFLOAT3, 4> positions)
	: ModelObject(filename, scaling, ModelObject::RENDER_MODE::NOMODEL)
{
	HRESULT hr = S_OK;

	SetShader(ModelShaderId::Plane);

	// 頂点バッファの生成
	{
		std::vector<ModelResource::Vertex> vertices =
		{
			{
			DirectX::XMFLOAT3(positions[0].x,  positions[0].y, positions[0].z),
			DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMUINT4(0, 0, 0, 0),
			DirectX::XMFLOAT2(0.0f, 0.0f),
			DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
			DirectX::XMFLOAT3(0.0f,  0.0f, 0.0f), DirectX::XMFLOAT3(0.0f,  0.0f, 0.0f)},

			{
			DirectX::XMFLOAT3(positions[1].x,  positions[1].y, positions[1].z),
			DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMUINT4(0, 0, 0, 0),
			DirectX::XMFLOAT2(1.0f, 0.0f),
			DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
			DirectX::XMFLOAT3(0.0f,  0.0f, 0.0f), DirectX::XMFLOAT3(0.0f,  0.0f, 0.0f)},

			{
			DirectX::XMFLOAT3(positions[2].x, positions[2].y, positions[2].z),
			DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMUINT4(0, 0, 0, 0),
			DirectX::XMFLOAT2(0.0f, 1.0f),
			DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
			DirectX::XMFLOAT3(0.0f,  0.0f, 0.0f), DirectX::XMFLOAT3(0.0f,  0.0f, 0.0f)},

			{
			DirectX::XMFLOAT3(positions[3].x, positions[3].y, positions[3].z),
			DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMUINT4(0, 0, 0, 0),
			DirectX::XMFLOAT2(1.0f, 1.0f),
			DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
			DirectX::XMFLOAT3(0.0f,  0.0f, 0.0f), DirectX::XMFLOAT3(0.0f,  0.0f, 0.0f)},
		};

		mesh.vertices = vertices;

		// 頂点バッファを作成するための設定オプション
		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.ByteWidth = sizeof(ModelResource::Vertex) * 4; // 4頂点
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexData = {};
		vertexData.pSysMem = vertices.data();

		// 頂点バッファオブオブジェクトの生成
		hr = device->CreateBuffer(&buffer_desc, &vertexData, mesh.vertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	{
		unsigned int indices[] = {
			0, 1, 2,
			2, 1, 3
		};

		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(indices);
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA indexData = {};
		indexData.pSysMem = indices;

		hr = device->CreateBuffer(&indexBufferDesc, &indexData, mesh.indexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	mesh.material = new ModelResource::Material();

	// テクスチャ生成
	if (filename != "")
	{
		// テクスチャファイル読み込み
		D3D11_TEXTURE2D_DESC desc;
		hr = GpuResourceUtils::LoadTexture(
			device,
			filename,
			mesh.material->diffuseMap.GetAddressOf(),
			&desc
		);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		textureSize = {
			static_cast<float>(desc.Width),
			static_cast<float>(desc.Height)
		};
	}
	else
	{
		// ダミーテクスチャ生成
		D3D11_TEXTURE2D_DESC desc;
		hr = GpuResourceUtils::CreateDummyTexture(device, 0xFFFFFFFF, mesh.material->diffuseMap.GetAddressOf(), &desc);

		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		textureSize = {
			static_cast<float>(desc.Width),
			static_cast<float>(desc.Height)
		};
	}
}

void Plane::Update(float elapsedTime)
{
	// スケール行列生成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	// 回転行列生成
	DirectX::XMMATRIX R = AnglesToMatrix(angle);
	// 位置行列生成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	DirectX::XMMATRIX W = S * R * T;

	DirectX::XMStoreFloat4x4(&transform, W);
}

void Plane::Render(const RenderContext& rc)
{
	ModelShader* shader = T_GRAPHICS.GetModelShader(m_shaderId);
	shader->Begin(rc);
	shader->Draw(rc, mesh);
	shader->End(rc);
}
#include "Plane.h"
#include "Misc.h"
#include "GpuResourceUtils.h"
#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Rendering/Shaders/PlaneShader.h"

/**************************************************************************//**
	@brief		板のメッシュを作るコンストラクタ
	@param[in]    device	デバイス
	@param[in]    filename	テクスチャのファイルパス
	@param[in]    scaling	スケール
	@param[in]    positions	位置
*//***************************************************************************/
Plane::Plane(ID3D11Device* device, const char* filename, float scaling, XMFLOAT3 centerPos, float positionZ, float plane_width)
	: ModelObject(filename, scaling, ModelObject::RENDER_MODE::NOMODEL)
{
	HRESULT hr = S_OK;

	SetShader(ModelShaderId::Plane);

	// 頂点バッファの生成
	{
		std::vector<ModelResource::Vertex> vertices =
		{
			{
			DirectX::XMFLOAT3(centerPos.x + plane_width, centerPos.y - plane_width, positionZ),
			DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMUINT4(0, 0, 0, 0),
			DirectX::XMFLOAT2(0.0f, 0.0f),
			DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
			DirectX::XMFLOAT3(0.0f,  0.0f, 0.0f), DirectX::XMFLOAT3(0.0f,  0.0f, 0.0f)},

			{
			DirectX::XMFLOAT3(centerPos.x + plane_width, centerPos.y + plane_width, positionZ),
			DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMUINT4(0, 0, 0, 0),
			DirectX::XMFLOAT2(1.0f, 0.0f),
			DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
			DirectX::XMFLOAT3(0.0f,  0.0f, 0.0f), DirectX::XMFLOAT3(0.0f,  0.0f, 0.0f)},

			{
			DirectX::XMFLOAT3(centerPos.x - plane_width, centerPos.y - plane_width, positionZ),
			DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMUINT4(0, 0, 0, 0),
			DirectX::XMFLOAT2(0.0f, 1.0f),
			DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
			DirectX::XMFLOAT3(0.0f,  0.0f, 0.0f), DirectX::XMFLOAT3(0.0f,  0.0f, 0.0f)},

			{
			DirectX::XMFLOAT3(centerPos.x - plane_width, centerPos.y + plane_width, positionZ),
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

	mesh.material = new ModelResource::Material;

	// テクスチャ生成
	if (*filename != NULL)
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

// 旧Planeコンストラクタ
#if 0
Plane::Plane(ID3D11Device* device, const char* filename, float scaling, std::array<DirectX::XMFLOAT3, 4> positions)
	: ModelObject(filename, scaling, ModelObject::RENDER_MODE::NOMODEL)
{
	HRESULT hr = S_OK;

	SetShader(ModelShaderId::Plane);

	// 頂点バッファの生成
	{
		using namespace DirectX;

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

	mesh.material = new ModelResource::Material;

	// テクスチャ生成
	if (*filename != NULL)
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

#endif // 0

/**************************************************************************//**
	@brief		更新
	@param[in]    elapsedTime	アップデートタイマー
*//***************************************************************************/
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

/**************************************************************************//**
	@brief		レンダリング
	@param[in]    rc	レンダリングコンテクスト
*//***************************************************************************/
void Plane::Render(const RenderContext& rc)
{
	ModelShader* shader = T_GRAPHICS.GetModelShader(m_shaderId);
	shader->Begin(rc);
	shader->Draw(rc, mesh);
	shader->End(rc);
}

PlaneDX12::PlaneDX12(const char* filename, float scaling, XMFLOAT3 centerPos, float positionZ, float plane_width)
{
	HRESULT hr = S_OK;

	Graphics& graphics = Graphics::Instance();
	const RenderStateDX12* renderState = graphics.GetRenderStateDX12();
	ID3D12Device* d3d_device = graphics.GetDeviceDX12();

	m_dx12_ShaderId = static_cast<ModelShaderDX12Id>(ModelShaderDX12Id::Plane);

	// 頂点データー
	std::vector <ModelResource::Vertex> vertices = {
		{ { centerPos.x + plane_width, centerPos.y - plane_width, positionZ }, { 1.0f, 0.0f, 0.0f, 0.0f }, { 0, 0, 0, 0 }, { 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
		{ { centerPos.x + plane_width, centerPos.y + plane_width, positionZ }, { 1.0f, 0.0f, 0.0f, 0.0f }, { 0, 0, 0, 0 }, { 1.0f, 0.0f }, { 0.5f, 0.5f, 0.5f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
		{ { centerPos.x - plane_width, centerPos.y - plane_width, positionZ }, { 1.0f, 0.0f, 0.0f, 0.0f }, { 0, 0, 0, 0 }, { 0.0f, 1.0f }, { 0.5f, 0.5f, 0.5f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
		{ { centerPos.x - plane_width, centerPos.y + plane_width, positionZ }, { 1.0f, 0.0f, 0.0f, 0.0f }, { 0, 0, 0, 0 }, { 1.0f, 1.0f }, { 0.5f, 0.5f, 0.5f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }
	};

	// 頂点バッファの生成
	{
		const UINT vertexBufferSize = static_cast<UINT>(sizeof(ModelResource::Vertex) * vertices.size());

		// ヒーププロパティの設定
		D3D12_HEAP_PROPERTIES d3d_heap_props{};
		d3d_heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
		d3d_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		d3d_heap_props.CreationNodeMask = 1;
		d3d_heap_props.VisibleNodeMask = 1;

		// リソースの設定
		D3D12_RESOURCE_DESC d3d_resource_desc{};
		d3d_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		d3d_resource_desc.Alignment = 0;
		d3d_resource_desc.Width = vertexBufferSize;
		d3d_resource_desc.Height = 1;
		d3d_resource_desc.DepthOrArraySize = 1;
		d3d_resource_desc.MipLevels = 1;
		d3d_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
		d3d_resource_desc.SampleDesc.Count = 1;
		d3d_resource_desc.SampleDesc.Quality = 0;
		d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		d3d_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		// リソース生成
		hr = d3d_device->CreateCommittedResource(
			&d3d_heap_props,
			D3D12_HEAP_FLAG_NONE,
			&d3d_resource_desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(mesh.d3d_vb_resource.GetAddressOf())
		);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		mesh.d3d_vb_resource->SetName(L"PlaneVertexBuffer");

		// 頂点バッファビュー
		mesh.d3d_vbv.BufferLocation = mesh.d3d_vb_resource->GetGPUVirtualAddress();
		mesh.d3d_vbv.SizeInBytes = static_cast<UINT>(d3d_resource_desc.Width);
		mesh.d3d_vbv.StrideInBytes = sizeof(ModelResource::Vertex);

		// マップする

		void* mappedData = nullptr;
		hr = mesh.d3d_vb_resource->Map(0, nullptr, &mappedData);
		memcpy(mappedData, vertices.data(), vertices.size() * sizeof(ModelResource::Vertex));
		mesh.d3d_vb_resource->Unmap(0, nullptr);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// インデックスバッファの生成
	{
		UINT indices[] = {
			0, 1, 2,
			2, 1, 3
		};

		const UINT indexBufferSize = sizeof(indices);

		// ヒーププロパティの設定
		D3D12_HEAP_PROPERTIES d3d_heap_props{};
		d3d_heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
		d3d_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		d3d_heap_props.CreationNodeMask = 1;
		d3d_heap_props.VisibleNodeMask = 1;

		// リソースの設定
		D3D12_RESOURCE_DESC d3d_resource_desc{};
		d3d_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		d3d_resource_desc.Alignment = 0;
		d3d_resource_desc.Width = indexBufferSize;
		d3d_resource_desc.Height = 1;
		d3d_resource_desc.DepthOrArraySize = 1;
		d3d_resource_desc.MipLevels = 1;
		d3d_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
		d3d_resource_desc.SampleDesc.Count = 1;
		d3d_resource_desc.SampleDesc.Quality = 0;
		d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		d3d_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		// リソース生成
		hr = d3d_device->CreateCommittedResource(
			&d3d_heap_props,
			D3D12_HEAP_FLAG_NONE,
			&d3d_resource_desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(mesh.d3d_ib_resource.GetAddressOf())
		);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		mesh.d3d_ib_resource->SetName(L"PlaneIndexBuffer");

		// インデックスバッファビュー設定
		mesh.d3d_ibv.BufferLocation = mesh.d3d_ib_resource->GetGPUVirtualAddress();
		mesh.d3d_ibv.SizeInBytes = static_cast<UINT>(d3d_resource_desc.Width);
		mesh.d3d_ibv.Format = DXGI_FORMAT_R32_UINT;

		// インデックスデータ設定
		void* mappedData = nullptr;
		hr = mesh.d3d_ib_resource->Map(0, nullptr, &mappedData);
		memcpy(mappedData, &indices, 6 * sizeof(UINT));
		mesh.d3d_ib_resource->Unmap(0, nullptr);
	}

	mesh.material = new ModelResource::Material;

	// テクスチャの生成
	{
		if (filename != nullptr)
		{
			// テクスチャ読み込み
			hr = Graphics::Instance().LoadTexture(filename, mesh.material->d3d_srv_resource.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
		else
		{
			// ダミーテクスチャ生成
			hr = Graphics::Instance().CreateDummyTexture(mesh.material->d3d_srv_resource.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}

		// ディスクリプタ取得
		mesh.material->srv_descriptor = Graphics::Instance().GetShaderResourceDescriptorHeap()->PopDescriptor();

		// シェーダーリソースビューの生成
		D3D12_RESOURCE_DESC d3d_resource_desc = mesh.material->d3d_srv_resource->GetDesc();

		// シェーダーリソースビューの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC d3d_srv_desc = {};
		d3d_srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		d3d_srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		d3d_srv_desc.Format = d3d_resource_desc.Format;
		d3d_srv_desc.Texture2D.MipLevels = d3d_resource_desc.MipLevels;
		d3d_srv_desc.Texture2D.MostDetailedMip = 0;

		// シェーダリソースビューを生成
		d3d_device->CreateShaderResourceView(
			mesh.material->d3d_srv_resource.Get(),
			&d3d_srv_desc,
			mesh.material->srv_descriptor->GetCpuHandle()
		);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// テクスチャ情報の取得
		textureSize.x = static_cast<int>(d3d_resource_desc.Width);
		textureSize.y = static_cast<int>(d3d_resource_desc.Height);
	}
}

void PlaneDX12::RenderDX12(const RenderContextDX12& rc)
{
	ModelShaderDX12* shader = nullptr;

	// パイプライン設定
	shader = T_GRAPHICS.GetModelShaderDX12(m_dx12_ShaderId);

	m_Mesh.mesh = &mesh;
	//描画
	shader->Render(rc, m_Mesh);
}

void PlaneDX12::Update(float elapsedTime)
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

/**************************************************************************//**
	@brief		看板のメッシュを作るコンストラクタ
	@param[in]    device	デバイス
	@param[in]    filename	テクスチャのファイルパス
	@param[in]    scaling	スケール
	@param[in]    position	位置
*//***************************************************************************/
Billboard::Billboard(ID3D11Device* device, const char* filename, float scaling, DirectX::XMFLOAT3 position)
	: ModelObject(filename, scaling, ModelObject::RENDER_MODE::NOMODEL)
{
	SetShader(ModelShaderId::Billboard);

	HRESULT hr = S_OK;

	// 頂点バッファの生成
	{
		std::vector<ModelResource::Vertex> vertices =
		{
			{
			DirectX::XMFLOAT3(position.x,  position.y, position.z),
			DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMUINT4(0, 0, 0, 0),
			DirectX::XMFLOAT2(0.0f, 0.0f),
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
			0
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

	mesh.offsetTransforms.resize(1);

	mesh.material = new ModelResource::Material;

	// テクスチャ生成
	if (*filename != 0)
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

/**************************************************************************//**
	@brief		更新
	@param[in]    elapsedTime	アップデートタイマー
*//***************************************************************************/
void Billboard::Update(float elapsedTime)
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

/**************************************************************************//**
	@brief		レンダリング
	@param[in]    rc	レンダリングコンテクスト
*//***************************************************************************/
void Billboard::Render(const RenderContext& rc)
{
	//D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	//HRESULT hr = rc.deviceContext->Map(mesh.vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	//_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	//ModelResource::Vertex* v = static_cast<ModelResource::Vertex*>(mappedSubresource.pData);
	//for (int i = 0; i < mesh.vertices.size(); i++)
	//{
	//	v[i].position = mesh.vertices[i].position;
	//}

	//// 頂点バッファの内容の編集を終了する
	//rc.deviceContext->Unmap(mesh.vertexBuffer.Get(), 0);

	mesh.offsetTransforms[0] = transform;

	ModelShader* shader = T_GRAPHICS.GetModelShader(m_shaderId);
	shader->Begin(rc);
	shader->Draw(rc, mesh);
	shader->End(rc);
}

/**************************************************************************//**
	@brief		シェーダー設定
	@param[in]    device	デバイス
	@param[in]    filename	テクスチャのファイルパス
	@param[in]    scaling	スケール
	@param[in]    position	位置
*//***************************************************************************/
Fireball::Fireball(ID3D11Device* device, const char* filename, float scaling, DirectX::XMFLOAT3 position)
	: Billboard(device, filename, scaling, position)
{
	SetShader(ModelShaderId::Fireball);
}
/**************************************************************************//**
	@brief		シェーダー設定
	@param[in]    device	デバイス
	@param[in]    filename	テクスチャのファイルパス
	@param[in]    scaling	スケール
	@param[in]    position	位置
*//***************************************************************************/
RunningDust::RunningDust(ID3D11Device* device, const char* filename, float scaling, DirectX::XMFLOAT3 position, float alpha, int model_id, int age)
	: Billboard(device, filename, scaling, position)
{
	SetShader(ModelShaderId::Billboard);
}

void RunningDust::Update(float elapsedTime)
{
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();

	//mesh.vertices[0].position = player->GetPosition();

	// スケール行列生成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	// 回転行列生成
	DirectX::XMMATRIX R = AnglesToMatrix(angle);
	// 位置行列生成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(player->GetPosition().x, player->GetPosition().y, player->GetPosition().z);

	DirectX::XMMATRIX W = S * R * T;

	DirectX::XMStoreFloat4x4(&transform, W);
}
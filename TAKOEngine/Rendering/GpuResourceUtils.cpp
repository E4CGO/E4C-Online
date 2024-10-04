#include <filesystem>
#include <fstream>
#include <wrl.h>
#include "Misc.h"
#include "GpuResourceUtils.h"

static std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> cached_textures;

HRESULT GpuResourceUtils::LoadVertexShader(
	ID3D11Device* device,
	const char* filename,
	const D3D11_INPUT_ELEMENT_DESC inputElementDescs[],
	UINT inputElementCount,
	ID3D11InputLayout** inputLayout,
	ID3D11VertexShader** vertexShader
)
{
	FILE* fp = nullptr;
	fopen_s(&fp, filename, "rb");
	_ASSERT_EXPR_A(fp, "Vertex Shader File not found");

	// ファイルサイズを求める
	fseek(fp, 0, SEEK_END);
	long  size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// メモリ上に頂点シェーダーデータを格納する領域を用意する
	std::unique_ptr<u_char[]> data = std::make_unique<u_char[]>(size);
	fread(data.get(), size, 1, fp);
	fclose(fp);

	// 頂点シェーダー生成
	HRESULT hr = device->CreateVertexShader(data.get(), size, nullptr, vertexShader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// 入力レイアウト
	if (inputLayout != nullptr)
	{
		hr = device->CreateInputLayout(inputElementDescs, inputElementCount, data.get(), size, inputLayout);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	return hr;
}

// ピクセルシェーダー読み込む
HRESULT GpuResourceUtils::LoadPixelShader(
	ID3D11Device* device,
	const char* filename,
	ID3D11PixelShader** pixelShader
)
{
	// ファイルを開く
	FILE* fp = nullptr;
	fopen_s(&fp, filename, "rb");
	_ASSERT_EXPR_A(fp, "Pixel Shader File not found");

	// ファイルサイズを求める
	fseek(fp, 0, SEEK_END);
	long  size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// メモリ上に頂点シェーダーデータを格納する領域を用意する
	std::unique_ptr<u_char[]> data = std::make_unique<u_char[]>(size);
	fread(data.get(), size, 1, fp);
	fclose(fp);

	// ピクセルシェーダー生成
	HRESULT hr = device->CreatePixelShader(data.get(), size, nullptr, pixelShader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return hr;
}

//ジオメトリシェーダー
HRESULT GpuResourceUtils::LoadGeometryShader(
	ID3D11Device* device,
	const char* filename,
	ID3D11GeometryShader** geometryShader)
{
	//ファイルを開く
	FILE* fp = nullptr;
	fopen_s(&fp, filename, "rb");
	_ASSERT_EXPR_A(fp, "Geometry Shader File not found");

	//ファイルのサイズを求める
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	//メモリ上に頂点シェーダーデータを格納する領域を用意する
	std::unique_ptr<u_char[]> data = std::make_unique<u_char[]>(size);
	fread(data.get(), size, 1, fp);
	fclose(fp);

	//ジオメトリシェーダー生成
	HRESULT hr = device->CreateGeometryShader(data.get(), size, nullptr, geometryShader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return hr;
}

//ハルシェーダー
HRESULT GpuResourceUtils::LoadHullShader(
	ID3D11Device* device,
	const char* filename,
	ID3D11HullShader** hullShader)
{
	//ファイルを開く
	FILE* fp = nullptr;
	fopen_s(&fp, filename, "rb");
	_ASSERT_EXPR_A(fp, "hull Shader File not found");

	//ファイルのサイズを求める
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	//メモリ上に頂点シェーダーデータを格納する領域を用意する
	std::unique_ptr<u_char[]> data = std::make_unique<u_char[]>(size);
	fread(data.get(), size, 1, fp);
	fclose(fp);

	//ハルシェーダー生成
	HRESULT hr = device->CreateHullShader(data.get(), size, nullptr, hullShader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return hr;
}

//ドメインシェーダー
HRESULT GpuResourceUtils::LoadDomainShader(
	ID3D11Device* device,
	const char* filename,
	ID3D11DomainShader** domainShader)
{
	//ファイルを開く
	FILE* fp = nullptr;
	fopen_s(&fp, filename, "rb");
	_ASSERT_EXPR_A(fp, "domain　Shader File not found");

	//ファイルのサイズを求める
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	//メモリ上に頂点シェーダーデータを格納する領域を用意する
	std::unique_ptr<u_char[]> data = std::make_unique<u_char[]>(size);
	fread(data.get(), size, 1, fp);
	fclose(fp);

	//ドメインシェーダー生成
	HRESULT hr = device->CreateDomainShader(data.get(), size, nullptr, domainShader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return hr;
}

//コンピュートシェーダー
HRESULT GpuResourceUtils::LoadComputeShader(
	ID3D11Device* device,
	const char* filename,
	ID3D11ComputeShader** computeShader)
{
	//ファイルを開く
	FILE* fp = nullptr;
	fopen_s(&fp, filename, "rb");
	_ASSERT_EXPR_A(fp, "domain　Shader File not found");

	//ファイルのサイズを求める
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	//メモリ上に頂点シェーダーデータを格納する領域を用意する
	std::unique_ptr<u_char[]> data = std::make_unique<u_char[]>(size);
	fread(data.get(), size, 1, fp);
	fclose(fp);

	//コンピュートシェーダー生成
	HRESULT hr = device->CreateComputeShader(data.get(), size, nullptr, computeShader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return hr;
}

void GpuResourceUtils::LoadShaderFile(const char* filename, std::vector<BYTE>& data)
{
	// ファイルを開く
	FILE* fp = nullptr;
	fopen_s(&fp, filename, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	// ファイルのサイズを求める
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// メモリ上に頂点シェーダーデータを格納する領域を用意する
	data.resize(size);
	fread(data.data(), size, 1, fp);
	fclose(fp);
}

// テクスチャを読み込む
HRESULT GpuResourceUtils::LoadTexture(
	ID3D11Device* device,
	const char* filename,
	ID3D11ShaderResourceView** shaderResourceView,
	D3D11_TEXTURE2D_DESC* texture2dDesc
)
{
	// 拡張子を取得
	std::filesystem::path filepath(filename);
	std::string extension = filepath.extension().string();
	std::transform(extension.begin(), extension.end(), extension.begin(), tolower); // 小文字化

	// ワイド文字に変換
	std::wstring wfilename = filepath.wstring();

	// フォーマット毎に画像読み込み処理
	HRESULT hr;

	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratch_image;

	if (extension == ".tga")
	{
		hr = DirectX::GetMetadataFromTGAFile(wfilename.c_str(), metadata);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		hr = DirectX::LoadFromTGAFile(wfilename.c_str(), &metadata, scratch_image);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	else if (extension == ".dds")
	{
		hr = DirectX::GetMetadataFromDDSFile(wfilename.c_str(), DirectX::DDS_FLAGS_NONE, metadata);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		hr = DirectX::LoadFromDDSFile(wfilename.c_str(), DirectX::DDS_FLAGS_NONE, &metadata, scratch_image);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	else if (extension == ".hdr")
	{
		hr = DirectX::GetMetadataFromHDRFile(wfilename.c_str(), metadata);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		hr = DirectX::LoadFromHDRFile(wfilename.c_str(), &metadata, scratch_image);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	else // WIC
	{
		hr = DirectX::GetMetadataFromWICFile(wfilename.c_str(), DirectX::WIC_FLAGS_NONE, metadata);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		hr = DirectX::LoadFromWICFile(wfilename.c_str(), DirectX::WIC_FLAGS_NONE, &metadata, scratch_image);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	if (metadata.mipLevels == 1)
	{
		DirectX::ScratchImage scratch_image_mip;
		hr = DirectX::GenerateMipMaps(scratch_image.GetImages(), scratch_image.GetImageCount(), scratch_image.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, scratch_image_mip);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		scratch_image = std::move(scratch_image_mip);
	}

	// シェーダーリソースビュー作成
	hr = DirectX::CreateShaderResourceView(device, scratch_image.GetImages(), scratch_image.GetImageCount(), metadata, shaderResourceView);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	if (texture2dDesc != nullptr)
	{
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		(*shaderResourceView)->GetResource(resource.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		hr = resource->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		texture2d->GetDesc(texture2dDesc);
	}

	return hr;
}

HRESULT GpuResourceUtils::load_texture_from_memory(ID3D11Device* device, const void* data, size_t size, ID3D11ShaderResourceView** shader_resource_view, bool generate_mips, size_t mip_levels)
{
	HRESULT hr{ S_OK };
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	hr = DirectX::CreateDDSTextureFromMemory(device, reinterpret_cast<const uint8_t*>(data), size, resource.GetAddressOf(), shader_resource_view);
	if (hr != S_OK)
	{
		hr = DirectX::CreateWICTextureFromMemory(device, reinterpret_cast<const uint8_t*>(data), size, resource.GetAddressOf(), shader_resource_view);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	return hr;
}

HRESULT GpuResourceUtils::load_texture_from_file(ID3D11Device* device, const wchar_t* filename, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc)
{
	HRESULT hr{ S_OK };
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;

	auto it = resources.find(filename);
	if (it != resources.end())
	{
		*shader_resource_view = it->second.Get();
		(*shader_resource_view)->AddRef();
		(*shader_resource_view)->GetResource(resource.GetAddressOf());
	}
	else
	{
		hr = DirectX::CreateDDSTextureFromFile(device, filename, resource.GetAddressOf(), shader_resource_view);
		if (hr != S_OK)
		{
			hr = DirectX::CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shader_resource_view);
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			resources.insert(std::make_pair(filename, *shader_resource_view));
		}
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
	texture2d->GetDesc(texture2d_desc);

	return hr;
}

HRESULT GpuResourceUtils::CreateDummyTexture(
	ID3D11Device* device,
	UINT color,
	ID3D11ShaderResourceView** shaderResourceView,
	D3D11_TEXTURE2D_DESC* texture2dDesc
)
{
	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = 1;
	desc.Height = 1;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA data{};
	data.pSysMem = &color;
	data.SysMemPitch = desc.Width;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	HRESULT hr = device->CreateTexture2D(&desc, &data, texture.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	hr = device->CreateShaderResourceView(texture.Get(), nullptr, shaderResourceView);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// テクスチャ情報取得
	if (texture2dDesc != nullptr)
	{
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		(*shaderResourceView)->GetResource(resource.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		hr = resource->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
		texture2d->GetDesc(texture2dDesc);
	}

	return hr;
}

// 定数バッファ作成
HRESULT GpuResourceUtils::CreateConstantBuffer(
	ID3D11Device* device,
	UINT bufferSize,
	ID3D11Buffer** constantBuffer
)
{
	D3D11_BUFFER_DESC desc{};
	::memset(&desc, 0, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.ByteWidth = bufferSize;
	desc.StructureByteStride = 0;

	HRESULT hr = device->CreateBuffer(&desc, 0, constantBuffer);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return hr;
}

//HRESULT GpuResourceUtils::GetMetadataFromGLBFile(const wchar_t* szFile, DirectX::TexMetadata& metadata)
//{
//	HRESULT hr = S_OK;
//
//	// GLBファイルを開く
//	std::ifstream file(szFile, std::ios::binary | std::ios::ate);
//	if (!file.is_open())
//	{
//		return E_FAIL; // ファイルのオープンに失敗した場合はエラーを返す
//	}
//
//	// ファイルサイズを取得
//	std::streamsize fileSize = file.tellg();
//	file.seekg(0, std::ios::beg);
//
//	// ファイルデータを格納するバッファを作成
//	std::vector<uint8_t> buffer(fileSize);
//	if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize))
//	{
//		return E_FAIL; // ファイルの読み込みに失敗した場合はエラーを返す
//	}
//
//	// DirectX::TexMetadataを使用してGLBファイルのメタデータを取得
//	//hr = DirectX::GetMetadataFromGltf(reinterpret_cast<const uint8_t*>(buffer.data()), buffer.size(), DirectX::DDS_FLAGS_NONE, metadata);
//	//if (FAILED(hr))
//	//{
//	//	return hr; // メタデータの取得に失敗した場合はエラーコードを返す
//	//}
//
//	return S_OK;
//}
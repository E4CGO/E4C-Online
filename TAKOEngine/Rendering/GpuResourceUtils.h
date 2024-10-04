#pragma once

#include <map>
#include <wrl.h>
#include <string>
#include <d3d11.h>
#include "DirectXTex.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

// GPUリソースユーティリティ
class GpuResourceUtils
{
public:
	// 頂点シェーダー読み込み
	static HRESULT LoadVertexShader(
		ID3D11Device* device,
		const char* filename,
		const D3D11_INPUT_ELEMENT_DESC inputElementDescs[],
		UINT inputElementCount,
		ID3D11InputLayout** inputLayout,
		ID3D11VertexShader** vertexShader
	);

	// ピクセルシェーダー読み込み
	static HRESULT LoadPixelShader(
		ID3D11Device* device,
		const char* filename,
		ID3D11PixelShader** pixelShader
	);

	//ジオメトリシェーダー
	static HRESULT LoadGeometryShader(
		ID3D11Device* device,
		const char* filename,
		ID3D11GeometryShader** geometryShader);

	//ハルシェーダー
	static HRESULT LoadHullShader(
		ID3D11Device* device,
		const char* filename,
		ID3D11HullShader** hullShader);

	//ドメインシェーダー
	static HRESULT LoadDomainShader(
		ID3D11Device* device,
		const char* filename,
		ID3D11DomainShader** domainShader);

	//コンピュートシェーダー
	static HRESULT LoadComputeShader(
		ID3D11Device* device,
		const char* filename,
		ID3D11ComputeShader** computeShader);

	//シェーダー読み込み
	static void LoadShaderFile(const char* filename, std::vector<BYTE>& data);

	// テクスチャ読み込み
	static HRESULT LoadTexture(
		ID3D11Device* device,
		const char* filename,
		ID3D11ShaderResourceView** shaderResourceView,
		D3D11_TEXTURE2D_DESC* texture2dDesc = nullptr
	);

	static HRESULT load_texture_from_memory(ID3D11Device* device,
		const void* data,
		size_t size,
		ID3D11ShaderResourceView** shader_resource_view,
		bool generate_mips,
		size_t mip_levels
	);

	static HRESULT load_texture_from_file(ID3D11Device* device,
		const wchar_t* filename,
		ID3D11ShaderResourceView** shader_resource_view,
		D3D11_TEXTURE2D_DESC* texture2d_desc
	);

	// ダミーテクスチャ作成
	static HRESULT CreateDummyTexture(
		ID3D11Device* device,
		UINT color,
		ID3D11ShaderResourceView** shaderResourceView,
		D3D11_TEXTURE2D_DESC* texture2dDesc = nullptr
	);

	// 定数バッファ作成
	static HRESULT CreateConstantBuffer(
		ID3D11Device* device,
		UINT bufferSize,
		ID3D11Buffer** constantBuffer
	);

private:
	static HRESULT GetMetadataFromGLBFile(const wchar_t* szFile, DirectX::TexMetadata& metadata);
};

static std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> resources;
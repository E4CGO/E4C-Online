#pragma once

#include <d3d11.h>
#include "DirectXTex.h"

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

	// テクスチャ読み込み
	static HRESULT LoadTexture(
		ID3D11Device* device,
		const char* filename,
		ID3D11ShaderResourceView** shaderResourceView,
		D3D11_TEXTURE2D_DESC* texture2dDesc = nullptr
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

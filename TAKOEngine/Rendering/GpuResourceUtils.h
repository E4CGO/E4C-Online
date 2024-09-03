#pragma once

#include <map>
#include <wrl.h>
#include <string>
#include <d3d11.h>
#include "DirectXTex.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

// GPU���\�[�X���[�e�B���e�B
class GpuResourceUtils
{
public:
	// ���_�V�F�[�_�[�ǂݍ���
	static HRESULT LoadVertexShader(
		ID3D11Device* device,
		const char* filename,
		const D3D11_INPUT_ELEMENT_DESC inputElementDescs[],
		UINT inputElementCount,
		ID3D11InputLayout** inputLayout,
		ID3D11VertexShader** vertexShader
	);

	// �s�N�Z���V�F�[�_�[�ǂݍ���
	static HRESULT LoadPixelShader(
		ID3D11Device* device,
		const char* filename,
		ID3D11PixelShader** pixelShader
	);

	//�W�I���g���V�F�[�_�[
	static HRESULT LoadGeometryShader(
		ID3D11Device* device,
		const char* filename,
		ID3D11GeometryShader** geometryShader);

	//�n���V�F�[�_�[
	static HRESULT LoadHullShader(
		ID3D11Device* device,
		const char* filename,
		ID3D11HullShader** hullShader);

	//�h���C���V�F�[�_�[
	static HRESULT LoadDomainShader(
		ID3D11Device* device,
		const char* filename,
		ID3D11DomainShader** domainShader);

	//�R���s���[�g�V�F�[�_�[
	static HRESULT LoadComputeShader(
		ID3D11Device* device,
		const char* filename,
		ID3D11ComputeShader** computeShader);

	// �e�N�X�`���ǂݍ���
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

	// �_�~�[�e�N�X�`���쐬
	static HRESULT CreateDummyTexture(
		ID3D11Device* device,
		UINT color,
		ID3D11ShaderResourceView** shaderResourceView,
		D3D11_TEXTURE2D_DESC* texture2dDesc = nullptr
	);

	// �萔�o�b�t�@�쐬
	static HRESULT CreateConstantBuffer(
		ID3D11Device* device,
		UINT bufferSize,
		ID3D11Buffer** constantBuffer
	);

private:
	static HRESULT GetMetadataFromGLBFile(const wchar_t* szFile, DirectX::TexMetadata& metadata);
};

static std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> resources;
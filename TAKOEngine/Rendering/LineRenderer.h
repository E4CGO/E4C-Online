//! @file LineRenderer.h
//! @note

#ifndef __GRAHICS_LINE_RENDERER_H__
#define __GRAHICS_LINE_RENDERER_H__

#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <d3d12.h>
#include <DirectXMath.h>

#include "RenderState.h"

//***************************************************
// @class LineRenderer
// @brief 
// @par   [説明]
//***************************************************
class LineRenderer
{
public:
	LineRenderer(ID3D11Device* device, UINT vertexCount);
	~LineRenderer() {}

public:
	// 描画実行
	void Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

	// 頂点追加
	void AddVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT2& texcoord);

private:
	struct ConstantBuffer
	{
		DirectX::XMFLOAT4X4	wvp;
	};

	struct Vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT2	texcoord;
	};


	Microsoft::WRL::ComPtr<ID3D11Buffer>			vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			constantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;

	std::vector<Vertex>			vertices;
	UINT						capacity = 0;

	D3D11_TEXTURE2D_DESC textureDesc = {};
};

//***************************************************
// @class LineRendererDX12
// @brief 
// @par   [説明]
//***************************************************
class LineRendererDX12
{
public:
	LineRendererDX12(ID3D12Device* device, UINT vertexCount);
	~LineRendererDX12();

	void Render(ID3D12GraphicsCommandList* d3d_command_list, SpriteDX12* sprite);

	// 頂点追加
	void AddVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT2& texcoord);

	void Finalize();

private:
	void CreateConstantBuffer();

	const Descriptor* UpdateSceneConstantBuffer();

private:
	struct CBScene
	{
		DirectX::XMFLOAT4X4	wvp;
	};

	struct Vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT2	texcoord;
	};
	std::vector<Vertex>			vertices;
	UINT						capacity = 0;

	Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_cbv_resource;
	const Descriptor* cbv_descriptor = nullptr;
	CBScene* cbv_data = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_vb_resource;
	D3D12_VERTEX_BUFFER_VIEW				d3d_vbv = {};

	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;
};

#endif // !__GRAHICS_LINE_RENDERER_H__
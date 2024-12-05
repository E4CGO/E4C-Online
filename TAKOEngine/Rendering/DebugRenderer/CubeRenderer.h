//! @file CubeRenderer.h
//! @note

#ifndef __GRAHICS_CUBE_RENDERER_H__
#define __GRAHICS_CUBE_RENDERER_H__

#include <vector>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>

#include "TAKOEngine\Rendering\RenderContext.h"
#include "TAKOEngine\Rendering\Descriptor.h"

//*********************************************************
// @file   CubeRenderer
// @brief  立方体デバック描画(DX12)
// @par    
//*********************************************************
class CubeRenderer
{
public:
	CubeRenderer(ID3D12Device* device, UINT cube_count = 1);
	~CubeRenderer();

	// 描画
	void Render(const RenderContextDX12& rc);

	// 立方体情報セット
	void SetCube(const DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, const DirectX::XMFLOAT4 color)
	{
		m_index = 0;

		m_position = position;
		m_scale = scale;
		m_color = color;
	}

private:
	// フレームリソース生成
	void CreateFrameResource();

	// 立方体メッシュ作成
	void CreateCubeMesh();

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;

	struct Vertex
	{
		DirectX::XMFLOAT3	position;
	};

	struct CbMesh
	{
		DirectX::XMFLOAT4X4	worldViewProjection;
		DirectX::XMFLOAT4	color;
	};

	struct FrameResource
	{
		Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_vb_resource;
		D3D12_VERTEX_BUFFER_VIEW				d3d_vbv = {};
		Vertex* vertex_data = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_cbv_resource;
		const Descriptor* cbv_descriptor = nullptr;
		CbMesh* cb_mesh_data = nullptr;
	};
	std::vector<FrameResource>	m_frame_resources;
	int	m_index = 0;
	int m_count = 0;

	UINT	vertexCount = 0;

	DirectX::XMFLOAT3 m_position = { 0,0,0 };
	DirectX::XMFLOAT3 m_scale = { 1,1,1 };
	DirectX::XMFLOAT4 m_color = { 1,1,1,1 };
};

#endif // !__GRAHICS_CUBE_RENDERER_H__
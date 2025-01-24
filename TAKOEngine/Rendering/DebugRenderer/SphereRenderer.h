//! @flie SphereRenderer.h
//! @note 

#ifndef __GRAHICS_SPHERE_RENDERER_H__
#define __GRAHICS_SPHERE_RENDERER_H__

#include <vector>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>

#include "TAKOEngine\Rendering\RenderContext.h"
#include "TAKOEngine\Rendering\Descriptor.h"

//*********************************************************
// @file   SphereRenderer
// @brief  球デバック描画(DX12)
// @par    
//*********************************************************
class SphereRenderer
{
public:
	SphereRenderer(ID3D12Device* device, UINT sphere_count = 1);
	~SphereRenderer();

	// 描画
	void Render(const RenderContextDX12& rc);

	void SetSphere(const DirectX::XMFLOAT3 center, float radius, const DirectX::XMFLOAT4 color)
	{
		m_index = 0;

		m_center = center;
		m_radius = radius;
		m_color = color;
	}

private:
	// コンスタントバッファ作成
	void CreateConstantBuffer(ID3D12Device* device);

	// フレームリソース生成
	void CreateFrameResource(ID3D12Device* device, float radius, int slices, int stacks);

	// 球メッシュ作成
	void CreateSphereMesh(float radius, int slices, int stacks);

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

	DirectX::XMFLOAT3 m_center = { 0,0,0 };
	float m_radius = 0;
	DirectX::XMFLOAT4 m_color = { 1,1,1,1 };
};

#endif // !__GRAHICS_SPHERE_RENDERER_H__
//! @file CylinderRenderer.h
//! @note
 
#ifndef __GRAHICS_CYLINDER_RENDERER_H__
#define __GRAHICS_CYLINDER_RENDERER_H__


#include <vector>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>

#include "TAKOEngine\Rendering\RenderContext.h"
#include "TAKOEngine\Rendering\Descriptor.h"

//*********************************************************
// @file   CylinderRenderer
// @brief  円柱デバック描画(DX12)
// @par    
//*********************************************************
class CylinderRenderer
{
public:
	CylinderRenderer(ID3D12Device* device, UINT cylinder_count = 1);
	~CylinderRenderer();

	// 描画
	void Render(const RenderContextDX12& rc);

	// 円柱情報セット
	void SetCylinder(const DirectX::XMFLOAT3 position, const DirectX::XMFLOAT3 direction, float radius, float height, const DirectX::XMFLOAT4 color)
	{
		m_index = 0;

		m_position = position;
		m_direction = direction;
		m_radius = radius;
		m_height = height;
		m_color = color;
	}

private:
	// フレームリソース生成
	void CreateFrameResource(ID3D12Device* device, float radius1, float radius2, float start, float height, int slices, int stacks);

	// 円柱メッシュ作成
	void CreateCylinderMesh(float radius1, float radius2, float start, float height, int slices, int stacks);

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
	DirectX::XMFLOAT3 m_direction = { 0,0,0 };
	float m_radius = 0;
	float m_height = 0;
	DirectX::XMFLOAT4 m_color = { 1,1,1,1 };
};

#endif // !__GRAHICS_CYLINDER_RENDERER_H__
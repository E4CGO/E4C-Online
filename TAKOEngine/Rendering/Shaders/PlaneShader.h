//! @file PlaneShader.h
//! @note

#ifndef __INCLUDED_PLANE_SHADER_H__
#define __INCLUDED_PLANE_SHADER_H__

#include "TAKOEngine/Rendering/Plane.h"
#include "TAKOEngine/Rendering/Shaders/ModelShader.h"

/**************************************************************************//**
	@class	PlaneShader
	@brief　板シェーダークラス
	@par    [説明]
		板描画管理
*//***************************************************************************/
class PlaneShader : public ModelShader
{
public:
	PlaneShader(ID3D11Device* device) : ModelShader(device, "Data/Shader/PlaneVS.cso", "Data/Shader/PlanePS.cso") {}
	PlaneShader(ID3D11Device* device, const char* vs, const char* ps) : ModelShader(device, vs, ps) {}
	virtual ~PlaneShader() override = default;

	// 描画開始
	void Begin(const RenderContext& rc) override;

	// モデル描画
	void Draw(const RenderContext& rc, const ModelResource::Mesh& mesh) override;

	void End(const RenderContext& rc) override;

protected:

	// レンダーステート設定
	void SetRenderState(const RenderContext& rc) override;

	// シェーダーリソースビュー設定
	void SetShaderResourceView(const ModelResource::Mesh& mesh, ID3D11DeviceContext*& dc) override;
};

/**************************************************************************//**
	@class	PortalShader
	@brief	テレポートシェーダークラス
	@par    [説明]
		テレポート画管理
*//***************************************************************************/
class PortalShader : public PlaneShader
{
public:
	PortalShader(ID3D11Device* device) : PlaneShader(device, "Data/Shader/PortalVS.cso", "Data/Shader/PortalPS.cso") {}
	virtual ~PortalShader() override = default;
};
/**************************************************************************//**
	@class	PortalShader
	@brief	テレポートシェーダークラス
	@par    [説明]
		四角く区切って描画するバージョン
*//***************************************************************************/
class PortalSquareShader : public PlaneShader
{
public:
	PortalSquareShader(ID3D11Device* device) : PlaneShader(device, "Data/Shader/PortalVS.cso", "Data/Shader/PortalSquarePS.cso") {}
	virtual ~PortalSquareShader() override = default;
};

/**************************************************************************//**
	@class	BillboardShader
	@brief	看板シェーダークラス
	@par    [説明]
		看板描画管理
*//***************************************************************************/
class BillboardShader : public ModelShader
{
public:
	BillboardShader(ID3D11Device* device) : ModelShader(device, "Data/Shader/BillBoardVS.cso", "Data/Shader/BillBoardPS.cso")
	{
		GpuResourceUtils::CreateConstantBuffer(
			device,
			sizeof(DirectX::XMFLOAT4X4),
			m_WorldMatrixBuffer.GetAddressOf());

		GpuResourceUtils::LoadGeometryShader(
			device,
			GeometryShaderName,
			geometryShader.GetAddressOf());
	}

	BillboardShader(ID3D11Device* device, const char* vs, const char* ps) : ModelShader(device, vs, ps)
	{
		GpuResourceUtils::CreateConstantBuffer(
			device,
			sizeof(DirectX::XMFLOAT4X4),
			m_WorldMatrixBuffer.GetAddressOf());

		GpuResourceUtils::LoadGeometryShader(
			device,
			GeometryShaderName,
			geometryShader.GetAddressOf());
	}
	virtual ~BillboardShader() override = default;

	// 描画開始
	void Begin(const RenderContext& rc) override;

	// モデル描画
	void Draw(const RenderContext& rc, const ModelResource::Mesh& mesh) override;

	void End(const RenderContext& rc) override;

protected:

	const char* GeometryShaderName = "Data/Shader/BillboardGS.cso";

	// レンダーステート設定
	void SetRenderState(const RenderContext& rc) override;

	// シェーダーリソースビュー設定
	void SetShaderResourceView(const ModelResource::Mesh& mesh, ID3D11DeviceContext*& dc) override;

	Microsoft::WRL::ComPtr<ID3D11GeometryShader>		geometryShader;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_WorldMatrixBuffer;
};

/**************************************************************************//**
	@class	FireballShader
	@brief	火球シェーダークラス
	@par    [説明]
		火球描画管理
*//***************************************************************************/
class FireballShader : public BillboardShader
{
public:
	FireballShader(ID3D11Device* device) : BillboardShader(device, "Data/Shader/BillboardVS.cso", "Data/Shader/FireballPS.cso") {}
};

/**************************************************************************//**
	@class	PlaneShaderDX12
	@brief　板シェーダークラス
	@par    [説明]
		板描画管理
*//***************************************************************************/
class PlaneShaderDX12 : public ModelShaderDX12
{
public:
	PlaneShaderDX12(ID3D12Device* device, const char* vertexShaderName = "Data/Shader/PlaneDX12VS.cso", const char* pixelShaderName = "Data/Shader/PlaneDX12PS.cso");
	virtual ~PlaneShaderDX12() override = default;

	void Finalize() override {};

	void Render(const RenderContextDX12& rc, const ModelDX12::Mesh& mesh) override;

protected:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;

	SamplerManager* m_sampler = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_cbv_resource;
	const Descriptor* cbv_descriptor = nullptr;
	DirectX::XMFLOAT4X4 worldmatrix = DirectX::XMFLOAT4X4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
};

/**************************************************************************//**
	@class	PortalSquareShaderDX12
	@brief	テレポートシェーダークラス
	@par    [説明]
		四角く区切って描画するバージョン
*//***************************************************************************/
class PortalSquareShaderDX12 : public PlaneShaderDX12
{
public:
	PortalSquareShaderDX12(ID3D12Device* device) : PlaneShaderDX12(device, "Data/Shader/PlaneDX12VS.cso", "Data/Shader/PortalSquareDX12PS.cso") {}
	virtual ~PortalSquareShaderDX12() override = default;
};

/**************************************************************************//**
	@class	LoadingShaderDX12
	@brief	画廊切り替え効果クラス
	@par    [説明]
		四角く区切って描画するバージョン
*//***************************************************************************/
class LoadingShaderDX12 : public PlaneShaderDX12
{
public:
	LoadingShaderDX12(ID3D12Device* device) : PlaneShaderDX12(device, "Data/Shader/PlaneSpriteDX12VS.cso", "Data/Shader/LoadingDX12PS.cso") {}
	virtual ~LoadingShaderDX12() override = default;
};

/**************************************************************************//**
	@class	BillboardShaderDX12
	@brief	看板シェーダークラス
	@par    [説明]
		看板描画管理
*//***************************************************************************/
class BillBoardShaderDX12 : public ModelShaderDX12
{
public:
	BillBoardShaderDX12(ID3D12Device* device, const char* vertexShaderName = "Data/Shader/BillboardDX12VS.cso",
		const char* pixelShaderName = "Data/Shader/BillboardDX12PS.cso",
		const char* geometryShaderName = "Data/Shader/BillboardDX12GS.cso");
	virtual ~BillBoardShaderDX12() override = default;

	void Finalize() override {};

	void Render(const RenderContextDX12& rc, const ModelDX12::Mesh& mesh) override;

protected:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;

	SamplerManager* m_sampler = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_cbv_resource;
	const Descriptor* cbv_descriptor = nullptr;
	DirectX::XMFLOAT4X4 worldmatrix = DirectX::XMFLOAT4X4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
};

/**************************************************************************//**
	@class	FireballShaderDX12
	@brief	火球シェーダークラス
	@par    [説明]
		火球描画管理
*//***************************************************************************/
class FireballShaderDX12 : public BillBoardShaderDX12
{
public:
	FireballShaderDX12(ID3D12Device* device) : BillBoardShaderDX12(device, "Data/Shader/BillboardDX12VS.cso",
		"Data/Shader/FireballDX12PS.cso",
		"Data/Shader/BillboardDX12GS.cso") {
	};
	virtual ~FireballShaderDX12() override = default;
};

#endif //!__INCLUDED_PLANE_SHADER_H__
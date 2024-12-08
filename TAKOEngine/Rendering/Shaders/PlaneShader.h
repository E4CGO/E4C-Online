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

#endif //!__INCLUDED_PLANE_SHADER_H__
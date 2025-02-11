﻿//! @file CylinderShader.h
//! @note

#ifndef __INCLUDED_CYLINDER_SHADER_H__
#define __INCLUDED_CYLINDER_SHADER_H__

#include "TAKOEngine/Rendering/Cylinder.h"
#include "TAKOEngine/Rendering/Shaders/ModelShader.h"

/**************************************************************************//**
	@class    CylinderShader
	@brief	DX11で円錐作成
	@par    [説明]
	円錐を作成、描画するクラス
*//***************************************************************************/
class CylinderShader : public ModelShader
{
public:
	CylinderShader(ID3D11Device* device, const char* vs, const char* ps);
	~CylinderShader() override = default;

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

//*******************************************************
// @class ZoneShaderDX12
// @brief ZoneShaderDX12のパラーメータ設定クラス
// @par   [説明]
// @par   DX12に対応したzone描画
//*******************************************************
class ZoneShaderDX12 : public ModelShaderDX12
{
public:
	ZoneShaderDX12(ID3D12Device* device, const char* vertexShaderName = "Data/Shader/PrimitiveDX12VS.cso", const char* pixelShaderName = "Data/Shader/ZoneDX12PS.cso");
	~ZoneShaderDX12() override = default;

	void Finalize() override {};

	void Render(const RenderContextDX12& rc, const ModelDX12::Mesh& mesh) override;

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state_front;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state_back;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;

	SamplerManager* m_sampler = nullptr;
};

/**************************************************************************//**
	@class    ElectricShaderDX12
	@brief	ElectricShaderDX12のパラーメータ設定クラス
	@par    [説明]
		ボスの電気効果を描画する
*//***************************************************************************/
class ElectricShaderDX12 : public ZoneShaderDX12
{
public:
	ElectricShaderDX12(ID3D12Device* device) : ZoneShaderDX12(device, "Data/Shader/PrimitiveDX12VS.cso", "Data/Shader/ElectricDX12PS.cso") {}

	virtual ~ElectricShaderDX12() override = default;
};

/**************************************************************************//**
	@class    ChargeShaderdDX12
	@brief    ElectricShaderDX12のパラーメータ設定クラス
	@par    [説明]
		魔法使いチャージ効果を描画する
*//***************************************************************************/
class ChargeShaderdDX12 : public ZoneShaderDX12
{
public:
	ChargeShaderdDX12(ID3D12Device* device) : ZoneShaderDX12(device, "Data/Shader/PrimitiveDX12VS.cso", "Data/Shader/ChargeDX12PS.cso") {}

	virtual ~ChargeShaderdDX12() override = default;
};

/**************************************************************************//**
	@class    HealingShaderdCylinderDX12
	@brief    HealingShaderdCylinderDX12のパラーメータ設定クラス
	@par    [説明]
			円錐の回復の効果を描画するクラス
*//***************************************************************************/
class HealingShaderdCylinderDX12 : public ZoneShaderDX12
{
public:
	HealingShaderdCylinderDX12(ID3D12Device* device) : ZoneShaderDX12(device, "Data/Shader/PrimitiveDX12VS.cso", "Data/Shader/HealCylinderDX12PS.cso") {}

	virtual ~HealingShaderdCylinderDX12() override = default;
};

/**************************************************************************//**
	@class    HealingShaderdCircleDX12
	@brief    HealingShaderdCircleDX12のパラーメータ設定クラス
	@par    [説明]
		円の回復の効果を描画するクラス
*//***************************************************************************/
class HealingShaderdCircleDX12 : public ZoneShaderDX12
{
public:
	HealingShaderdCircleDX12(ID3D12Device* device) : ZoneShaderDX12(device, "Data/Shader/PrimitiveDX12VS.cso", "Data/Shader/HealCircleDX12PS.cso") {}

	virtual ~HealingShaderdCircleDX12() override = default;
};

/**************************************************************************//**
	@class    BeamDX12DX12
	@brief    BeamDX12DX12のパラーメータ設定クラス
	@par    [説明]
		ビームの効果を描画するクラス
*//***************************************************************************/
class BeamDX12 : public ZoneShaderDX12
{
public:
	BeamDX12(ID3D12Device* device) : ZoneShaderDX12(device, "Data/Shader/PrimitiveDX12VS.cso", "Data/Shader/BeamDX12PS.cso") {}

	virtual ~BeamDX12() override = default;
};

#endif //!__INCLUDED_CYLINDER_SHADER_H__
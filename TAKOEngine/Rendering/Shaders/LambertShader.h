//! @file LambertShader.h
//! @note

#ifndef __GRAPHICS_LAMBERT_SHADER_H__
#define __GRAPHICS_LAMBERT_SHADER_H__

#include "TAKOEngine/Rendering/Shaders/Shader.h"
#include "TAKOEngine/Rendering/Shaders/ModelShader.h"
#include "TAKOEngine/Rendering/Model/ModelResource.h"

#include "ModelShaderDX12.h"

//****************************************************
// @class LambertShader
// @brief DirectX11の描画
// @par   PBRべース描画
//****************************************************
class LambertShader : public ModelShader
{
private:
	const char* VertexShaderName = "Data/Shader/LambertVS.cso";
	const char* PixelShaderName = "Data/Shader/LambertPS.cso";

public:
	LambertShader(ID3D11Device* device);
	~LambertShader() override = default;

	void Begin(const RenderContext& rc) override;
	void End(const RenderContext& rc) override;

	// モデル描画
	void Draw(const RenderContext& rc, const iModel* model, DirectX::XMFLOAT4 color = { 1, 1, 1, 1 }) override;

protected:
	// レンダーステート設定
	void SetRenderState(const RenderContext& rc) override;

	// シェーダーリソースビュー設定
	void SetShaderResourceView(const ModelResource::Mesh& mesh, ID3D11DeviceContext*& dc) override;

private:

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_sceneConstantBuffer;

	struct primitive_constants
	{
		DirectX::XMFLOAT4X4 world;
		int material{ -1 };
		int has_tangent{ 0 };
		int skin{ -1 };
		int pad;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_PrimitiveConstantBuffer;

	static const size_t PRIMITIVE_MAX_JOINTS = 512;
	struct primitive_joint_constants
	{
		DirectX::XMFLOAT4X4 matrices[PRIMITIVE_MAX_JOINTS];
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_PrimitiveJointConstantBuffer;
};

//****************************************************
// @class LambertShader
// @brief DirectX12の描画テスト
// @par   PBRべース描画
//****************************************************
class LambertShaderDX12 : public ModelShaderDX12
{
public:
	LambertShaderDX12(ID3D12Device* device, bool instancing = false);
	~LambertShaderDX12() override;

	void Render(const RenderContextDX12& rc, const ModelDX12::Mesh& mesh) override;

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;

	SamplerManager* m_sampler = nullptr;
};

#endif // !__GRAPHICS_LAMBERT_SHADER_H__

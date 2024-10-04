#pragma once

#include <string>
#include <d3d12.h>

#include "TAKOEngine/Rendering/Model/ModelResource.h"
#include "TAKOEngine/Rendering/Shaders/Shader.h"

class ModelShader : public Shader
{
public:
	ModelShader(ID3D11Device* device, const char* vs, const char* ps);

	virtual ~ModelShader() override = default;

	// 描画開始
	virtual void Begin(const RenderContext& rc) override;
	void Begin(const RenderContextDX12& rc) override;
	void Draw(const RenderContextDX12& rc, iModel* model) override;
	void Draw(const RenderContextDX12& rc, ModelDX12* model) override;
	void End(const RenderContext& rc) override;
	void End(const RenderContextDX12& rc) override;

	// モデル描画
	virtual void Draw(const RenderContext& rc, const iModel* model, DirectX::XMFLOAT4 color = { 1, 1, 1, 1 });

protected:
	// レンダーステート設定
	virtual void SetRenderState(const RenderContext& rc) = 0;
	
	// シェーダーリソースビュー設定
	virtual void SetShaderResourceView(const ModelResource::Mesh& mesh, ID3D11DeviceContext*& dc) = 0;

protected:
	struct CbScene
	{
		DirectX::XMFLOAT4		cameraPosition;
		DirectX::XMFLOAT4X4		viewProjection;
		DirectX::XMFLOAT4		ambientLightColor;
		DirectionalLightData	directionalLightData;
		PointLightData			pointLightData[PointLightMax];	// 点光源
		SpotLightData			spotLightData[SpotLightMax];	// スポットライト
		int						pointLightCount;				// 点光源数
		int						spotLightCount;					// スポットライト数
		DirectX::XMFLOAT2		dummy;
	};

	struct CbMesh
	{
		DirectX::XMFLOAT4 materialColor;
		float linearGamma;								// リニア補正
		DirectX::XMFLOAT3 dummy;
		ShaderData shaderData;
	};

	struct CbSkeleton
	{
		DirectX::XMFLOAT4X4 boneTransforms[256];
	};

	struct CbShadowMap
	{
		DirectX::XMFLOAT4X4 lightViewProjection[myRenderer::NUM_SHADOW_MAP];  //ライトビュープロジェクション行列
		DirectX::XMFLOAT4   shadowBias;                           //深度比較用のオフセット値
		DirectX::XMFLOAT3   shadowColor;			              //影の色
		float               dummy;
	};

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> sceneConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> meshConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> skeletonConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> shadowMapConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> toontexture;

	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;

	PipelineState m_pipelineState;
};

//******************************************************************
//  @class  SkinningPipeline
//  @brief  スキニングパイプライン
//  @par    コンピュートシェーダーを使用して最適化を行う
//******************************************************************
class SkinningPipeline
{
public:
	SkinningPipeline(ID3D12Device* device);
	~SkinningPipeline();

	//計算
	void Compute(const RenderContextDX12& rc, ModelDX12* model);

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_d3d_root_signature;
};

//! @file ModelShaderDX12.h
//! @note

#ifndef __GRAPHICS_MODEL_SHADERDX12_H__
#define __GRAPHICS_MODEL_SHADERDX12_H__

#include <string>
#include <d3d12.h>

#include "TAKOEngine/Rendering/RenderContext.h"
#include "TAKOEngine/Rendering/Model/ModelDX12.h"
#include "TAKOEngine/Rendering/Model/ModelResource.h"
#include "TAKOEngine/Rendering/GpuResourceUtils.h"
#include "TAKOEngine/Rendering/FrustumCulling.h"

//******************************************************************
//  @class  ModelShaderDX12
//  @brief  シェーダー関連の基底クラス(DX12)
//  @par    [説明]
//******************************************************************
class ModelShaderDX12
{
protected:
	ModelShaderDX12() {};

public:
	virtual ~ModelShaderDX12() {};

	//描画
	virtual void Render(const RenderContextDX12& rc, iModel* model) = 0;
};

//******************************************************************
//  @class  SkinningPipeline
//  @brief  スキニングパイプライン
//  @par    [説明]
//  @par    コンピュートシェーダーを使用して最適化を行う
//******************************************************************
class SkinningPipeline
{
public:
	SkinningPipeline(ID3D12Device* device);
	~SkinningPipeline();

	//計算
	void Compute(const RenderContextDX12& rc, iModel* model);

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_d3d_root_signature;
};

#endif // !__GRAPHICS_MODEL_SHADERDX12_H__

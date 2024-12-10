#pragma once

#include "TAKOEngine/Tool/Singleton.h"
#include "GameObject/ObjectManager.h"
#include "TAKOEngine/Rendering/Model/ModelDX12.h"

class InstancingModelManager : public Manager<ModelDX12>, public Singleton<InstancingModelManager>
{
	friend class Singleton<InstancingModelManager>;
public:
	void Update(float elapsedTime)
	{

	}

	void RenderDX12(const RenderContextDX12& rc)
	{
		for (ModelDX12* item : this->items)
		{
			// モデル描画
			ModelShaderDX12Id shaderId = static_cast<ModelShaderDX12Id>(0xFFFFFFFF);
			ModelShaderDX12* shader = nullptr;

			for (const ModelDX12::Mesh& mesh : item->GetMeshes())
			{
				ModelShaderDX12Id currentShaderId = static_cast<ModelShaderDX12Id>(mesh.mesh->material->shaderId);

				currentShaderId = ModelShaderDX12Id::ToonInstancing;

				if (shaderId != currentShaderId)
				{
					// シェーダー変更
					shaderId = currentShaderId;

					//currentShaderIdがEnumCountより多かったら
					if (currentShaderId == ModelShaderDX12Id::EnumCount) continue;

					// パイプライン設定
					shader = T_GRAPHICS.GetModelShaderDX12(currentShaderId);
				}

				if (mesh.frame_resources.size() == 0) continue;

				//描画
				shader->Render(rc, mesh);
			}
		}
	}
};
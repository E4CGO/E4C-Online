//! @file ModelObject.cpp
//! @note
#include "ModelObject.h"
#include "TAKOEngine/Physics/UnrotatedBoxCollider.h"
#include "TAKOEngine/Physics/BoundingBoxCollider.h"
#include "TAKOEngine/Physics/SphereCollider.h"
#include "TAKOEngine/Physics/CapsuleCollider.h"
#include "TAKOEngine/Physics/ModelCollider.h"
#include "TAKOEngine/Physics/MapCollider.h"

/**************************************************************************//**
	@brief		コンストラクタ（引数付き）
	@param[in]	filename	モデルファイルパス
	@param[in]	scaling		モデルスケール
	@param[in]	renderMode	レンダーボード
*//***************************************************************************/
ModelObject::ModelObject(const char* filename, float scaling, ModelObject::RENDER_MODE renderMode, ModelObject::MODEL_TYPE modelType)
{
	LoadModel(filename, scaling, renderMode, modelType);
}

/**************************************************************************//**
	@brief		モデル生成
	@param[in]	filename	戻るファイルパス
	@param[in]	scaling		モデルスケール
	@param[in]	renderMode	レンダーモード
	@param[in]	modelType	モデル作り方分け
	return		なし
*//***************************************************************************/
void ModelObject::LoadModel(const char* filename, float scaling, ModelObject::RENDER_MODE renderMode, ModelObject::MODEL_TYPE modelType)
{
	if (strlen(filename) == 0) return;

	m_renderMode = renderMode;

	switch (modelType)
	{
	case ModelObject::RHS_PBR:
		m_shaderId = ModelShaderId::Lambert;
		break;
	case ModelObject::RHS_TOON:
		m_shaderId = ModelShaderId::Toon;
		m_dx12_ShaderId = ModelShaderDX12Id::Toon;
		break;
	case ModelObject::LHS_PBR:
		m_shaderId = ModelShaderId::Phong;
		m_dx12_ShaderId = ModelShaderDX12Id::Lambert;
		break;
	case ModelObject::LHS_TOON:
		m_shaderId = ModelShaderId::Toon;
		m_dx12_ShaderId = ModelShaderDX12Id::Toon;
		break;
	default:
		break;
	}

	switch (m_renderMode)
	{
	case ModelObject::RENDER_MODE::DX11:
		m_pmodels.push_back(std::make_unique<ModelDX11>(T_GRAPHICS.GetDevice(), filename, scaling, modelType));
		break;
	case ModelObject::RENDER_MODE::DX11GLTF:
		m_pmodels.push_back(std::make_unique<GLTFModelDX11>(T_GRAPHICS.GetDevice(), filename, scaling, modelType));
		break;
	case ModelObject::RENDER_MODE::DX12:
		m_pmodels.push_back(std::make_unique<ModelDX12>(T_GRAPHICS.GetDeviceDX12(), filename, scaling, modelType));
		SetShader(m_pmodels.at(modelIndex++)->GetModelName().c_str(), m_dx12_ShaderId);
		break;
	case ModelObject::RENDER_MODE::DX12GLTF:
		break;
	case ModelObject::RENDER_MODE::NOMODEL:
		break;
	}

	//スキニング
	m_skinning_pipeline = T_GRAPHICS.GetSkinningPipeline();
}

/**************************************************************************//**
	@brief		モデルリストをクリーン
	@param[in]	なし
	@return なし
*//***************************************************************************/
void ModelObject::CleanModels()
{
	m_pmodels.clear();
	modelIndex = 0;
}

/**************************************************************************//**
	@brief		マテリアルのshaderの設定
	@param[in]	modelName	　Modelの名前
	@param[in]	id			　シェーダーID
	@param[in]	materialName　マテリアルの名前の配列(vector型)
	@return なし
*//***************************************************************************/
void ModelObject::SetShader(const char* modelName, const ModelShaderDX12Id id, const std::vector<const char*>& materialNames)
{
	// 名前検索
	iModel* model = FindModelName(modelName);

	if (!model) return;

	// シェーダー設定
	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Material& material : resource->GetMaterials())
	{
		ModelResource::Material& mat = const_cast<ModelResource::Material&>(material);

		// マテリアル名が指定されていない場合、または一致する場合にシェーダーを設定
		if (materialNames.empty() || std::find(materialNames.begin(), materialNames.end(), mat.name) != materialNames.end())
		{
			mat.shaderId = static_cast<int>(id);
		}
	}
}

/**************************************************************************//**
	@brief		モデルの名前検索
	@param[in]	name	ファイルパス
	@return     ModelInfo*
*//***************************************************************************/
iModel* ModelObject::FindModelName(const char* name)
{
	// 全て総当たりで名前比較する
	for (auto& model : m_pmodels)
	{
		if (model->GetModelName() == name)
		{
			return model.get();
		}
	}

	// 見つからなかった
	return nullptr;
}

/**************************************************************************//**
	@brief		モデルアニメーション設定
	@param[in]	index			アニメーションインデックス
	@param[in]	loop			ループフラグ
	@param[in]	blendSeconds	ブレンドタイム（秒）
	@return なし
*//***************************************************************************/
void ModelObject::SetAnimation(const int index, bool loop, float blendSeconds)
{
	for (auto& model : m_pmodels)
	{
		if (model != nullptr)
			model->PlayAnimation(index, loop, blendSeconds);
	}
}

/**************************************************************************//**
	@brief		個別モデルアニメーション設定
	@param[in]	model_idx		モデルインデックス
	@param[in]	animation_index	アニメーションインデックス
	@param[in]	loop			ループフラグ
	@param[in]	blendSeconds	ブレンドタイム（秒）
	@return なし
*//***************************************************************************/
void ModelObject::SetModelAnimation(const int model_idx, const int animation_index, const bool loop, const float blendSeconds)
{
	if (m_pmodels.size() <= model_idx) return;
	m_pmodels[model_idx]->PlayAnimation(animation_index, loop, blendSeconds);
}

/**************************************************************************//**
	@brief	全てのモデルのアニメーション判定
	@return	アニメーション中判定
*//***************************************************************************/
bool ModelObject::IsPlayAnimation(void)
{
	for (auto& model : m_pmodels)
	{
		if (model->IsPlayAnimation()) return true;
	}
	return false;
}

/**************************************************************************//**
	@brief	個別てのモデルのアニメーション判定
	@param[in]	idx モデルインデックス
	@return	アニメーション中判定
*//***************************************************************************/
bool ModelObject::IsPlayAnimation(int idx)
{
	return m_pmodels[idx]->IsPlayAnimation();
}

/**************************************************************************//**
	@brief	更新処理
	@param[in]	elapsedTime 経過時間
	@return なし
*//***************************************************************************/
void ModelObject::Update(float elapsedTime)
{
	// 行列更新
	UpdateTransform();

	if (T_GRAPHICS.isDX11Active)
	{
		for (auto& model : m_pmodels)
		{
			if (model == nullptr) continue;

			// アニメーション更新
			model->UpdateAnimation(elapsedTime * m_animationSpeed);

			// トランスフォーム更新
			model->UpdateTransform(transform);
		}
	}

	if (T_GRAPHICS.isDX12Active)
	{
		for (auto& model : m_pmodels)
		{
			if (model == nullptr) continue;

			// アニメーション更新
			model->UpdateAnimation(elapsedTime * m_animationSpeed);

			// トランスフォーム更新
			model->UpdateTransform(transform);
		}
	}
}

/**************************************************************************//**
	@brief	描画処理
	@param[in]	rc	レンダーコンテクスト参照
	@return なし
*//***************************************************************************/
void ModelObject::Render(const RenderContext& rc)
{
	if (!m_visible) return;
	for (auto& model : m_pmodels)
	{
		if (model == nullptr) return;

		ModelShader* shader = T_GRAPHICS.GetModelShader(m_shaderId);

		// 描画
		shader->Begin(rc);
		shader->Draw(rc, model.get(), m_color);
		shader->End(rc);
	}
}

/**************************************************************************//**
	@brief	描画処理
	@param[in]	rc	レンダーコンテクスト参照
	@return なし
*//***************************************************************************/
void ModelObject::RenderDX12(const RenderContextDX12& rc)
{
	if (!m_visible) return;

	for (auto& model : m_pmodels)
	{
		if (model == nullptr) return;

		// カメラに写っている範囲のオブジェクトをフラグでマークする配列を用意
		std::vector<bool> visibleObjects(model->GetMeshes().size(), false);
		if (visibleObjects.size() == 0) continue;

		// 視錐台カリングを実行して可視オブジェクトをマーク
		FrustumCulling::FrustumCullingFlag(model->GetMeshes(), visibleObjects);
		int culling = 0;

		// スキニング
		m_skinning_pipeline->Compute(rc, model.get());

		// モデル描画
		ModelShaderDX12Id shaderId = static_cast<ModelShaderDX12Id>(0xFFFFFFFF);
		ModelShaderDX12* shader = nullptr;

		for (const ModelDX12::Mesh& mesh : model->GetMeshes())
		{
			ModelShaderDX12Id currentShaderId = static_cast<ModelShaderDX12Id>(mesh.mesh->material->shaderId);

			if (shaderId != currentShaderId)
			{
				// シェーダー変更
				shaderId = currentShaderId;

				//currentShaderIdがEnumCountより多かったら
				if (currentShaderId == ModelShaderDX12Id::EnumCount) continue;

				// パイプライン設定
				shader = T_GRAPHICS.GetModelShaderDX12(currentShaderId);
			}

			// フラグがfalseの場合はフラスタム外なのでスキップ
			if (!visibleObjects[culling++]) continue;

			if (mesh.frame_resources.size() == 0) continue;

			//描画
			shader->Render(rc, mesh);
		}
	}
}

/**************************************************************************//**
	@brief		コライダー設定
	@param[in]	collider	コライダータイプ
	@param[in]	idx			モデルID
	@return		なし
*//***************************************************************************/
void ModelObject::SetCollider(Collider::COLLIDER_TYPE collider, int idx)
{
	switch (collider)
	{
	case Collider::COLLIDER_TYPE::SPHERE:
		this->collider = std::make_unique<SphereCollider>();
		break;
	case Collider::COLLIDER_TYPE::UNROTATED_BOX:
		this->collider = std::make_unique<UnrotatedBoxCollider>();
		break;
	case Collider::COLLIDER_TYPE::CAPSULE:
		this->collider = std::make_unique<CapsuleCollider>();
		break;
	case Collider::COLLIDER_TYPE::MODEL:
		this->collider = std::make_unique<ModelCollider>(m_pmodels[idx].get());
		break;
	case Collider::COLLIDER_TYPE::BOUNDING_BOX:
		this->collider = std::make_unique<BoundingBoxCollider>(m_pmodels[idx].get());
		break;
	case Collider::COLLIDER_TYPE::MAP:
		//this->collider = std::make_unique<ModelCollider>(model.get());
		this->collider = std::make_unique<MapCollider>(m_pmodels[idx].get());
		break;
	default:
		this->collider = nullptr;
		break;
	}
}

/**************************************************************************//**
	@brief		モデルノード座標を取得
	@param[in]	idx			モデルインデックス
	@param[in]	nodeName	ノード名前
	@param[in]	offset		オフセット
	@return		モデルノード座標
*//***************************************************************************/
DirectX::XMFLOAT3 ModelObject::GetNodePosition(int idx, const char* nodeName, const DirectX::XMFLOAT3& offset)
{
	DirectX::XMFLOAT3 pos = {};
	iModel::Node* node = m_pmodels[idx]->FindNode(nodeName);
	if (node == nullptr) return pos;

	DirectX::XMVECTOR Offset = DirectX::XMLoadFloat3(&offset);
	DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&node->worldTransform);
	DirectX::XMStoreFloat3(&pos, DirectX::XMVector3Transform(Offset, W));

	return pos;
}

/**************************************************************************//**
	@brief		モデルルートノード座標を取得
	@param[in]	idx			モデルインデックス
	@param[in]	offset		オフセット
	@return		モデルノード座標
*//***************************************************************************/
DirectX::XMFLOAT3 ModelObject::GetNodePosition(int idx, const DirectX::XMFLOAT3& offset)
{
	DirectX::XMFLOAT3 pos = {};
	iModel::Node* node = m_pmodels[idx]->GetRootNode();

	DirectX::XMVECTOR Offset = DirectX::XMLoadFloat3(&offset);
	DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&node->worldTransform);
	DirectX::XMStoreFloat3(&pos, DirectX::XMVector3Transform(Offset, W));

	return pos;
}
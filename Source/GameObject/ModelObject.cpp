//! @file ModelObject.cpp
//! @note
#include "ModelObject.h"
#include "TAKOEngine/Physics/UnrotatedBoxCollider.h"
#include "TAKOEngine/Physics/BoundingBoxCollider.h"
#include "TAKOEngine/Physics/SphereCollider.h"
#include "TAKOEngine/Physics/ModelCollider.h"
#include "TAKOEngine/Physics/MapCollider.h"

#include "TAKOEngine/Rendering/Model/ModelDX11.h"
#include "TAKOEngine/Rendering/Model/NewModelDX11.h"
#include "TAKOEngine/Rendering/Model/ModelDX12.h"

/**************************************************************************//**
	@brief		コンストラクタ（引数付き）
	@param[in]	filename	モデルファイルパス
	@param[in]	scaling		モデルスケール
	@param[in]	renderMode	レンダーボード
*//***************************************************************************/
ModelObject::ModelObject(const char* filename, float scaling, ModelObject::RENDER_MODE renderMode)
{
	LoadModel(filename, scaling, renderMode);
}

/**************************************************************************//**
	@brief		モデル生成
	@param[in]	filename	戻るファイルパス
	@param[in]	scaling		モデルスケール
	@param[in]	renderMode	レンダーモード
	return		なし
*//***************************************************************************/
void ModelObject::LoadModel(const char* filename, float scaling, ModelObject::RENDER_MODE renderMode)
{
	if (strlen(filename) == 0) return;

	m_renderMode = renderMode;

	switch (m_renderMode)
	{
	case ModelObject::RENDER_MODE::DX11:
		m_pmodels.push_back(std::make_unique<ModelDX11>(T_GRAPHICS.GetDevice(), filename, scaling));
		break;
	case ModelObject::RENDER_MODE::DX11GLTF:
		m_pmodels.push_back(std::make_unique<NewModelDX11>(T_GRAPHICS.GetDevice(), filename, scaling));
		break;
	case ModelObject::RENDER_MODE::DX12:
		break;
	case ModelObject::RENDER_MODE::DX12GLTF:
		break;
	case ModelObject::RENDER_MODE::NOMODEL:
		break;
	}
}

void ModelObject::CleanModels()
{
	m_pmodels.clear();
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
bool ModelObject::IsPlayAnimaition(void)
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
bool ModelObject::IsPlayAnimaition(int idx)
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
	for (auto& model : m_pmodels)
	{
		if (model == nullptr) continue;

		// 行列更新
		UpdateTransform();

		// アニメーション更新
		model->UpdateAnimation(elapsedTime * m_animationSpeed);

		// トランスフォーム更新
		model->UpdateTransform(transform);
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
		if (m_renderMode != DX11)
		{
			static_cast<NewModelDX11*>(model.get())->Render(rc);
			return;
		}
		// 描画
		ModelShader* shader = T_GRAPHICS.GetModelShader(m_shaderId);
		shader->Begin(rc);
		shader->Draw(rc, model.get(), m_color);
		shader->End(rc);
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
	case Collider::COLLIDER_TYPE::UNROTATED_BOX:
		this->collider = std::make_unique<UnrotatedBoxCollider>();
		break;
	case Collider::COLLIDER_TYPE::MODEL:
		this->collider = std::make_unique<ModelCollider>(m_pmodels[idx].get());
		break;
	case Collider::COLLIDER_TYPE::SPHERE:
		this->collider = std::make_unique<SphereCollider>();
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
//! @file ModelObject.cpp
//! @note
#include "ModelObject.h"
#include "TAKOEngine/Physics/SphereCollider.h"
#include "TAKOEngine/Physics/AABBCollider.h"
#include "TAKOEngine/Physics/OBBCollider.h"
#include "TAKOEngine/Physics/CapsuleCollider.h"
#include "TAKOEngine/Physics/ModelCollider.h"
#include "TAKOEngine/Physics/MapCollider.h"
#include "TAKOEngine/Physics/AttackCollider.h"
#include "TAKOEngine/Physics/CollisionManager.h"

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
	@brief		デストラクタ
*//***************************************************************************/
ModelObject::~ModelObject()
{
	for (const std::pair<uint8_t, Collider*>& collider : m_pColliders)
	{
		COLLISIONS.Remove(collider.second);
	}
	m_pColliders.clear();
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
	case ModelObject::LHS_PBR_Instancing:
		m_dx12_ShaderId = ModelShaderDX12Id::LambertInstancing;
		break;
	case ModelObject::LHS_TOON:
		m_shaderId = ModelShaderId::Toon;
		m_dx12_ShaderId = ModelShaderDX12Id::Toon;
		break;
	case ModelObject::LHS_TOON_Instancing:
		m_dx12_ShaderId = ModelShaderDX12Id::ToonInstancing;
		break;
	case ModelObject::LHS_Phong:
		m_dx12_ShaderId = ModelShaderDX12Id::Phong;
		break;
	case ModelObject::LHS_Phong_Instancing:
		m_dx12_ShaderId = ModelShaderDX12Id::PhongInstancing;
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

#ifdef _DEBUG
	if(m_pMoveCollider)
	{
		m_pMoveCollider->DrawDebugPrimitive({ 1, 1, 1, 1 });
	}
	if (!m_pColliders.empty())
	{
		for (const std::pair<uint8_t, Collider*>& attackCollider : m_pColliders)
		{
			//if (attackCollider.first == COLLIDER_ID::COL_BODY) continue;
			DirectX::XMFLOAT4 color = { 1, 1, 1, 1 };
			if (attackCollider.second->GetOBJType() & (Collider::COLLIDER_OBJ::PLAYER | Collider::COLLIDER_OBJ::ENEMY | Collider::COLLIDER_OBJ::ITEM))
			{
				color = { 0, 1, 0, 1 };
			}
			else if (attackCollider.second->GetOBJType() & (Collider::COLLIDER_OBJ::PLAYER_ATTACK | Collider::COLLIDER_OBJ::PLAYER_PROJECTILE | Collider::COLLIDER_OBJ::ENEMY_ATTACK | Collider::COLLIDER_OBJ::ENEMY_PROJECTILE))
			{
				color = { 1, 0, 0, 1 };
			}

			attackCollider.second->DrawDebugPrimitive(color);
		}
	}
#endif // _DEBUG
}

/**************************************************************************//**
	@brief		移動用コライダー設定(名前がSetCollider→SetMoveColliderに変わりました。Mapの当たり判定は基本こっちのみ)
	@param[in]	shapeType	コライダータイプ
	@param[in]	objType		オブジェクトタイプ
	@param[in]	idx			モデルID
	@return		なし
*//***************************************************************************/
void ModelObject::SetMoveCollider(Collider::COLLIDER_TYPE shapeType, Collider::COLLIDER_OBJ objType, int idx)
{
	switch (shapeType)
	{
	case Collider::COLLIDER_TYPE::SPHERE:
		m_pMoveCollider = std::make_unique<SphereCollider>(objType, &transform);
		break;
	case Collider::COLLIDER_TYPE::AABB:
		m_pMoveCollider = std::make_unique<AABBCollider>(objType, &transform);
		break;
	case Collider::COLLIDER_TYPE::OBB:
		m_pMoveCollider = std::make_unique<OBBCollider>(objType, &transform);
		break;
	case Collider::COLLIDER_TYPE::CAPSULE:
		m_pMoveCollider = std::make_unique<CapsuleCollider>(objType, &transform);
		break;
	case Collider::COLLIDER_TYPE::MODEL:
		m_pMoveCollider = std::make_unique<ModelCollider>(m_pmodels[idx].get(), objType, &transform);
		break;
	case Collider::COLLIDER_TYPE::MAP:
		//this->collider = std::make_unique<ModelCollider>(model.get());
		m_pMoveCollider = std::make_unique<MapCollider>(m_pmodels[idx].get(), objType, &transform);
		break;
	default:
		m_pMoveCollider = nullptr;
		break;
	}

	m_pMoveCollider->SetOwner(this);
}

/**************************************************************************//**
	@brief		移動用コライダー設定
	@param[in]	sphereParam		球コライダーのパラメータ
	@param[in]	objType			オブジェクトタイプ
	@return		なし
*//***************************************************************************/
void ModelObject::SetMoveCollider(Sphere sphereParam, Collider::COLLIDER_OBJ objType)
{
	m_pMoveCollider = std::make_unique<SphereCollider>(objType, &transform);
	m_pMoveCollider->SetOwner(this);
	m_pMoveCollider->SetParam(sphereParam);
}

/**************************************************************************//**
	@brief		当たり判定用コライダー設定
	@param[in]	idx				コライダーID
	@param[in]	sphereParam		球コライダーのパラメータ
	@param[in]	objType			オブジェクトタイプ
	@param[in]	transform		参照するオブジェクト行列
	@return		なし
*//***************************************************************************/
void ModelObject::SetCollider(uint8_t idx, Sphere sphereParam, Collider::COLLIDER_OBJ objType, DirectX::XMFLOAT4X4* transform)
{
	m_pColliders[idx] = new SphereCollider(objType, transform);
	m_pColliders[idx]->SetOwner(this);
	m_pColliders[idx]->SetParam(sphereParam);
	COLLISIONS.Register(m_pColliders[idx]);
}
/**************************************************************************//**
	@brief		当たり判定用コライダー設定
	@param[in]	idx				コライダーID
	@param[in]	capsuleParam	カプセルコライダーのパラメータ
	@param[in]	objType			オブジェクトタイプ
	@param[in]	transform		参照するオブジェクト行列
	@return		なし
*//***************************************************************************/
void ModelObject::SetCollider(uint8_t idx, Capsule capsuleParam, Collider::COLLIDER_OBJ objType, DirectX::XMFLOAT4X4* transform)
{
	m_pColliders[idx] = new CapsuleCollider(objType, transform);
	m_pColliders[idx]->SetOwner(this);
	m_pColliders[idx]->SetParam(capsuleParam);
	COLLISIONS.Register(m_pColliders[idx]);
}

/**************************************************************************//**
	@brief		攻撃判定用コライダー生成
	@param[in]	power			攻撃力
	@param[in]	idx				コライダーID
	@param[in]	sphereParam		球コライダーのパラメータ
	@param[in]	objType			オブジェクトタイプ
	@param[in]	hittableOBJ		ヒットするオブジェクトタイプ
	@param[in]	transform		参照するオブジェクト行列
	@return		なし
*//***************************************************************************/
void ModelObject::MakeAttackCollider(int power, uint8_t idx, Sphere sphereParam, Collider::COLLIDER_OBJ objType, uint16_t hittableOBJ, DirectX::XMFLOAT4X4* transform)
{
	m_pColliders[idx] = new AttackSphereCollider(power, objType, transform);
	m_pColliders[idx]->SetHittableOBJ(hittableOBJ);
	m_pColliders[idx]->SetOwner(this);
	m_pColliders[idx]->SetParam(sphereParam);
	COLLISIONS.Register(m_pColliders[idx]);
}
void ModelObject::MakeAttackCollider(ATTACK_COLLIDER_DATA data, Sphere sphereParam, DirectX::XMFLOAT4X4* transform)
{
	m_pColliders[data.idx] = new AttackSphereCollider(data.power, data.objType, transform, data.hitStartRate, data.hitEndRate);
	m_pColliders[data.idx]->SetHittableOBJ(data.hittableOBJ);
	m_pColliders[data.idx]->SetOwner(this);
	m_pColliders[data.idx]->SetParam(sphereParam);
	COLLISIONS.Register(m_pColliders[data.idx]);
}
/**************************************************************************//**
	@brief		攻撃判定用コライダー生成
	@param[in]	power			攻撃力
	@param[in]	idx				コライダーID
	@param[in]	capsuleParam	カプセルコライダーのパラメータ
	@param[in]	objType			オブジェクトタイプ
	@param[in]	hittableOBJ		ヒットするオブジェクトタイプ
	@param[in]	transform		参照するオブジェクト行列
	@return		なし
*//***************************************************************************/
void ModelObject::MakeAttackCollider(int power, uint8_t idx, Capsule capsuleParam, Collider::COLLIDER_OBJ objType, uint16_t hittableOBJ, DirectX::XMFLOAT4X4* transform)
{
	m_pColliders[idx] = new AttackCapsuleCollider(power, objType, transform);
	m_pColliders[idx]->SetHittableOBJ(hittableOBJ);
	m_pColliders[idx]->SetOwner(this);
	m_pColliders[idx]->SetParam(capsuleParam);
	COLLISIONS.Register(m_pColliders[idx]);
}
void ModelObject::MakeAttackCollider(ATTACK_COLLIDER_DATA data, Capsule capsuleParam, DirectX::XMFLOAT4X4* transform)
{
	m_pColliders[data.idx] = new AttackCapsuleCollider(data.power, data.objType, transform, data.hitStartRate, data.hitEndRate);
	m_pColliders[data.idx]->SetHittableOBJ(data.hittableOBJ);
	m_pColliders[data.idx]->SetOwner(this);
	m_pColliders[data.idx]->SetParam(capsuleParam);
	COLLISIONS.Register(m_pColliders[data.idx]);
}

/**************************************************************************//**
	@brief		攻撃判定用コライダー消去
	@param[in]	idx				コライダーID
	@return		なし
*//***************************************************************************/
void ModelObject::DeleteAttackCollider(uint8_t idx)
{
	COLLISIONS.Remove(m_pColliders[idx]);
	m_pColliders.erase(idx);
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

/**************************************************************************//**
 	@brief	コライダー更新処理
*//***************************************************************************/
void ModelObject::UpdateColliders()
{
	if (m_pMoveCollider) m_pMoveCollider->Update();
	for (const std::pair<int, Collider*>& collider : m_pColliders)
	{
		collider.second->Update();
	}
}
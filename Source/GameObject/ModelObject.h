//! @file GameObject.h
//! @note

#ifndef __INCLUDED_MODEL_OBJECT_H__
#define __INCLUDED_MODEL_OBJECT_H__

#include <unordered_map>
#include "GameObject.h"
#include "TAKOEngine/Rendering/Model/ModelDX11.h"
#include "TAKOEngine/Rendering/Model/GLTFModelDX11.h"
#include "TAKOEngine/Rendering/Model/ModelDX12.h"

/**************************************************************************//**
	@class	ModelObject
	@brief	モデルを保有するゲームオブジェクトクラス
	@par	[説明]
*//***************************************************************************/
class ModelObject : public GameObject
{
public:
	// TODO: iModelに移動
	enum RENDER_MODE
	{
		DX11 = 0,
		DX11GLTF,
		DX12,
		DX12GLTF,
		NOMODEL,
	};
	// TODO: iModelに移動
	enum MODEL_TYPE
	{
		RHS_PBR,
		RHS_TOON,
		LHS_PBR,
		LHS_PBR_Instancing,
		LHS_TOON,
		LHS_TOON_Instancing,
		LHS_Phong,
		LHS_Phong_Instancing,
		Foliage,
	};

	RENDER_MODE m_renderMode = DX11;

	// コンストラクタ
	ModelObject(void) {};
	// コンストラクタ（引数付き）
	ModelObject(const char* filename, float scaling = 1.0f, ModelObject::RENDER_MODE renderMode = ModelObject::DX11, ModelObject::MODEL_TYPE modelType = ModelObject::MODEL_TYPE::LHS_TOON);
	virtual ~ModelObject();

	// モデルを読み取り
	void LoadModel(const char* filename, float scaling = 1.0f, ModelObject::RENDER_MODE renderMode = ModelObject::RENDER_MODE::DX11, ModelObject::MODEL_TYPE modelType = ModelObject::MODEL_TYPE::LHS_TOON);

	void CleanModels();

	// 更新処理
	virtual void Update(float elapsedTime) override;
	// 描画処理
	virtual void Render(const RenderContext& rc) override;
	virtual void RenderDX12(const RenderContextDX12& rc) override;

	// モデルを取得
	std::unique_ptr<iModel>& GetModel(int idx = 0) { return m_pmodels[idx]; }
	// モデルリストを取得
	std::vector<std::unique_ptr<iModel>>& GetModels() { return m_pmodels; }

	// 表示設定
	void Show() { m_visible = true; }
	// 非表示設定
	void Hide() { m_visible = false; }

	// シェーダー設定
	void SetShader(const ModelShaderId id) { m_shaderId = id; };
	void SetShaderDX12(const ModelShaderDX12Id id) { m_dx12_ShaderId = id; };
	void SetShader(const char* modelName, const ModelShaderDX12Id id, const std::vector<const char*>& materialNames = {});

	// モデルの名前検索
	iModel* FindModelName(const char* name);

	// アニメーション設定
	void SetAnimation(const int index, const bool loop, const float blendSeconds = 0.2f);
	// 個別モデルアニメーション設定
	void SetModelAnimation(const int model_idx, const int animation_index, const bool loop, const float blendSeconds = 0.2f);
	// アニメーション判定
	bool IsPlayAnimation(void);
	// 個別モデルアニメーション判定
	bool IsPlayAnimation(const int model_idx);

	// カラー設定
	void SetColor(const DirectX::XMFLOAT4 color) { this->m_color = color; }

	// 位置設定
	virtual void SetPosition(const DirectX::XMFLOAT3& position) override {
		this->position = position; 
		if(m_pMoveCollider)
			m_pMoveCollider->SetPosition(position); 
	}

	// 移動用コライダー
	Collider* GetMoveCollider() { return m_pMoveCollider.get(); }
	void SetMoveCollider(Collider::COLLIDER_TYPE shapeType, Collider::COLLIDER_OBJ objType, int idx = 0);
	void SetMoveCollider(Sphere sphereParam, Collider::COLLIDER_OBJ objType);
	
	// 当たり判定用コライダー
	std::unordered_map<uint8_t, Collider*>& GetColliders() { return m_pColliders; }
	Collider* GetCollider(uint8_t idx) { return (m_pColliders.contains(idx)) ? m_pColliders[idx] : nullptr; }
	void SetCollider(uint8_t idx, Sphere sphereParam, Collider::COLLIDER_OBJ objType, DirectX::XMFLOAT4X4* transform);
	void SetCollider(uint8_t idx, Capsule capsuleParam, Collider::COLLIDER_OBJ objType, DirectX::XMFLOAT4X4* transform);
	void MakeAttackCollider(int power, uint8_t idx, Sphere sphereParam, Collider::COLLIDER_OBJ objType, uint16_t hittableOBJ, DirectX::XMFLOAT4X4* transform);
	void MakeAttackCollider(int power, uint8_t idx, Capsule capsuleParam, Collider::COLLIDER_OBJ objType, uint16_t hittableOBJ, DirectX::XMFLOAT4X4* transform);
	struct ATTACK_COLLIDER_DATA
	{
		int power = 0;
		uint8_t idx = 0;
		Collider::COLLIDER_OBJ objType = Collider::COLLIDER_OBJ::HIT_ERR;
		uint16_t hittableOBJ = 0;
		float hitStartRate = 0.0f;
		float hitEndRate = 1.0f;
	};
	void MakeAttackCollider(ATTACK_COLLIDER_DATA data, Sphere sphereParam, DirectX::XMFLOAT4X4* transform);
	void MakePlayerNormalAttackCollider(ATTACK_COLLIDER_DATA data, Sphere sphereParam, DirectX::XMFLOAT4X4* transform);
	void MakePlayerSkill1AttackCollider(ATTACK_COLLIDER_DATA data, Sphere sphereParam, DirectX::XMFLOAT4X4* transform);
	void MakeAttackCollider(ATTACK_COLLIDER_DATA data, Capsule capsuleParam, DirectX::XMFLOAT4X4* transform);
	void DeleteAttackCollider(uint8_t idx);

	// アニメーションのスピードを取得
	float GetAnimationSpeed() { return m_animationSpeed; }
	// アニメーションのスピードを設定
	void SetAnimationSpeed(const float speed) { m_animationSpeed = speed; }

	// モデルノード座標を取得
	DirectX::XMFLOAT3 GetNodePosition(int idx, const char* nodeName, const DirectX::XMFLOAT3& offset = {});
	// 頭モデルノード座標を取得
	DirectX::XMFLOAT3 GetNodePosition(const char* nodeName, const DirectX::XMFLOAT3& offset = {})
	{
		return GetNodePosition(0, nodeName, offset);
	}
	// モデルルートノード座標を取得
	DirectX::XMFLOAT3 GetNodePosition(int idx, const DirectX::XMFLOAT3& offset = {});
	// 頭モデルルートノード座標を取得
	DirectX::XMFLOAT3 GetNodePosition(const DirectX::XMFLOAT3& offset = {})
	{
		return GetNodePosition(0, offset);
	}
protected:
	// コライダー更新処理
	virtual void UpdateColliders();
protected:
	// 色
	DirectX::XMFLOAT4 m_color = { 1, 1, 1, 1 };

	// 移動用コライダー
	std::unique_ptr<Collider> m_pMoveCollider = nullptr;
	// 当たり判定用コライダー
	std::unordered_map<uint8_t, Collider*> m_pColliders;

	// シェーダーID
	ModelShaderId m_shaderId = ModelShaderId::Toon;
	ModelShaderDX12Id m_dx12_ShaderId = ModelShaderDX12Id::Toon;

	// モデルリスト
	int modelIndex = 0;
	std::vector<std::unique_ptr<iModel>> m_pmodels;

	// 可視化
	bool m_visible = true;

	// アニメーションスピード
	float m_animationSpeed = 1.0f;

	//スキニング
	SkinningPipeline* m_skinning_pipeline = nullptr;
};

#endif //__INCLUDED_MODEL_OBJECT_H__
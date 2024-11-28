//! @file GameObject.h
//! @note

#ifndef __INCLUDED_MODEL_OBJECT_H__
#define __INCLUDED_MODEL_OBJECT_H__

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
	enum RENDER_MODE
	{
		DX11 = 0,
		DX11GLTF,
		DX12,
		DX12GLTF,
		NOMODEL,
	};

	RENDER_MODE m_renderMode = DX11;

	struct ModelInfo
	{
		std::string name;
		std::unique_ptr<iModel> model;
	};

	// コンストラクタ
	ModelObject(void) {};
	// コンストラクタ（引数付き）
	ModelObject(const char* filename, float scaling = 1.0f, ModelObject::RENDER_MODE renderMode = ModelObject::DX11, int modelType = 1);
	virtual ~ModelObject() = default;

	// モデルを読み取り
	void LoadModel(const char* filename, float scaling = 1.0f, ModelObject::RENDER_MODE renderMode = ModelObject::RENDER_MODE::DX12, int modelType = 1);

	void CleanModels();

	// 更新処理
	virtual void Update(float elapsedTime) override;
	// 描画処理
	virtual void Render(const RenderContext& rc) override;
	virtual void RenderDX12(const RenderContextDX12& rc) override;

	// モデルを取得
	std::unique_ptr<iModel>& GetModel(int idx = 0) { return m_pmodels[idx].model; }
	// モデルリストを取得
	std::vector<ModelInfo>& GetModels() { return m_pmodels; }

	// 表示設定
	void Show() { m_visible = true; }
	// 非表示設定
	void Hide() { m_visible = false; }

	// シェーダー設定
	void SetShader(const ModelShaderId id) { m_shaderId = id; };
	void SetShader(const char* modelName, const ModelShaderDX12Id id);

	// モデルの名前抜き取り
	const char* extractBaseName(const char* filePath);

	// モデルの名前検索
	ModelInfo* FindModelName(const char* name);

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

	// 衝突判定
	Collider* GetCollider() { return collider.get(); }
	void SetCollider(Collider::COLLIDER_TYPE collider, int idx = 0);

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
	// 色
	DirectX::XMFLOAT4 m_color = { 1, 1, 1, 1 };

	// 衝突判定
	std::unique_ptr<Collider> collider = nullptr;

	// シェーダーID
	ModelShaderId m_shaderId = ModelShaderId::Toon;
	ModelShaderDX12Id m_dx12_ShaderId = ModelShaderDX12Id::Toon;

	// モデルリスト
	std::vector<ModelInfo> m_pmodels;

	// 可視化
	bool m_visible = true;

	// アニメーションスピード
	float m_animationSpeed = 1.0f;

	//スキニング
	SkinningPipeline* m_skinning_pipeline = nullptr;
};

#endif //__INCLUDED_MODEL_OBJECT_H__
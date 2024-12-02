#pragma once

#include <memory>
#include <vector>
#include <DirectXMath.h>

#include "Model.h"
#include "TAKOEngine/Rendering/Model/ModelResource.h"
#include "TAKOEngine/Rendering/Constant.h"

// モデル
class ModelDX12 : public iModel
{
public:
	ModelDX12(ID3D12Device* device, const char* filename, float scaling = 1.0f, int modelType = 1);
	~ModelDX12() override;

	// 行列計算
	void UpdateTransform(const DirectX::XMFLOAT4X4& worldTransform) override;

	// フレームリソース更新処理
	void UpdateFrameResource(const DirectX::XMFLOAT4X4 transform);

	//デバッグ情報
	void DrawDebugGUI() override;

	// アニメーション----------------------------------------------------------------------
	// アニメーション再生中か
	bool IsPlayAnimation() const override;
	
	// アニメーション再生
	void PlayAnimation(int animationIndex, bool loop = false, float blendSeconds = 0.2f) override;
	
	// アニメーション停止
	void StopAnimation();

	// アニメーション更新処理
	void UpdateAnimation(float elapsedTime) override;

	// アニメーション計算処理
	void ComputeAnimation(float elapsedTime) override;

	// ブレンディング計算処理
	void ComputeBlending(float elapsedTime) override;

	// アニメーションコピー
	void CopyAnimations(iModel* model) override;

	// アニメーション取得
	int GetCurrentAnimationIndex() const override { return m_current_animation_Index; }

	// 現在のアニメーション再生時間取得
	float GetCurrentAnimationSeconds() const override { return m_currentAnimationBlendSeconds; }

	float GetAnimationRate() const override { return m_currentAnimationBlendSeconds / m_resource->GetAnimations().at(m_current_animation_Index).secondsLength;}

	void SetAnimationRate(float rate) override { m_currentAnimationBlendSeconds = m_resource->GetAnimations().at(m_current_animation_Index).secondsLength * rate; }

	//-------------------------------------------------------------------------------------
	// バウンディングボックス計算
	void ComputeWorldBounds();

	// ノードリスト取得
	const std::vector<Node>& GetNodes() const override { return m_nodes; }

	// ルートノード取得
	Node* GetRootNode() override { return m_nodes.data(); }

	// ノード検索
	Node* FindNode(const char* name) override;

	// ノードコピー
	void CopyNodes(iModel* model) override;

	// メッシュリスト取得
	const std::vector<Mesh>& GetMeshes() const override { return m_meshes; }

	// リソース取得
	const ModelResource* GetResource() const override { return m_resource.get(); }

	void SetLinearGamma(float g) { linearGamma = g; }
	float GetLinearGamma() const { return linearGamma; }

	// インスタンシング用関数----------------------------------
	//割り当てられた番号を返す
	int AllocateInstancingIndex();

	//割り当てられた番号を解放する
	void FreeInstancingIndex(int instancingIndex);

	//行列計算
	void UpdateTransform(int instancingIndex, const DirectX::XMFLOAT4X4& transform);

	//現在の姿勢行列を取得
	const DirectX::XMFLOAT4X4& GetTransform(int instancingIndex) const;

	float scaling = 1.0f;

private:
	// インスタンシング用
	bool exist[InstancingMax] = {};
	DirectX::XMFLOAT4X4 m_transform[InstancingMax];

	int modelType = 0;
};

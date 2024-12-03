#pragma once

#include <memory>
#include <vector>
#include <DirectXMath.h>
#include <wrl.h>
#include <d3d11.h>

#include "Model.h"
#include "ModelResource.h"

class ModelDX11 : public iModel
{
public:
	ModelDX11(ID3D11Device* device, const char* filename, float scaling = 1.0f, int modelType = 0);
	~ModelDX11() override;

	// ノードデータ取得
	const std::vector<Node>& GetNodes() const override { return nodes; }

	// メッシュ取得
	const std::vector<Mesh>& GetMeshes() const override { return m_meshes; }

	// トランスフォーム更新処理
	void UpdateTransform(const DirectX::XMFLOAT4X4& worldTransform) override;

	// ルートノード取得
	Node* GetRootNode() override { return nodes.data(); }

	// ノード検索
	Node* FindNode(const char* name) override;

	// リソース取得
	const ModelResource* GetResource() const override { return resource.get(); }

	// アニメーション再生
	void PlayAnimation(int index, bool loop, float blendSeconds = 0.2f) override;

	// アニメーション再生中か
	bool IsPlayAnimation() const override;

	// アニメーション更新処理
	void UpdateAnimation(float elapsedTime) override;

	// アニメーション取得
	int GetAnimationId() { return currentAnimationIndex; }

	void CopyAnimations(iModel* model) override;
	void CopyNodes(iModel* model) override;

	int GetCurrentAnimationIndex() const override { return  currentAnimationIndex; }

	// 現在のアニメーション再生時間取得
	float GetCurrentAnimationSeconds() const override { return  currentAnimationSeconds; }
	float GetAnimationRate() const override { return currentAnimationSeconds / resource->GetAnimations().at(currentAnimationIndex).secondsLength; }
	void SetAnimationRate(float rate) override { currentAnimationSeconds = resource->GetAnimations().at(currentAnimationIndex).secondsLength * rate; }

	void SetLinearGamma(float g) { linearGamma = g; }
	float GetLinearGamma() const { return linearGamma; }

	//デバッグ情報
	void DrawDebugGUI() override;

	// アニメーション計算処理
	void ComputeAnimation(float elapsedTime) override;

	// ブレンディング計算処理
	void ComputeBlending(float elapsedTime) override;

	// バウンディングボックス計算
	void ComputeWorldBounds() override;

	float scaling = 1.0f;
};
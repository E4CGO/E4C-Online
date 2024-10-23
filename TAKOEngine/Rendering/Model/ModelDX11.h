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
	ModelDX11(ID3D11Device* device, const char* filename, float scaling = 1.0f);
	~ModelDX11();

	// ノードデータ取得
	const std::vector<Node>& GetNodes() const { return nodes; }

	// メッシュ取得
	const std::vector<Mesh>& GetMeshes() const { return m_meshes; }

	// トランスフォーム更新処理
	void UpdateTransform(const DirectX::XMFLOAT4X4& worldTransform);

	// ルートノード取得
	Node* GetRootNode() { return nodes.data(); }

	// ノード検索
	Node* FindNode(const char* name);

	// リソース取得
	const ModelResource* GetResource() const { return resource.get(); }

	// アニメーション再生
	void PlayAnimation(int index, bool loop, float blendSeconds = 0.2f);

	// アニメーション再生中か
	bool IsPlayAnimation() const;

	// アニメーション更新処理
	void UpdateAnimation(float elapsedTime);

	// アニメーション取得
	int GetAnimationId() { return currentAnimationIndex; }

	void CopyAnimations(iModel* model);
	void CopyNodes(iModel* model);

	int GetCurrentAnimationIndex() const { return  currentAnimationIndex; }

	// 現在のアニメーション再生時間取得
	float GetCurrentAnimationSeconds() const { return  currentAnimationSeconds; }
	float GetAnimationRate() const { return currentAnimationSeconds / resource->GetAnimations().at(currentAnimationIndex).secondsLength; }
	void SetAnimationRate(float rate) { currentAnimationSeconds = resource->GetAnimations().at(currentAnimationIndex).secondsLength * rate; }

	void SetLinearGamma(float g) { linearGamma = g; }
	float GetLinearGamma() const { return linearGamma; }

	const std::vector<Node>& GetNodesDX12() const { return m_nodes; }
	std::vector<Node>& GetNodesDX12() { return m_nodes; }

	// メッシュリスト取得
	std::vector<Mesh>& GetMeshes() { return m_meshes; }

	// リソース取得
	const ModelResource* GetResourceDX12() const { return m_resource.get(); }

	//デバッグ情報
	void DrawDebugGUI();

	// アニメーション計算処理
	void ComputeAnimation(float elapsedTime);

	// ブレンディング計算処理
	void ComputeBlending(float elapsedTime);

	// バウンディングボックス計算
	void ComputeWorldBounds();

	float scaling = 1.0f;
};
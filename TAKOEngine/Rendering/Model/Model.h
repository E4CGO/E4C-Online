#pragma once

#include <memory>
#include <vector>
#include <DirectXMath.h>
#include <wrl.h>
#include <d3d11.h>

#include "ModelResource.h"

class iModel
{
public:
	iModel() = default;
	iModel(ID3D11Device* device, const char* filename, float scaling = 1.0f, int modelType = 0) {};
	virtual ~iModel() {};

	struct Node
	{
		std::string name;
		std::string path;
		int parentIndex;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 rotation;
		DirectX::XMFLOAT3 scale;

		Node* parent = nullptr;
		std::vector<Node*> children;

		DirectX::XMFLOAT4X4 localTransform;
		DirectX::XMFLOAT4X4 globalTransform;
		DirectX::XMFLOAT4X4 worldTransform;

		bool visible = true;
	};

	struct FrameResource
	{
		Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_cbv_resource;
		Descriptor* cbv_descriptor = nullptr;
		DirectX::XMFLOAT4X4* cbv_data = nullptr;
	};

	struct Mesh
	{
		const ModelResource::Mesh* mesh = nullptr;
		DirectX::BoundingBox	   worldBounds;
		std::vector<FrameResource> frame_resources;
	};

	// ノードデータ取得
	const std::vector<Node>& GetNodes() const { return nodes; }

	// メッシュ取得
	const std::vector<Mesh>& GetMeshes() const { return m_meshes; }

	// トランスフォーム更新処理
	virtual void UpdateTransform(const DirectX::XMFLOAT4X4& worldTransform) = 0;

	// ルートノード取得
	Node* GetRootNode() { return nodes.data(); }

	// ノード検索
	virtual Node* FindNode(const char* name) = 0;

	// リソース取得
	const ModelResource* GetResource() const { return resource.get(); }

	// アニメーション再生
	virtual void PlayAnimation(int index, bool loop, float blendSeconds = 0.2f) = 0;

	// アニメーション再生中か
	virtual bool IsPlayAnimation() const = 0;

	// アニメーション更新処理
	virtual void UpdateAnimation(float elapsedTime) = 0;

	// アニメーション取得
	int GetAnimationId() { return currentAnimationIndex; }

	virtual void CopyAnimations(iModel* model) = 0;
	virtual void CopyNodes(iModel* model) = 0;

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
	virtual void DrawDebugGUI() = 0;

	// アニメーション計算処理
	virtual void ComputeAnimation(float elapsedTime) = 0;

	// ブレンディング計算処理
	virtual void ComputeBlending(float elapsedTime) = 0;

	// バウンディングボックス計算
	virtual void ComputeWorldBounds() = 0;

protected:
	std::shared_ptr<ModelResource>	resource;
	std::vector<Node>				nodes;
	DirectX::BoundingBox	        bounds;

	int   currentAnimationIndex = -1;
	float currentAnimationSeconds = 0.0f;
	bool animationPlaying = false;
	bool animationLoop = false;

	struct NodeCache
	{
		DirectX::XMFLOAT3 position = { 0, 0, 0 };
		DirectX::XMFLOAT4 rotation = { 0, 0, 0, 0 };
		DirectX::XMFLOAT3 scale = { 1, 1, 1 };
	};
	std::vector<NodeCache> nodeCaches;

	float currentAnimationBlendSeconds = 0.0f;
	float animationBlendSecondsLength = -1.0f;
	bool animationBlending = false;

	float scaling = 1.0f;

	float linearGamma = 1.0f;

	std::shared_ptr<ModelResource>			m_resource;
	std::vector<Node>						m_nodes;
	std::vector<Mesh>						m_meshes;

	int								m_current_animation = -1;
	float							m_current_seconds = 0.0f;
	bool							m_loop_animation = false;
	bool							m_end_animation = false;
};
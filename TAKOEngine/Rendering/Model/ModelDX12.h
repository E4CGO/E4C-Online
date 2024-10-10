#pragma once

#include <memory>
#include <vector>
#include <DirectXMath.h>

#include "TAKOEngine/Rendering/Model/ModelResource.h"
#include "TAKOEngine/Rendering/Constant.h"

// モデル
class ModelDX12 //: public iModel
{
public:
	ModelDX12(const char* filename);
	~ModelDX12();

	struct Node
	{
		const char* name;
		Node* parent;
		DirectX::XMFLOAT3	scale;
		DirectX::XMFLOAT4	rotate;
		DirectX::XMFLOAT3	translate;
		DirectX::XMFLOAT4X4	local_transform;
		DirectX::XMFLOAT4X4	global_transform;

		std::vector<Node*>	children;
	};

	struct Bone
	{
		DirectX::XMFLOAT4X4	offset_transform;
		Node* node = nullptr;
	};

	struct Mesh
	{
		struct Constants
		{
			DirectX::XMFLOAT4X4	world_transform;
			DirectX::XMFLOAT4X4	bone_transforms[255];
		};

		struct FrameResource
		{
			Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_vbv_uav_resource;
			D3D12_VERTEX_BUFFER_VIEW				d3d_vbv;
			const Descriptor* uav_descriptor = nullptr;

			Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_cbv_resource;
			const Descriptor* cbv_descriptor = nullptr;
			Constants* cbv_data = nullptr;
		};
		std::vector<FrameResource>	frame_resources;

		const ModelResource::Mesh* mesh = nullptr;

		std::vector<Bone>	bones;
		Node* node = nullptr;
		UINT				vertex_count;
	};

	// 行列計算
	void UpdateTransform();

	// フレームリソース更新処理
	void UpdateFrameResource();

	// アニメーション
	bool IsPlayAnimation() const { return m_current_animation >= 0; }
	void PlayAnimation(int animationIndex, bool loop = false);
	void StopAnimation();
	void UpdateAnimation(float elapsedTime);

	void ComputeAnimation(float elapsedTime);
	void ComputeBlending(float elapsedTime);
	void ComputeWorldBounds();

	// ノードリスト取得
	const std::vector<Node>& GetNodes() const { return m_nodes; }
	std::vector<Node>& GetNodes() { return m_nodes; }

	// メッシュリスト取得
	std::vector<Mesh>& GetMeshes() { return m_meshes; }

	// リソース取得
	const ModelResource* GetResource() const { return m_resource.get(); }

	void SetTransformMatrix(DirectX::XMFLOAT4X4 transform) { this->transform = transform; }

private:
	std::shared_ptr<ModelResource>			m_resource;
	std::vector<Node>						m_nodes;
	std::vector<Mesh>						m_meshes;

	int								m_current_animation = -1;
	float							m_current_seconds = 0.0f;
	bool							m_loop_animation = false;
	bool							m_end_animation = false;
	
	DirectX::XMFLOAT4X4 transform;
};

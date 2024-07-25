#pragma once

#include <memory>
#include <vector>
#include <DirectXMath.h>
#include <wrl.h>
#include <d3d11.h>
#include "ModelResource.h"

class Model
{
public:
	Model(ID3D11Device* device, const char* filename, float scaling = 1.0f);
	~Model();

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

	// �m�[�h�f�[�^�擾
	const std::vector<Node>& GetNodes() const { return nodes; }

	// ���b�V���擾
	const std::vector<Mesh>&GetMeshes() const { return m_meshes; }

	// �g�����X�t�H�[���X�V����
	void UpdateTransform(const DirectX::XMFLOAT4X4& worldTransform);

	// ���[�g�m�[�h�擾
	Node* GetRootNode() { return nodes.data(); }
	
	// �m�[�h����
	Node* FindNode(const char* name);

	// ���\�[�X�擾
	const ModelResource* GetResource() const { return resource.get(); }

	// �A�j���[�V�����Đ�
	void PlayAnimation(int index, bool loop, float blendSeconds = 0.2f);

	// �A�j���[�V�����Đ�����
	bool IsPlayAnimation() const;

	// �A�j���[�V�����X�V����
	void UpdateAnimation(float elapsedTime);

	// �A�j���[�V�����擾
	int GetAnimationId() { return currentAnimationIndex; }

	int GetCurrentAnimationIndex() const { return  currentAnimationIndex; }
	
	// ���݂̃A�j���[�V�����Đ����Ԏ擾
	float GetCurrentAnimationSeconds() const { return  currentAnimationSeconds; }
	float GetAnimationRate() const { return currentAnimationSeconds / resource->GetAnimations().at(currentAnimationIndex).secondsLength; }
	void SetAnimationRate(float rate) { currentAnimationSeconds = resource->GetAnimations().at(currentAnimationIndex).secondsLength * rate; }

	void SetLinearGamma(float g) { linearGamma = g; }
	float GetLinearGamma() const { return linearGamma; }

	const std::vector<Node>& GetNodesDX12() const { return m_nodes; }
	std::vector<Node>& GetNodesDX12() { return m_nodes; }

	// ���b�V�����X�g�擾
	std::vector<Mesh>& GetMeshes() { return m_meshes; }

	// ���\�[�X�擾
	const ModelResource* GetResourceDX12() const { return m_resource.get(); }

	//�f�o�b�O���
	void DrawDebugGUI();

private:
	// �A�j���[�V�����v�Z����
	void ComputeAnimation(float elapsedTime);
	
	// �u�����f�B���O�v�Z����
	void ComputeBlending(float elapsedTime);

	// �o�E���f�B���O�{�b�N�X�v�Z
	void ComputeWorldBounds();

private:
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
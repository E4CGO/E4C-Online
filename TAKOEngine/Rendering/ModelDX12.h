#pragma once

#include <memory>
#include <vector>
#include <DirectXMath.h>

#include "TAKOEngine/Rendering/ModelResource.h"

// ���f��
class ModelDX12
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
		DirectX::XMFLOAT4X4	world_transform;

		std::vector<Node*>	children;
	};

	struct FrameResource
	{
		Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_cbv_resource;
		Descriptor* cbv_descriptor = nullptr;
		DirectX::XMFLOAT4X4* cbv_data = nullptr;
	};

	struct Mesh
	{
		const ModelResource::Mesh* mesh;
		std::vector<FrameResource>				frame_resources;
	};

	// �s��v�Z
	void UpdateTransform(const DirectX::XMFLOAT4X4& transform);

	// �A�j���[�V����
	bool IsPlayAnimation() const { return m_current_animation >= 0; }
	void PlayAnimation(int animationIndex, bool loop = false);
	void StopAnimation();
	void UpdateAnimation(float elapsedTime);

	// �m�[�h���X�g�擾
	const std::vector<Node>& GetNodes() const { return m_nodes; }
	std::vector<Node>& GetNodes() { return m_nodes; }

	// ���b�V�����X�g�擾
	std::vector<Mesh>& GetMeshes() { return m_meshes; }

	// ���\�[�X�擾
	const ModelResource* GetResource() const { return m_resource.get(); }

private:
	std::shared_ptr<ModelResource>			m_resource;
	std::vector<Node>						m_nodes;
	std::vector<Mesh>						m_meshes;

	int								m_current_animation = -1;
	float							m_current_seconds = 0.0f;
	bool							m_loop_animation = false;
	bool							m_end_animation = false;
};

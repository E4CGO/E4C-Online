#pragma once

#include <map>

#define NOMINMAX
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "External/tinygltf/tiny_gltf.h"

#include "TAKOEngine/Rendering/RenderContext.h"
#include "TAKOEngine/Rendering/Model/Model.h"

class GLTFImporter
{
	std::string filename;
public:

	struct  buffer_view
	{
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
		size_t stride_in_bytes{ 0 };
		size_t size_in_bytes{ 0 };
		size_t count() const
		{
			return size_in_bytes / stride_in_bytes;
		}
	};

	GLTFImporter(ID3D11Device* device, const std::string& filename);
	~GLTFImporter();

	void fetch_nodes(const tinygltf::Model& gltf_model);
	void cumulate_transforms(std::vector<ModelResource::Node>& nodes);

	buffer_view make_buffer_view(const tinygltf::Accessor& accessor);
	void fetch_meshes(ID3D11Device* device, const tinygltf::Model& gltf_model);

	void fetch_materials(ID3D11Device* device, const tinygltf::Model& gltf_model);

	void fetch_textures(ID3D11Device* device, const tinygltf::Model& gltf_model);

	void fetch_animations(const tinygltf::Model& gltf_model);
	void animate(size_t animation_index, float time, std::vector<ModelResource::Node>& animated_nodes);

	void render(const RenderContext& rc, const DirectX::XMFLOAT4X4 world, const std::vector<ModelResource::Node>& animated_nodes);

	void UpdateTransform(const DirectX::XMFLOAT4X4& worldTransform);
	void PlayAnimation(int index, bool loop, float blendSeconds = 0.2f);
	bool IsPlayAnimation() const;
	void UpdateAnimation(float elapsedTime);
	void ComputeAnimation(float elapsedTime);
	void ComputeBlending(float elapsedTime);
	void ComputeWorldBounds();

	//////

	//// ノードデータ取得
	//const std::vector<ModelResource::Node>& GetNodes() const { return nodes; }
	//const std::vector<ModelResource::Node>& GetLocalNodes() const { return this->nodes; }

	//std::vector<animation>& GetAnimations() { return animations; }

	//// メッシュ取得
	//const std::vector<Mesh>& GetMeshes() const { return m_meshes; }

	//// ルートノード取得
	//ModelResource::Node* GetRootNode() { return m_nodes.data(); }

	//// ノード検索
	//ModelResource::Node* FindNode(const char* name);

	//// リソース取得
	//const ModelResource* GetResource() const { return resource.get(); }

	//// アニメーション取得
	//int GetAnimationId() { return currentAnimationIndex; }

	//int GetCurrentAnimationIndex() const { return  currentAnimationIndex; }

	//// 現在のアニメーション再生時間取得
	//float GetCurrentAnimationSeconds() const { return  currentAnimationSeconds; }
	//float GetAnimationRate() const { return currentAnimationSeconds / resource->GetAnimations().at(currentAnimationIndex).secondsLength; }
	//void SetAnimationRate(float rate) { currentAnimationSeconds = resource->GetAnimations().at(currentAnimationIndex).secondsLength * rate; }

	//void SetLinearGamma(float g) { linearGamma = g; }
	//float GetLinearGamma() const { return linearGamma; }

	//const std::vector<ModelResource::Node>& GetNodesDX12() const { return m_nodes; }
	//std::vector<ModelResource::Node>& GetNodesDX12() { return m_nodes; }

	//// メッシュリスト取得
	//std::vector<Mesh>& GetMeshes() { return m_meshes; }

	//// リソース取得
	//const ModelResource* GetResourceDX12() const { return m_resource.get(); }

	//デバッグ情報
	void DrawDebugGUI();
};
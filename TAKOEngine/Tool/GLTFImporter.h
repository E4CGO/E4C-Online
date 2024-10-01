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

class gltf_model : public iModel
{
	std::string filename;
public:
	gltf_model(ID3D11Device* device, const std::string& filename, float scaling = 1.0f);
	~gltf_model();

	struct scene
	{
		std::string name;
		std::vector<int> nodes;
	};
	std::vector<scene> scenes;

	std::vector<Node> nodes;

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
	struct mesh
	{
		std::string name;
		struct primitive
		{
			int material;
			std::map<std::string, buffer_view> vertex_buffer_views;
			buffer_view index_buffer_view;
		};
		std::vector<primitive> primitives;
	};
	std::vector<mesh> meshes;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;

	struct primitive_constants
	{
		DirectX::XMFLOAT4X4 world;
		int material{ -1 };
		int has_tangent{ 0 };
		int skin{ -1 };
		int pad;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> primitive_cbuffer;

	struct texture_info
	{
		int index = -1;
		int texcoord = 0;
	};

	struct normal_texture_info
	{
		int index = -1;
		int texcoord = 0;
		float scale = 1;
	};

	struct occlusion_texture_info
	{
		int index = -1;
		int texcoord = 0;
		float strength = 1;
	};

	struct pbr_metallic_roughness
	{
		float basecolor_factor[4]{ 1,1,1,1 };
		texture_info basecolor_texture;
		float metallic_factor = 1;
		float roughness_factor = 1;
		texture_info metallic_roughness_texture;
	};

	struct material
	{
		std::string name;
		struct cbuffer
		{
			float emissive_factor[3]{ 0, 0, 0 };
			int alpha_mode = 0; // "OPAQUE" : 0, "MASK" : 1, "BLEND" : 2
			float alpha_cutoff = 0.5f;
			bool double_sided = false;

			pbr_metallic_roughness pbr_metallic_roughness;

			normal_texture_info normal_texture;
			occlusion_texture_info occlusion_texture;
			texture_info emissive_texture;
		};
		cbuffer data;
	};
	std::vector<material> materials;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> material_resource_view;

	struct texture
	{
		std::string name;
		int source{ -1 };
	};
	std::vector<texture> textures;

	struct image
	{
		std::string name;
		int width{ -1 };
		int height{ -1 };
		int component{ -1 };
		int bits{ -1 };
		int pixel_type{ -1 };
		int buffer_view;
		std::string mime_type;
		std::string uri;
		bool as_is{ false };
	};
	std::vector<image> images;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> texture_resource_views;

	struct skin
	{
		std::vector<DirectX::XMFLOAT4X4> inverse_bind_matrices;
		std::vector<int> joints;
	};
	std::vector<skin> skins;

	std::vector<animation> animations;

	static const size_t PRIMITIVE_MAX_JOINTS = 512;
	struct primitive_joint_constants
	{
		DirectX::XMFLOAT4X4 matrices[PRIMITIVE_MAX_JOINTS];
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> primitive_joint_cbuffer;

	void fetch_nodes(const tinygltf::Model& gltf_model);
	void cumulate_transforms(std::vector<Node>& nodes);

	buffer_view make_buffer_view(const tinygltf::Accessor& accessor);
	void fetch_meshes(ID3D11Device* device, const tinygltf::Model& gltf_model);

	void fetch_materials(ID3D11Device* device, const tinygltf::Model& gltf_model);

	void fetch_textures(ID3D11Device* device, const tinygltf::Model& gltf_model);

	void fetch_animations(const tinygltf::Model& gltf_model);
	void animate(size_t animation_index, float time, std::vector<Node>& animated_nodes);

	void render(const RenderContext& rc, const DirectX::XMFLOAT4X4 world, const std::vector<Node>& animated_nodes);
	void renderDX12(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4 world, const std::vector<Node>& animated_nodes);

	void UpdateTransform(const DirectX::XMFLOAT4X4& worldTransform);
	void PlayAnimation(int index, bool loop, float blendSeconds = 0.2f);
	bool IsPlayAnimation() const;
	void UpdateAnimation(float elapsedTime);
	void ComputeAnimation(float elapsedTime);
	void ComputeBlending(float elapsedTime);
	void ComputeWorldBounds();

	//////

	// ノードデータ取得
	const std::vector<Node>& GetNodes() const { return nodes; }
	const std::vector<Node>& GetLocalNodes() const { return this->nodes; }

	std::vector<animation>& GetAnimations() { return animations; }

	// メッシュ取得
	const std::vector<Mesh>& GetMeshes() const { return m_meshes; }

	// ルートノード取得
	Node* GetRootNode() { return m_nodes.data(); }

	// ノード検索
	Node* FindNode(const char* name);

	// リソース取得
	const ModelResource* GetResource() const { return resource.get(); }

	// アニメーション取得
	int GetAnimationId() { return currentAnimationIndex; }

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
};
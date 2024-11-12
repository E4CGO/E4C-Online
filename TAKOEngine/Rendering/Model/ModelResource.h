#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <wrl.h>
#include <d3d11.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "TAKOEngine/Rendering/Descriptor.h"

class ModelResource
{
public:
	ModelResource() {}
	~ModelResource();

	using NodeId = UINT64;

	struct Node
	{
		NodeId id;
		std::string name;
		std::string path;
		int parentIndex;

		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 rotation;
		DirectX::XMFLOAT3 scale;

		template<class Archive>
		void serialize(Archive& archive);
	};

	struct Material
	{
		std::string name;
		std::string	textureFilename;
		std::string diffuseTextureFileName;
		std::string normalTextureFileName;

		DirectX::XMFLOAT4 color = { 1, 1, 1, 1 };
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseMap;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMap;

		Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_cbv_resource;
		Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_srv_resource;
		Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_normal_srv_resource;

		const Descriptor* cbv_descriptor = nullptr;
		const Descriptor* srv_descriptor = nullptr;
		const Descriptor* srv_normal_descriptor = nullptr;

		template<class Archive>
		void serialize(Archive& archive);
	};

	struct Subset
	{
		UINT		startIndex = 0;
		UINT		indexCount = 0;
		int			materialIndex = 0;

		Material* material = nullptr;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};

	struct Vertex
	{
		DirectX::XMFLOAT3 position = { 0, 0, 0 };
		DirectX::XMFLOAT4 boneWeight = { 1, 0, 0, 0 };
		DirectX::XMUINT4  boneIndex = { 0, 0, 0, 0 };
		DirectX::XMFLOAT2 texcoord = { 0, 0 };
		DirectX::XMFLOAT4 color = { 1, 1, 1, 1 };
		DirectX::XMFLOAT3 normal = { 0, 0, 0 };
		DirectX::XMFLOAT3 tangent = { 0, 0, 0 };

		template<class Archive>
		void serialize(Archive& archive);
	};

	struct Bone
	{
		int nodeIndex;
		DirectX::XMFLOAT4X4 offsetTransform;
		Node* node = nullptr;

		template<class Archive>
		void serialize(Archive& archive);
	};

	struct Mesh
	{
		std::vector<Vertex>						vertices;
		std::vector<uint32_t>					indices;
		std::vector<Subset>						subsets;

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

		int materialIndex = 0;
		Material* material = nullptr;

		int                                     nodeIndex = 0;
		Node* node = nullptr;
		std::vector<int>						nodeIndices;
		std::vector<DirectX::XMFLOAT4X4>		offsetTransforms;

		std::vector<Bone> bones;

		DirectX::BoundingBox	localBounds;
		DirectX::XMFLOAT3		boundsMin;
		DirectX::XMFLOAT3		boundsMax;

		Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_vb_resource;
		Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_ib_resource;
		Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_cb_resource;
		Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_toon_srv_resource;

		const Descriptor* srv_descriptor = nullptr;
		const Descriptor* srv_toon_descriptor = nullptr;

		D3D12_VERTEX_BUFFER_VIEW				d3d_vbv;
		D3D12_INDEX_BUFFER_VIEW					d3d_ibv;

		template<class Archive>
		void serialize(Archive& archive);
	};

	struct NodeKeyData
	{
		DirectX::XMFLOAT3	scale;
		DirectX::XMFLOAT4	rotate;
		DirectX::XMFLOAT3	translate;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};

	struct Keyframe
	{
		float						seconds;
		std::vector<NodeKeyData>	nodeKeys;

		template<class Archive>
		void serialize(Archive& archive, int version);
	};

	struct VectorKeyframe
	{
		float seconds;
		DirectX::XMFLOAT3 value;

		template<class Archive>
		void serialize(Archive& archive);
	};
	struct QuaternionKeyframe
	{
		float seconds;
		DirectX::XMFLOAT4 value;

		template<class Archive>
		void serialize(Archive& archive);
	};
	struct NodeAnim
	{
		std::vector <VectorKeyframe> positionKeyframes;
		std::vector<QuaternionKeyframe> rotationKeyframes;
		std::vector<VectorKeyframe> scaleKeyframes;

		template<class Archive>
		void serialize(Archive& archive);
	};
	struct Animation
	{
		std::string name;
		float secondsLength;
		std::vector<NodeAnim> nodeAnims;
		std::vector<Keyframe>		keyframes;

		template<class Archive>
		void serialize(Archive& archive);
	};

	//////////////////////////////////////////////
	///
	///
	///
	///
	struct scene
	{
		std::string name;
		std::vector<int> nodes;
	};

	struct node
	{
		std::string name;
		int skin{ -1 };
		int mesh{ -1 };

		std::vector<int> children;

		DirectX::XMFLOAT4 rotation{ 0, 0, 0, 1 };
		DirectX::XMFLOAT3 scale{ 1, 1, 1 };
		DirectX::XMFLOAT3 translation{ 0, 0, 0 };

		DirectX::XMFLOAT4X4 global_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	};

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

	struct primitive_constants
	{
		DirectX::XMFLOAT4X4 world;
		int material{ -1 };
		int has_tangent{ 0 };
		int skin{ -1 };
		int pad;
	};

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

	struct texture
	{
		std::string name;
		int source{ -1 };
	};

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

	struct skin
	{
		std::vector<DirectX::XMFLOAT4X4> inverse_bind_matrices;
		std::vector<int> joints;
	};

	struct animation
	{
		std::string name;
		float duration{ 0.0f };

		struct channel
		{
			int sampler{ -1 };
			int target_node{ -1 };
			std::string target_path;
		};
		std::vector<channel> channels;

		struct sampler
		{
			int input{ -1 };
			int output{ -1 };
			std::string interpolation;
		};
		std::vector<sampler> samplers;

		std::unordered_map<int/*sampler.input*/, std::vector<float>> timelines;
		std::unordered_map<int/*sampler.output*/, std::vector<DirectX::XMFLOAT3>> scales;
		std::unordered_map<int/*sampler.output*/, std::vector<DirectX::XMFLOAT4>> rotations;
		std::unordered_map<int/*sampler.output*/, std::vector<DirectX::XMFLOAT3>> translations;
	};

	static const size_t PRIMITIVE_MAX_JOINTS = 512;
	struct primitive_joint_constants
	{
		DirectX::XMFLOAT4X4 matrices[PRIMITIVE_MAX_JOINTS];
	};

	///
	///
	///
	///
	//////////////////////////////////////////////

	// 各種データ取得
	const std::vector<Mesh>& GetMeshes() const { return meshes; }
	const std::vector<Node>& GetNodes() const { return nodes; }
	void SetNodes(std::vector<Node> nodes) { this->nodes = nodes; }
	const std::vector<Animation>& GetAnimations() const { return animations; }
	void SetAnimations(std::vector<Animation> animations) { this->animations = animations; }
	const std::vector<Material>& GetMaterials() const { return materials; }

	//////////////////////////////////////////////
	///
	///
	///
	///

	const std::vector<scene>& GetNewScenes() const { return newScenes; }
	const std::vector<node>& GetNewNodes() const { return newNodes; }
	const std::vector<node>& GetNewAnimationNodes() const { return newAnimationNodes; }
	const std::vector<mesh>& GetNewMeshes() const { return newMeshes; }
	const std::vector<material>& GetNewMaterials() const { return newMaterials; }
	const std::vector<texture>& GetNewTextures() const { return newTextures; }
	const std::vector<image>& GetNewImages() const { return newImages; }
	const std::vector<skin>& GetNewSkins() const { return newSkins; }
	const std::vector<animation>& GetNewAnimations() const { return newAnimations; }

	const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetNewMateriaslRV() const { return newMaterialsRV; }
	const std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& GetNewTexturesRV() const { return newTextureRV; }

	///
	///
	///
	///
	//////////////////////////////////////////////

	// 読み込み
	void Load(ID3D11Device* device, const char* filename);
	void Load(const char* filename);
	void LoadGLTF(ID3D11Device* device, std::string filename);

protected:

	void BuildModel(const char* dirname, const char* filename);

	// バウンディングボックス計算
	void ComputeLocalBounds();

	// シリアライズ
	void Serialize(const char* filename);

	// デシリアライズ
	void Deserialize(const char* filename);

	// ノードインデックスを取得する
	int FindNodeIndex(NodeId nodeId) const;
public:
	// コピーアニメーション
	void ImportAnimations(const char* filename);
protected:
	std::vector<Node>		nodes;
	std::vector<Material>	materials;
	std::vector<Mesh>		meshes;
	std::vector<Animation>	animations;

	//////////////////////////////////////////////
	///
	///
	///
	///

	std::vector<scene> newScenes;
	std::vector<node> newNodes;
	std::vector<mesh> newMeshes;
	std::vector<material> newMaterials;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> newMaterialsRV;
	std::vector<texture> newTextures;
	std::vector<image> newImages;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> newTextureRV;
	std::vector<skin> newSkins;
	std::vector<animation> newAnimations;
	std::vector<node> newAnimationNodes;

	///
	///
	///
	///
	//////////////////////////////////////////////
};
#pragma once

#include <string>
#include <vector>
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
		std::string			textureFilename;
		std::string diffuseTextureFileName;

		DirectX::XMFLOAT4 color = { 1, 1, 1, 1 };
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseMap;
		std::string normalTextureFileName;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMap;

		Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_cbv_resource;
		Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_srv_resource;
		Descriptor* cbv_descriptor = nullptr;
		Descriptor* srv_descriptor = nullptr;

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
		Node*                                   node = nullptr;
		std::vector<int>						nodeIndices;
		std::vector<DirectX::XMFLOAT4X4>		offsetTransforms;

		std::vector<Bone> bones;

		DirectX::BoundingBox	localBounds;
		DirectX::XMFLOAT3		boundsMin;
		DirectX::XMFLOAT3		boundsMax;

		Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_vb_resource;
		Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_ib_resource;
		Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_cb_resource;
		
		Descriptor* srv_descriptor = nullptr;

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

	// 各種データ取得
	const std::vector<Mesh>& GetMeshes() const { return meshes; }
	const std::vector<Node>& GetNodes() const { return nodes; }
	const std::vector<Animation>& GetAnimations() const { return animations; }
	const std::vector<Material>& GetMaterials() const { return materials; }

	// 読み込み
	void Load(ID3D11Device* device, const char* filename);
	void Load(const char* filename);

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
};
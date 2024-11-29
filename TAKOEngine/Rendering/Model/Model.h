#pragma once

#include <memory>
#include <vector>
#include <DirectXMath.h>
#include <wrl.h>
#include <d3d11.h>
#include <d3d12.h>

#include "TAKOEngine/Rendering/Constant.h"
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

			// インスタンシング用データ
			DirectX::XMFLOAT4X4 transform[InstancingMax] = {};  //姿勢行列
		};

		struct FrameResource
		{
			Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_vbv_uav_resource;
			D3D12_VERTEX_BUFFER_VIEW				d3d_vbv;
			const Descriptor* uav_descriptor = nullptr;

			Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_cbv_resource;
			const Descriptor* cbv_descriptor = nullptr;
			Constants* cbv_data = nullptr;

			int instancingCount = 0;	  //インスタンシング描画数
		};
		std::vector<FrameResource> frame_resources;

		const ModelResource::Mesh* mesh = nullptr;
		DirectX::BoundingBox	   worldBounds;

		std::vector<Bone>	bones;
		Node* node = nullptr;
		UINT  vertex_count;
	};

	//
	//

	std::vector<ModelResource::scene> scenes;
	std::vector<ModelResource::node> gltf_nodes;
	std::vector<ModelResource::mesh> meshes;

	std::vector<ModelResource::material> materials;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> material_resource_view;

	std::vector<ModelResource::texture> textures;

	std::vector<ModelResource::image> images;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> texture_resource_views;

	std::vector<ModelResource::skin> skins;

	std::vector<ModelResource::animation> animations;
	std::vector<ModelResource::node> animated_nodes;

	//
	//

	// ノードデータ取得
	virtual const std::vector<Node>& GetNodes() const = 0;

	// メッシュ取得
	virtual const std::vector<Mesh>& GetMeshes() const = 0;
	std::vector<Mesh>& GetMeshes() { return m_meshes; }

	const DirectX::XMFLOAT4X4 GetWorldMatrix() const { return m_WorldMatrix; }
	DirectX::XMFLOAT4X4 SetWorldMatrix(const DirectX::XMFLOAT4X4 world) { this->m_WorldMatrix = world; }

	// トランスフォーム更新処理
	virtual void UpdateTransform(const DirectX::XMFLOAT4X4& worldTransform) = 0;

	// ルートノード取得
	virtual Node* GetRootNode() = 0;

	// ノード検索
	virtual Node* FindNode(const char* name) = 0;

	// リソース取得
	virtual const ModelResource* GetResource() const = 0;

	// アニメーション再生
	virtual void PlayAnimation(int index, bool loop, float blendSeconds = 0.2f) = 0;

	// アニメーション再生中か
	virtual bool IsPlayAnimation() const = 0;

	// アニメーション更新処理
	virtual void UpdateAnimation(float elapsedTime) = 0;

	virtual void CopyAnimations(iModel* model) = 0;
	virtual void CopyNodes(iModel* model) = 0;

	// アニメーション取得
	virtual int GetCurrentAnimationIndex() const = 0;

	// 現在のアニメーション再生時間取得
	virtual float GetCurrentAnimationSeconds() const = 0;

	virtual float GetAnimationRate() const = 0;

	virtual void SetAnimationRate(float rate) = 0;

	// モデルの名前取得
	std::string GetModelName() { return name; }

	// モデルの名前抜き取り
	void extractBaseName(const char* filePath)
	{
		// ファイルパスが nullptr の場合は空文字列を代入
		if (!filePath)
		{
			name = "";
			return;
		}

		// 最後のスラッシュ（UNIX 系）を探す
		const char* lastSlashUnix = strrchr(filePath, '/');
		// 最後のバックスラッシュ（Windows 系）を探す
		const char* lastSlashWindows = strrchr(filePath, '\\');

		// スラッシュのどちらか最後に現れる方を選ぶ
		const char* lastSlash = nullptr;
		if (lastSlashUnix && lastSlashWindows)
		{
			lastSlash = (lastSlashUnix > lastSlashWindows) ? lastSlashUnix : lastSlashWindows;
		}
		else if (lastSlashUnix)
		{
			lastSlash = lastSlashUnix;
		}
		else if (lastSlashWindows)
		{
			lastSlash = lastSlashWindows;
		}

		// ファイル名部分を取得（スラッシュがない場合はそのまま全体）
		const char* fileName = (lastSlash != nullptr) ? lastSlash + 1 : filePath;

		// 最後のドットを見つける（拡張子の開始位置）
		const char* dot = strrchr(fileName, '.');
		size_t length = (dot != nullptr) ? static_cast<size_t>(dot - fileName) : strlen(fileName);

		// ファイル名部分を std::string にして代入
		name = std::string(fileName, length);
	};

	void SetLinearGamma(float g) { linearGamma = g; }
	float GetLinearGamma() const { return linearGamma; }

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

	DirectX::XMFLOAT4X4 m_WorldMatrix;

	// DX12
	std::shared_ptr<ModelResource>			m_resource;
	std::vector<Node>						m_nodes;
	std::vector<Mesh>						m_meshes;
	DirectX::BoundingBox	                m_bounds;

	int	  m_current_animation_Index = -1;
	float m_current_seconds = 0.0f;
	bool  m_playing_animation = false;
	bool  m_loop_animation = false;

	float m_animationBlendSecondsLength = -1.0f;
	float m_currentAnimationBlendSeconds = 0.0f;
	bool  m_animationBlending = false;
	std::string name = ""; // モデルの名前
};
//! @file GLTFModelDX11.h
//! @note

#ifndef __INCLUDED_GLTF_MODEL_DX11_H__
#define __INCLUDED_GLTF_MODEL_DX11_H__

#include <map>
#include <memory>
#include <vector>

#define NOMINMAX
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>

#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "tiny_gltf.h"

#include "TAKOEngine/Rendering/RenderContext.h"
#include "TAKOEngine/Rendering/Model/Model.h"

/**************************************************************************//**
	@class	GLTFModelDX11
	@brief	TinyGLTFベースモデル
	@par    [説明]
		TinyGLTFで読み込みでモデル作成
*//***************************************************************************/
class GLTFModelDX11 : public iModel
{
public:
	GLTFModelDX11(ID3D11Device* device, const std::string& filename, float scaling = 1.0f, int modelType = 0);
	~GLTFModelDX11() override {};

	void cumulate_transforms(std::vector<ModelResource::node>& nodes);

	ModelResource::buffer_view make_buffer_view(const tinygltf::Accessor& accessor);

	void animate(size_t animation_index, float time, std::vector<ModelResource::node>& animated_nodes);

	void UpdateTransform(const DirectX::XMFLOAT4X4& worldTransform) override;
	void PlayAnimation(int index, bool loop, float blendSeconds = 0.2f) override;
	bool IsPlayAnimation() const override; 
	void UpdateAnimation(float elapsedTime) override;
	void ComputeAnimation(float elapsedTime) override;
	void ComputeBlending(float elapsedTime) override;
	void ComputeWorldBounds() override;

	// ノードデータ取得
	const std::vector<Node>& GetNodes() const override { return nodes; }
	Node* FindNode(const char* name) override;

	// ルートノード取得
	Node* GetRootNode() override { return nodes.data(); }

	// メッシュ取得
	const std::vector<Mesh>& GetMeshes() const override { return m_meshes; }

	// リソース取得
	const ModelResource* GetResource() const override { return resource.get(); }

	int GetCurrentAnimationIndex() const override { return  currentAnimationIndex; }

	// 現在のアニメーション再生時間取得
	float GetCurrentAnimationSeconds() const override { return  currentAnimationSeconds; }
	float GetAnimationRate() const override { return currentAnimationSeconds / resource->GetAnimations().at(currentAnimationIndex).secondsLength; }
	void SetAnimationRate(float rate) override { currentAnimationSeconds = resource->GetAnimations().at(currentAnimationIndex).secondsLength * rate; }

	void CopyAnimations(iModel* model) override;
	void CopyNodes(iModel* model) override;

	void DrawDebugGUI() override;

private:

	std::string filename;
	float time = 0;

	float scaling = 1.0f;
	int modelType = 0;
};

#endif //!__INCLUDED_GLTF_MODEL_DX11_H__
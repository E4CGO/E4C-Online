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
	~GLTFModelDX11() {};

	void cumulate_transforms(std::vector<ModelResource::node>& nodes);

	ModelResource::buffer_view make_buffer_view(const tinygltf::Accessor& accessor);

	void animate(size_t animation_index, float time, std::vector<ModelResource::node>& animated_nodes);

	void UpdateTransform(const DirectX::XMFLOAT4X4& worldTransform);
	void PlayAnimation(int index, bool loop, float blendSeconds = 0.2f);
	bool IsPlayAnimation() const;
	void UpdateAnimation(float elapsedTime);
	void ComputeAnimation(float elapsedTime);
	void ComputeBlending(float elapsedTime);
	void ComputeWorldBounds();

	iModel::Node* FindNode(const char* name);

	void CopyAnimations(iModel* model);
	void CopyNodes(iModel* model);

	void DrawDebugGUI();

private:

	std::string filename;
	float time = 0;

	float scaling = 1.0f;
	int modelType = 0;
};

#endif //!__INCLUDED_GLTF_MODEL_DX11_H__
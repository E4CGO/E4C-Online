#pragma once

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

class NewModelDX11 : public iModel
{
public:
	NewModelDX11(ID3D11Device* device, const std::string& filename, float scaling);
	~NewModelDX11() {};

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;

	std::vector<ModelResource::scene> scenes;
	std::vector<ModelResource::node> nodes;
	std::vector<ModelResource::mesh> meshes;

	struct primitive_constants
	{
		DirectX::XMFLOAT4X4 world;
		int material{ -1 };
		int has_tangent{ 0 };
		int skin{ -1 };
		int pad;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> primitive_cbuffer;

	std::vector<ModelResource::material> materials;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> material_resource_view;

	std::vector<ModelResource::texture> textures;

	std::vector<ModelResource::image> images;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> texture_resource_views;

	std::vector<ModelResource::skin> skins;

	std::vector<ModelResource::animation> animations;
	std::vector<ModelResource::node> animated_nodes;

	static const size_t PRIMITIVE_MAX_JOINTS = 512;
	struct primitive_joint_constants
	{
		DirectX::XMFLOAT4X4 matrices[PRIMITIVE_MAX_JOINTS];
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> primitive_joint_cbuffer;

	Microsoft::WRL::ComPtr<ID3D11Buffer> scene_cbuffer;

	DirectX::XMFLOAT4X4 world;

	std::string filename;
	float time = 0;

	void cumulate_transforms(std::vector<ModelResource::node>& nodes);

	ModelResource::buffer_view make_buffer_view(const tinygltf::Accessor& accessor);

	void animate(size_t animation_index, float time, std::vector<ModelResource::node>& animated_nodes);

	void Update(float elapsedTime);

	void Render(const RenderContext& rc);

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
};

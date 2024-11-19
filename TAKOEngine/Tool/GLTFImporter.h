//! @file GLTFImporter.h
//! @note TinyGLTFでモデルの読み込み

#ifndef __INCLUDE_GLTF_IMPORTER_H__
#define __INCLUDE_GLTF_IMPORTER_H__

#define NOMINMAX

#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "External/tinygltf/tiny_gltf.h"

#include "TAKOEngine/Rendering/Model/ModelResource.h"

/**************************************************************************//**
	@class	GLTFImporter
	@brief	TinyGLTFでモデルの読み込み
	@par    [説明]
*//***************************************************************************/
class GLTFImporter
{
public:
	GLTFImporter() {};
	~GLTFImporter() {};

	static void fetch_nodes(const tinygltf::Model& gltf_model, std::vector<ModelResource::node>& model_nodes, std::vector<ModelResource::scene>& model_scenes);

	static void cumulate_transforms(std::vector<ModelResource::node>& model_nodes, std::vector<ModelResource::scene>& model_scenes);

	static ModelResource::buffer_view make_model_buffer_view(const tinygltf::Accessor& accessor);

	static void fetch_meshes(ID3D11Device* device, const tinygltf::Model& gltf_model, std::vector<ModelResource::mesh>& model_meshes);

	static void fetch_materials(ID3D11Device* device, const tinygltf::Model& gltf_model, std::vector<ModelResource::material>& model_materials, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& model_materialsRV);

	static void fetch_textures(ID3D11Device* device, const tinygltf::Model& gltf_model, std::vector<ModelResource::texture>& model_textures, std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& model_texturesRV, std::vector<ModelResource::image>& model_images, std::string filename);

	static void fetch_animations(const tinygltf::Model& gltf_model, std::vector<ModelResource::skin>& model_skins, std::vector<ModelResource::animation>& model_animations);
};

#endif // !__INCLUDE_GLTF_IMPORTER_H__

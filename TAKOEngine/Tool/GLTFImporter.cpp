#include "GLTFImporter.h"

#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"

#include <map>
#include <stack>
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/GpuResourceUtils.h"

/**************************************************************************//**
	@brief
	@param[in]    gltf_model
	@param[in]    model_nodes
	@param[in]    model_scenes
*//***************************************************************************/
void GLTFImporter::fetch_nodes(const tinygltf::Model& gltf_model, std::vector<ModelResource::node>& model_nodes, std::vector<ModelResource::scene>& model_scenes)
{
	for (std::vector<tinygltf::Node>::const_reference gltf_node : gltf_model.nodes)
	{
		ModelResource::node& node{ model_nodes.emplace_back() };
		node.name = gltf_node.name;
		node.skin = gltf_node.skin;
		node.mesh = gltf_node.mesh;
		node.children = gltf_node.children;

		if (!gltf_node.matrix.empty())
		{
			DirectX::XMFLOAT4X4 matrix;
			for (size_t row = 0; row < 4; row++)
			{
				for (size_t column = 0; column < 4; column++)
				{
					matrix(row, column) = static_cast<float>(gltf_node.matrix.at(4 * row + column));
				}
			}
			DirectX::XMVECTOR S, T, R;
			bool succeed = DirectX::XMMatrixDecompose(&S, &R, &T, DirectX::XMLoadFloat4x4(&matrix));
			_ASSERT_EXPR(succeed, L"Failed to decompose matrix");

			DirectX::XMStoreFloat3(&node.scale, S);
			DirectX::XMStoreFloat4(&node.rotation, R);
			DirectX::XMStoreFloat3(&node.translation, T);
		}
		else
		{
			if (gltf_node.scale.size() > 0)
			{
				node.scale.x = static_cast<float>(gltf_node.scale.at(0));
				node.scale.y = static_cast<float>(gltf_node.scale.at(1));
				node.scale.z = static_cast<float>(gltf_node.scale.at(2));
			}

			if (gltf_node.translation.size() > 0)
			{
				node.translation.x = static_cast<float>(gltf_node.translation.at(0));
				node.translation.y = static_cast<float>(gltf_node.translation.at(1));
				node.translation.z = static_cast<float>(gltf_node.translation.at(2));
			}

			if (gltf_node.rotation.size() > 0)
			{
				node.rotation.x = static_cast<float>(gltf_node.rotation.at(0));
				node.rotation.y = static_cast<float>(gltf_node.rotation.at(1));
				node.rotation.z = static_cast<float>(gltf_node.rotation.at(2));
				node.rotation.w = static_cast<float>(gltf_node.rotation.at(3));
			}
		}
	}

	cumulate_transforms(model_nodes, model_scenes);
}

/**************************************************************************//**
	@brief
	@param[in]    model_nodes
	@param[in]    model_scenes
*//***************************************************************************/
void GLTFImporter::cumulate_transforms(std::vector<ModelResource::node>& model_nodes, std::vector<ModelResource::scene>& model_scenes)
{
	std::stack<DirectX::XMFLOAT4X4> parent_global_transforms;
	std::function<void(int)> traverse{ [&](int node_index)->void
	{
			ModelResource::node& node{model_nodes.at(node_index)};
			DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z) };
			DirectX::XMMATRIX R{ DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(node.rotation.x, node.rotation.y, node.rotation.z, node.rotation.w)) };
			DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(node.translation.x, node.translation.y, node.translation.z) };
			DirectX::XMStoreFloat4x4(&node.global_transform , S * R * T * DirectX::XMLoadFloat4x4(&parent_global_transforms.top()));
			for (int child_index : node.children)
			{
				parent_global_transforms.push(node.global_transform);
				traverse(child_index);
				parent_global_transforms.pop();
			}
	} };
	for (std::vector<int>::value_type node_index : model_scenes.at(0).nodes)
	{
		parent_global_transforms.push({ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 });
		traverse(node_index);
		parent_global_transforms.pop();
	}
}

/**************************************************************************//**
	@brief
	@param[in]    accessor
	@return
*//***************************************************************************/
ModelResource::buffer_view GLTFImporter::make_model_buffer_view(const tinygltf::Accessor& accessor)
{
	ModelResource::buffer_view buffer_view;
	switch (accessor.type)
	{
	case TINYGLTF_TYPE_SCALAR:
		switch (accessor.componentType)
		{
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
			buffer_view.format = DXGI_FORMAT_R8_UINT;
			buffer_view.stride_in_bytes = sizeof(BYTE);
			break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			buffer_view.format = DXGI_FORMAT_R16_UINT;
			buffer_view.stride_in_bytes = sizeof(USHORT);
			break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
			buffer_view.format = DXGI_FORMAT_R32_UINT;
			buffer_view.stride_in_bytes = sizeof(UINT);
			break;
		default:
			_ASSERT_EXPR(FALSE, L"This accessor component type is not supported");
			break;
		}
		break;
	case TINYGLTF_TYPE_VEC2:
		switch (accessor.componentType)
		{
		case TINYGLTF_COMPONENT_TYPE_BYTE:
			buffer_view.format = DXGI_FORMAT_R8G8_SINT;
			buffer_view.stride_in_bytes = sizeof(CHAR) * 2;
			break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
			buffer_view.format = DXGI_FORMAT_R8G8_UINT;
			buffer_view.stride_in_bytes = sizeof(BYTE) * 2;
			break;
		case TINYGLTF_COMPONENT_TYPE_SHORT:
			buffer_view.format = DXGI_FORMAT_R16G16_SINT;
			buffer_view.stride_in_bytes = sizeof(SHORT) * 2;
			break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			buffer_view.format = DXGI_FORMAT_R16G16_UINT;
			buffer_view.stride_in_bytes = sizeof(USHORT) * 2;
			break;
		case TINYGLTF_COMPONENT_TYPE_INT:
			buffer_view.format = DXGI_FORMAT_R32G32_SINT;
			buffer_view.stride_in_bytes = sizeof(INT) * 2;
			break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
			buffer_view.format = DXGI_FORMAT_R32G32_UINT;
			buffer_view.stride_in_bytes = sizeof(UINT) * 2;
			break;
		case TINYGLTF_COMPONENT_TYPE_FLOAT:
			buffer_view.format = DXGI_FORMAT_R32G32_FLOAT;
			buffer_view.stride_in_bytes = sizeof(FLOAT) * 2;
			break;
		case TINYGLTF_COMPONENT_TYPE_DOUBLE:
		default:
			_ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
			break;
		}
		break;
	case TINYGLTF_TYPE_VEC3:
		switch (accessor.componentType)
		{
		case TINYGLTF_COMPONENT_TYPE_INT:
			buffer_view.format = DXGI_FORMAT_R32G32B32_SINT;
			buffer_view.stride_in_bytes = sizeof(INT) * 3;
			break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
			buffer_view.format = DXGI_FORMAT_R32G32B32_UINT;
			buffer_view.stride_in_bytes = sizeof(UINT) * 3;
			break;
		case TINYGLTF_COMPONENT_TYPE_FLOAT:
			buffer_view.format = DXGI_FORMAT_R32G32B32_FLOAT;
			buffer_view.stride_in_bytes = sizeof(FLOAT) * 3;
			break;
		case TINYGLTF_COMPONENT_TYPE_BYTE:
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
		case TINYGLTF_COMPONENT_TYPE_SHORT:
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
		case TINYGLTF_COMPONENT_TYPE_DOUBLE:
		default:
			_ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
			break;
		}
		break;
	case TINYGLTF_TYPE_VEC4:
		switch (accessor.componentType)
		{
		case TINYGLTF_COMPONENT_TYPE_BYTE:
			buffer_view.format = DXGI_FORMAT_R8G8B8A8_SINT;
			buffer_view.stride_in_bytes = sizeof(CHAR) * 4;
			break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
			buffer_view.format = DXGI_FORMAT_R8G8B8A8_UINT;
			buffer_view.stride_in_bytes = sizeof(BYTE) * 4;
			break;
		case TINYGLTF_COMPONENT_TYPE_SHORT:
			buffer_view.format = DXGI_FORMAT_R16G16B16A16_SINT;
			buffer_view.stride_in_bytes = sizeof(SHORT) * 4;
			break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			buffer_view.format = DXGI_FORMAT_R16G16B16A16_UINT;
			buffer_view.stride_in_bytes = sizeof(USHORT) * 4;
			break;
		case TINYGLTF_COMPONENT_TYPE_INT:
			buffer_view.format = DXGI_FORMAT_R32G32B32A32_SINT;
			buffer_view.stride_in_bytes = sizeof(INT) * 4;
			break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
			buffer_view.format = DXGI_FORMAT_R32G32B32A32_UINT;
			buffer_view.stride_in_bytes = sizeof(UINT) * 4;
			break;
		case TINYGLTF_COMPONENT_TYPE_FLOAT:
			buffer_view.format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			buffer_view.stride_in_bytes = sizeof(FLOAT) * 4;
			break;
		case TINYGLTF_COMPONENT_TYPE_DOUBLE:
		default:
			_ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
			break;
		}
		break;
	default:
		_ASSERT_EXPR(FALSE, L"This accessor component type is not supported");
		break;
	}
	buffer_view.size_in_bytes = static_cast<UINT>(accessor.count * buffer_view.stride_in_bytes);
	return buffer_view;
}

/**************************************************************************//**
	@brief
	@param[in]    device
	@param[in]    gltf_model
	@param[in]    model_meshes
*//***************************************************************************/
void GLTFImporter::fetch_meshes(ID3D11Device* device, const tinygltf::Model& gltf_model, std::vector<ModelResource::mesh>& model_meshes)
{
	HRESULT hr;
	for (std::vector<tinygltf::Mesh>::const_reference gltf_mesh : gltf_model.meshes)
	{
		ModelResource::mesh& mesh{ model_meshes.emplace_back() };
		mesh.name = gltf_mesh.name;

		for (std::vector<tinygltf::Primitive>::const_reference gltf_primitive : gltf_mesh.primitives)
		{
			ModelResource::mesh::primitive& primitive{ mesh.primitives.emplace_back() };
			primitive.material = gltf_primitive.material;

			const tinygltf::Accessor& gltf_accessor{ gltf_model.accessors.at(gltf_primitive.indices) };
			const tinygltf::BufferView& gltf_buffer_view{ gltf_model.bufferViews.at(gltf_accessor.bufferView) };

			primitive.index_buffer_view = make_model_buffer_view(gltf_accessor);

			D3D11_BUFFER_DESC buffer_desc{};
			buffer_desc.ByteWidth = static_cast<UINT>(primitive.index_buffer_view.size_in_bytes);
			buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			D3D11_SUBRESOURCE_DATA subresource_data{};
			subresource_data.pSysMem = gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() +
				gltf_buffer_view.byteOffset + gltf_accessor.byteOffset;
			hr = device->CreateBuffer(&buffer_desc, &subresource_data, primitive.index_buffer_view.buffer.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

			for (std::map<std::string, int>::const_reference gltf_attribute : gltf_primitive.attributes)
			{
				const tinygltf::Accessor& gltf_accessor{ gltf_model.accessors.at(gltf_attribute.second) };
				const tinygltf::BufferView& gltf_buffer_view{ gltf_model.bufferViews.at(gltf_accessor.bufferView) };

				ModelResource::buffer_view vertex_buffer_view{ make_model_buffer_view(gltf_accessor) };

				D3D11_BUFFER_DESC buffer_desc{};
				buffer_desc.ByteWidth = static_cast<UINT>(vertex_buffer_view.size_in_bytes);
				buffer_desc.Usage = D3D11_USAGE_DEFAULT;
				buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				D3D11_SUBRESOURCE_DATA subresource_data{};
				subresource_data.pSysMem = gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() +
					gltf_buffer_view.byteOffset + gltf_accessor.byteOffset;
				hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer_view.buffer.ReleaseAndGetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

				primitive.vertex_buffer_views.emplace(std::make_pair(gltf_attribute.first, vertex_buffer_view));
			}

			const std::unordered_map <std::string, ModelResource::buffer_view> attributes{
				{"TANGENT", {DXGI_FORMAT_R32G32B32A32_FLOAT}},
				{"TEXCOORD_0", {DXGI_FORMAT_R32G32_FLOAT}},
				{"JOINTS_0", {DXGI_FORMAT_R16G16B16A16_UINT}},
				{"WEIGHTS_0", {DXGI_FORMAT_R32G32B32A32_FLOAT}},
			};
			for (std::unordered_map<std::string, ModelResource::buffer_view>::const_reference attribute : attributes)
			{
				if (primitive.vertex_buffer_views.find(attribute.first) == primitive.vertex_buffer_views.end())
				{
					primitive.vertex_buffer_views.insert(std::make_pair(attribute.first, attribute.second));
				}
			}
		}
	}
}

/**************************************************************************//**
	@brief
	@param[in]    device
	@param[in]    gltf_model
	@param[in]    model_materials
	@param[in]    model_materialsRV
*//***************************************************************************/
void GLTFImporter::fetch_materials(ID3D11Device* device, const tinygltf::Model& gltf_model, std::vector<ModelResource::material>& model_materials, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& model_materialsRV)
{
	for (std::vector<tinygltf::Material>::const_reference gltf_material : gltf_model.materials)
	{
		std::vector<ModelResource::material>::reference material = model_materials.emplace_back();

		material.name = gltf_material.name;

		material.data.emissive_factor[0] = static_cast<float>(gltf_material.emissiveFactor.at(0));
		material.data.emissive_factor[1] = static_cast<float>(gltf_material.emissiveFactor.at(1));
		material.data.emissive_factor[2] = static_cast<float>(gltf_material.emissiveFactor.at(2));

		material.data.alpha_mode = gltf_material.alphaMode == "OPAQUE" ? 0 : gltf_material.alphaMode == "MASK" ? 1 : gltf_material.alphaMode == "BLEND" ? 2 : 0;
		material.data.alpha_cutoff = static_cast<float>(gltf_material.alphaCutoff);
		material.data.double_sided = gltf_material.doubleSided ? 1 : 0;

		material.data.pbr_metallic_roughness.basecolor_factor[0] = static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor.at(0));
		material.data.pbr_metallic_roughness.basecolor_factor[1] = static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor.at(1));
		material.data.pbr_metallic_roughness.basecolor_factor[2] = static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor.at(2));
		material.data.pbr_metallic_roughness.basecolor_factor[3] = static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor.at(3));

		material.data.pbr_metallic_roughness.basecolor_texture.index = gltf_material.pbrMetallicRoughness.baseColorTexture.index;
		material.data.pbr_metallic_roughness.basecolor_texture.texcoord = gltf_material.pbrMetallicRoughness.baseColorTexture.texCoord;

		material.data.pbr_metallic_roughness.metallic_factor = static_cast<float>(gltf_material.pbrMetallicRoughness.metallicFactor);
		material.data.pbr_metallic_roughness.roughness_factor = static_cast<float>(gltf_material.pbrMetallicRoughness.roughnessFactor);
		material.data.pbr_metallic_roughness.metallic_roughness_texture.index = gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.index;
		material.data.pbr_metallic_roughness.metallic_roughness_texture.texcoord = gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.texCoord;

		material.data.normal_texture.index = gltf_material.normalTexture.index;
		material.data.normal_texture.texcoord = gltf_material.normalTexture.texCoord;
		material.data.normal_texture.scale = static_cast<float>(gltf_material.normalTexture.scale);

		material.data.occlusion_texture.index = gltf_material.occlusionTexture.index;
		material.data.occlusion_texture.texcoord = gltf_material.occlusionTexture.texCoord;
		material.data.occlusion_texture.strength = static_cast<float>(gltf_material.occlusionTexture.strength);

		material.data.emissive_texture.index = gltf_material.emissiveTexture.index;
		material.data.emissive_texture.texcoord = gltf_material.emissiveTexture.texCoord;
	}

	std::vector<ModelResource::material::cbuffer> material_data;
	for (std::vector<ModelResource::material>::const_reference material : model_materials)
	{
		material_data.emplace_back(material.data);
	}

	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> material_buffer;
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(ModelResource::material::cbuffer) * material_data.size());
	buffer_desc.StructureByteStride = sizeof(ModelResource::material::cbuffer);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	D3D11_SUBRESOURCE_DATA subresource_data{};
	subresource_data.pSysMem = material_data.data();
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, material_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
	shader_resource_view_desc.Format = DXGI_FORMAT_UNKNOWN;
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	shader_resource_view_desc.Buffer.NumElements = static_cast<UINT>(material_data.size());
	hr = device->CreateShaderResourceView(material_buffer.Get(), &shader_resource_view_desc, model_materialsRV.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

/**************************************************************************//**
	@brief
	@param[in]    device
	@param[in]    gltf_model
	@param[in]    model_textures
	@param[in]    model_texturesRV
	@param[in]    model_images
	@param[in]    filename
*//***************************************************************************/
void GLTFImporter::fetch_textures(ID3D11Device* device, const tinygltf::Model& gltf_model,
	std::vector<ModelResource::texture>& model_textures, std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& model_texturesRV,
	std::vector<ModelResource::image>& model_images,
	std::string filename)
{
	HRESULT hr{ S_OK };
	for (const tinygltf::Texture& gltf_texture : gltf_model.textures)
	{
		ModelResource::texture& texture{ model_textures.emplace_back() };
		texture.name = gltf_texture.name;
		texture.source = gltf_texture.source;
	}

	for (const tinygltf::Image& gltf_image : gltf_model.images)
	{
		ModelResource::image& image{ model_images.emplace_back() };
		image.name = gltf_image.name;
		image.width = gltf_image.width;
		image.height = gltf_image.height;
		image.component = gltf_image.component;
		image.bits = gltf_image.bits;
		image.pixel_type = gltf_image.pixel_type;
		image.buffer_view = gltf_image.bufferView;
		image.mime_type = gltf_image.mimeType;
		image.uri = image.uri;
		image.as_is = image.as_is;

		if (gltf_image.bufferView > -1)
		{
			const tinygltf::BufferView& buffer_view = gltf_model.bufferViews.at(image.buffer_view);
			const tinygltf::Buffer& buffer = gltf_model.buffers.at(buffer_view.buffer);
			const byte* data = buffer.data.data() + buffer_view.byteOffset;

			std::wstring name;
			if (image.name.size() > 0)
			{
				name = std::wstring(name.begin(), name.end()) + L":" + std::wstring(image.name.begin(), image.name.end());
			}
			else
			{
				std::wstringstream wss;
				wss << std::wstring(name.begin(), name.end()) << ":" << image.buffer_view << L":" << buffer_view.buffer << L":" << buffer_view.byteOffset;
				name = wss.str();
			}

			ID3D11ShaderResourceView* texture_resource_view{};
			hr = GpuResourceUtils::load_texture_from_memory(device, data, buffer_view.byteLength, &texture_resource_view, false, 0);

			if (hr == S_OK)
			{
				model_texturesRV.emplace_back().Attach(texture_resource_view);
			}
		}
		else
		{
			const std::filesystem::path path(filename);
			ID3D11ShaderResourceView* shader_resource_view{};
			D3D11_TEXTURE2D_DESC texture2d_desc;
			std::wstring filename{
				path.parent_path().concat(L"/").wstring() + std::wstring(gltf_image.uri.begin(), gltf_image.uri.end()) };

			hr = GpuResourceUtils::load_texture_from_file(device, filename.c_str(), &shader_resource_view, &texture2d_desc);
			if (hr == S_OK)
			{
				model_texturesRV.emplace_back().Attach(shader_resource_view);
			}
		}
	}
}

/**************************************************************************//**
	@brief
	@param[in]    gltf_model
	@param[in]    model_skins
	@param[in]    model_animations
*//***************************************************************************/
void GLTFImporter::fetch_animations(const tinygltf::Model& gltf_model, std::vector<ModelResource::skin>& model_skins, std::vector<ModelResource::animation>& model_animations)
{
	for (std::vector<tinygltf::Skin>::const_reference transmission_skin : gltf_model.skins)
	{
		ModelResource::skin& skin{ model_skins.emplace_back() };

		const tinygltf::Accessor& gltf_accessor{ gltf_model.accessors.at(transmission_skin.inverseBindMatrices) };
		const tinygltf::BufferView& gltf_buffer_view{ gltf_model.bufferViews.at(gltf_accessor.bufferView) };

		skin.inverse_bind_matrices.resize(gltf_accessor.count);
		memcpy(skin.inverse_bind_matrices.data(), gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() +
			gltf_buffer_view.byteOffset + gltf_accessor.byteOffset, gltf_accessor.count * sizeof(DirectX::XMFLOAT4X4));

		skin.joints = transmission_skin.joints;
	}

	for (std::vector<tinygltf::Animation>::const_reference gltf_animation : gltf_model.animations)
	{
		ModelResource::animation& animation(model_animations.emplace_back());
		animation.name = gltf_animation.name;
		for (std::vector<tinygltf::AnimationSampler>::const_reference gltf_sampler : gltf_animation.samplers)
		{
			ModelResource::animation::sampler& sampler{ animation.samplers.emplace_back() };
			sampler.input = gltf_sampler.input;
			sampler.output = gltf_sampler.output;
			sampler.interpolation = gltf_sampler.interpolation;

			const tinygltf::Accessor& gltf_accessor{ gltf_model.accessors.at(gltf_sampler.input) };
			const tinygltf::BufferView& gltf_buffer_view{ gltf_model.bufferViews.at(gltf_accessor.bufferView) };

			std::pair<std::unordered_map<int, std::vector<float>>::iterator, bool> timelines{
				animation.timelines.emplace(gltf_sampler.input, gltf_accessor.count)
			};
			if (timelines.second)
			{
				memcpy(timelines.first->second.data(), gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() +
					gltf_buffer_view.byteOffset + gltf_accessor.byteOffset, gltf_accessor.count * sizeof(FLOAT));
			}
		}
		for (std::vector<tinygltf::AnimationChannel>::const_reference gltf_channel : gltf_animation.channels)
		{
			ModelResource::animation::channel& channel{ animation.channels.emplace_back() };
			channel.sampler = gltf_channel.sampler;
			channel.target_node = gltf_channel.target_node;
			channel.target_path = gltf_channel.target_path;

			const tinygltf::AnimationSampler& gltf_sampler{ gltf_animation.samplers.at(gltf_channel.sampler) };
			const tinygltf::Accessor& gltf_accessor{ gltf_model.accessors.at(gltf_sampler.output) };
			const tinygltf::BufferView& gltf_buffer_view{ gltf_model.bufferViews.at(gltf_accessor.bufferView) };

			if (gltf_channel.target_path == "scale")
			{
				std::pair<std::unordered_map<int, std::vector<DirectX::XMFLOAT3>>::iterator, bool> scales{
					animation.scales.emplace(gltf_sampler.output, gltf_accessor.count) };
				if (scales.second)
				{
					memcpy(scales.first->second.data(), gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() +
						gltf_buffer_view.byteOffset + gltf_accessor.byteOffset, gltf_accessor.count * sizeof(DirectX::XMFLOAT3));
				}
			}
			if (gltf_channel.target_path == "rotation")
			{
				std::pair<std::unordered_map<int, std::vector<DirectX::XMFLOAT4>>::iterator, bool> rotation{
					animation.rotations.emplace(gltf_sampler.output, gltf_accessor.count) };
				if (rotation.second)
				{
					memcpy(rotation.first->second.data(), gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() +
						gltf_buffer_view.byteOffset + gltf_accessor.byteOffset, gltf_accessor.count * sizeof(DirectX::XMFLOAT4));
				}
			}

			if (gltf_channel.target_path == "translation")
			{
				std::pair<std::unordered_map<int, std::vector<DirectX::XMFLOAT3>>::iterator, bool> translations{
					animation.translations.emplace(gltf_sampler.output, gltf_accessor.count) };
				if (translations.second)
				{
					memcpy(translations.first->second.data(), gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() +
						gltf_buffer_view.byteOffset + gltf_accessor.byteOffset, gltf_accessor.count * sizeof(DirectX::XMFLOAT3));
				}
			}
		}
	}

	for (std::vector<ModelResource::animation>::reference animation : model_animations)
	{
		for (decltype(animation.timelines)::reference timelines : animation.timelines)
		{
			animation.duration = std::max<float>(animation.duration, timelines.second.back());
		}
	}
}
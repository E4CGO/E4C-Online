#include "NewModelDX11.h"

#include <filesystem>
#include <fstream>
#include <stack>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/ResourceManager.h"
#include "TAKOEngine/Rendering/GpuResourceUtils.h"
#include "TAKOEngine/Rendering/ConstantBuffer.h"

NewModelDX11::NewModelDX11(ID3D11Device* device, const std::string& filename, float scaling, int modelType) : scaling(scaling), modelType(modelType)
{
	resource = ResourceManager::Instance().LoadModelResourceGLTF(filename);

	scenes = resource->GetNewScenes();
	gltf_nodes = resource->GetNewNodes();
	meshes = resource->GetNewMeshes();
	materials = resource->GetNewMaterials();
	material_resource_view = resource->GetNewMateriaslRV();
	textures = resource->GetNewTextures();
	images = resource->GetNewImages();
	texture_resource_views = resource->GetNewTexturesRV();
	skins = resource->GetNewSkins();
	animations = resource->GetNewAnimations();

	cumulate_transforms(gltf_nodes);
}

void NewModelDX11::cumulate_transforms(std::vector<ModelResource::node>& nodes)
{
	const DirectX::XMFLOAT4X4 coordinate_system_transforms[]
	{
		{-1, 0, 0, 0,
		  0, 1, 0, 0,
		  0, 0, 1, 0,
		  0, 0, 0, 1}, // 0:RHS Y-UP

		{ 1, 0, 0, 0,
		  0, 1, 0, 0,
		  0, 0, 1, 0,
		  0, 0, 0, 1}, // 1:LHS Y-UP

		{-1, 0, 0, 0,
		  0, 0,-1, 0,
		  0, 1, 0, 0,
		  0, 0, 0, 1}, // 2:RHS Z-UP

		{ 1, 0, 0, 0,
		  0, 0, 1, 0,
		  0, 1, 0, 0,
		  0, 0, 0, 1}, //3:LHS Z - UP
	};

	std::stack<DirectX::XMFLOAT4X4> parent_global_transforms;
	std::function<void(int)> traverse{ [&](int node_index)->void
	{
			ModelResource::node& node{nodes.at(node_index)};
			DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&coordinate_system_transforms[modelType]) * DirectX::XMMatrixScaling(scaling, scaling, scaling) };
			DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z) };
			DirectX::XMMATRIX R{ DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(node.rotation.x, node.rotation.y, node.rotation.z, node.rotation.w)) };
			DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(node.translation.x, node.translation.y, node.translation.z) };
			DirectX::XMStoreFloat4x4(&node.global_transform , C * S * R * T * DirectX::XMLoadFloat4x4(&parent_global_transforms.top()));
			for (int child_index : node.children)
			{
				parent_global_transforms.push(node.global_transform);
				traverse(child_index);
				parent_global_transforms.pop();
			}
	} };
	for (std::vector<int>::value_type node_index : scenes.at(0).nodes)
	{
		parent_global_transforms.push({ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 });
		traverse(node_index);
		parent_global_transforms.pop();
	}
}

ModelResource::buffer_view NewModelDX11::make_buffer_view(const tinygltf::Accessor& accessor)
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

void NewModelDX11::animate(size_t animation_index, float time, std::vector<ModelResource::node>& animated_nodes)
{
	std::function<size_t(const std::vector<float>&, float, float&)> indexof
	{
		[](const std::vector<float>& timelines, float time, float& interpolation_factor)->size_t
		{
			const size_t keyframe_count{ timelines.size() };

			if (time > timelines.at(keyframe_count - 1))
			{
				interpolation_factor = 1.0f;
				return keyframe_count - 2;
			}
			else if (time < timelines.at(0))
			{
				interpolation_factor = 0.0f;
				return 0;
			}
			size_t keyframe_index{ 0 };
			for (size_t time_index = 0; time_index < keyframe_count; time_index++)
			{
				if (time < timelines.at(time_index))
				{
					keyframe_index = std::max<size_t>(0LL, time_index - 1);
					break;
				}
			}
			interpolation_factor = (time - timelines.at(keyframe_index + 0)) /
				(timelines.at(keyframe_index + 1) - timelines.at(keyframe_index + 0));
			return	keyframe_index;
		}
	};

	if (animations.size() > 0)
	{
		const ModelResource::animation& animation{ animations.at(animation_index) };
		for (std::vector<ModelResource::animation::channel>::const_reference channel : animation.channels)
		{
			const ModelResource::animation::sampler& sampler{ animation.samplers.at(channel.sampler) };
			const std::vector<float>& timeline{ animation.timelines.at(sampler.input) };
			if (timeline.size() == 0)
			{
				continue;
			}
			float interpolation_factor{};
			size_t keyframe_index{ indexof(timeline, time, interpolation_factor) };
			if (channel.target_path == "scale")
			{
				const std::vector<DirectX::XMFLOAT3>& scales{ animation.scales.at(sampler.output) };
				DirectX::XMStoreFloat3(&animated_nodes.at(channel.target_node).scale,
					DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&scales.at(keyframe_index + 0)),
						DirectX::XMLoadFloat3(&scales.at(keyframe_index + 1)), interpolation_factor));
			}
			if (channel.target_path == "rotation")
			{
				const std::vector<DirectX::XMFLOAT4>& rotations{ animation.rotations.at(sampler.output) };
				DirectX::XMStoreFloat4(&animated_nodes.at(channel.target_node).rotation,
					DirectX::XMQuaternionNormalize(DirectX::XMQuaternionSlerp(DirectX::XMLoadFloat4(&rotations.at(keyframe_index + 0)),
						DirectX::XMLoadFloat4(&rotations.at(keyframe_index + 1)), interpolation_factor)));
			}
			if (channel.target_path == "translation")
			{
				const std::vector<DirectX::XMFLOAT3>& translations{ animation.translations.at(sampler.output) };
				DirectX::XMStoreFloat3(&animated_nodes.at(channel.target_node).translation,
					DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&translations.at(keyframe_index + 0)),
						DirectX::XMLoadFloat3(&translations.at(keyframe_index + 1)), interpolation_factor));
			}
		}

		cumulate_transforms(animated_nodes);
	}
	else
	{
		animated_nodes = gltf_nodes;
	}
}

void NewModelDX11::UpdateTransform(const DirectX::XMFLOAT4X4& worldTransform)
{
	m_WorldMatrix = worldTransform;
}

void NewModelDX11::PlayAnimation(int index, bool loop, float blendSeconds)
{
}

bool NewModelDX11::IsPlayAnimation() const
{
	return false;
}

void NewModelDX11::UpdateAnimation(float elapsedTime)
{
	this->time += elapsedTime;
	this->animate(0, this->time, gltf_nodes);
}

void NewModelDX11::ComputeAnimation(float elapsedTime)
{
}

void NewModelDX11::ComputeBlending(float elapsedTime)
{
}

void NewModelDX11::ComputeWorldBounds()
{
}

iModel::Node* NewModelDX11::FindNode(const char* name)
{
	return nullptr;
}

void NewModelDX11::CopyAnimations(iModel* model)
{
}

void NewModelDX11::CopyNodes(iModel* model)
{
}

void NewModelDX11::DrawDebugGUI()
{
}
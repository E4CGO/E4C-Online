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

bool set_null_load_image(tinygltf::Image*, const int, std::string*, std::string*, int, int, const unsigned char*, int, void*)
{
	return true;
}

NewModelDX11::NewModelDX11(ID3D11Device* device, const std::string& filename, float scaling)
{
	resource = ResourceManager::Instance().LoadModelResourceGLTF(filename);

	scenes = resource->GetNewScenes();
	nodes = resource->GetNewNodes();
	meshes = resource->GetNewMeshes();
	materials = resource->GetNewMaterials();
	material_resource_view = resource->GetNewMateriaslRV();
	textures = resource->GetNewTextures();
	images = resource->GetNewImages();
	texture_resource_views = resource->GetNewTexturesRV();
	skins = resource->GetNewSkins();
	animations = resource->GetNewAnimations();

	const std::map<std::string, ModelResource::buffer_view>& vertex_buffer_views{
		meshes.at(0).primitives.at(0).vertex_buffer_views };
	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
			{ "POSITION", 0, vertex_buffer_views.at("POSITION").format, 0, 0, D3D11_INPUT_PER_VERTEX_DATA},
			{ "NORMAL", 0, vertex_buffer_views.at("NORMAL").format, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, vertex_buffer_views.at("TANGENT").format, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, vertex_buffer_views.at("TEXCOORD_0").format, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "JOINTS", 0, vertex_buffer_views.at("JOINTS_0").format, 4, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "WEIGHTS", 0,vertex_buffer_views.at("WEIGHTS_0").format, 5, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	GpuResourceUtils::LoadVertexShader(device, "Data/Shader/LambertVS.cso", input_element_desc, _countof(input_element_desc), input_layout.ReleaseAndGetAddressOf(), vertex_shader.ReleaseAndGetAddressOf());
	GpuResourceUtils::LoadPixelShader(device, "Data/Shader/LambertPS.cso", pixel_shader.ReleaseAndGetAddressOf());

	{
		HRESULT hr;
		D3D11_BUFFER_DESC buffer_desc{};

		buffer_desc.ByteWidth = sizeof(primitive_constants);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		hr = device->CreateBuffer(&buffer_desc, nullptr, primitive_cbuffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		buffer_desc.ByteWidth = sizeof(primitive_joint_constants);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		hr = device->CreateBuffer(&buffer_desc, nullptr, primitive_joint_cbuffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		buffer_desc.ByteWidth = (sizeof(CbScene) + 15) / 16 * 16;
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&buffer_desc, nullptr, scene_cbuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

void NewModelDX11::cumulate_transforms(std::vector<ModelResource::node>& nodes)
{
	std::stack<DirectX::XMFLOAT4X4> parent_global_transforms;
	std::function<void(int)> traverse{ [&](int node_index)->void
	{
			ModelResource::node& node{nodes.at(node_index)};
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
		animated_nodes = nodes;
	}
}

void NewModelDX11::Render(const RenderContext& rc)
{
	ID3D11DeviceContext* immediate_context = rc.deviceContext;

	CbScene cbscene;

	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.camera->GetView());
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.camera->GetProjection());
	DirectX::XMStoreFloat4x4(&cbscene.view_projection, V * P);

	const DirectX::XMFLOAT3& eye = rc.camera->GetEye();
	cbscene.camera_position.x = eye.x;
	cbscene.camera_position.y = eye.y;
	cbscene.camera_position.z = eye.z;

	cbscene.ambientLightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	cbscene.directionalLightData = rc.directionalLightData;

	memcpy_s(
		cbscene.pointLightData,
		sizeof(cbscene.pointLightData),
		rc.pointLightData,
		sizeof(rc.pointLightData)
	);
	cbscene.pointLightCount = rc.pointLightCount;
	memcpy_s(
		cbscene.spotLightData,
		sizeof(cbscene.spotLightData),
		rc.spotLightData,
		sizeof(rc.spotLightData)
	);
	cbscene.spotLightCount = rc.spotLightCount;

	ID3D11SamplerState* samplerStates[] =
	{
		rc.renderState->GetSamplerState(SamplerState::PointWrap),
		rc.renderState->GetSamplerState(SamplerState::LinearWrap),
		rc.renderState->GetSamplerState(SamplerState::AnisotropicWrap),
	};
	rc.deviceContext->PSSetSamplers(0, _countof(samplerStates), samplerStates);

	// レンダーステート設定
	const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	rc.deviceContext->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Opaque), blend_factor, 0xFFFFFFFF);
	rc.deviceContext->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	rc.deviceContext->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));

	rc.deviceContext->UpdateSubresource(scene_cbuffer.Get(), 0, 0, &cbscene, 0, 0);
	rc.deviceContext->VSSetConstantBuffers(1, 1, scene_cbuffer.GetAddressOf());
	rc.deviceContext->PSSetConstantBuffers(1, 1, scene_cbuffer.GetAddressOf());

	const std::vector<ModelResource::node>& nodes{ animated_nodes.size() > 0 ? animated_nodes : NewModelDX11::nodes };

	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
	immediate_context->IASetInputLayout(input_layout.Get());
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	std::function<void(int)> traverse{ [&](int node_index)->void {
		const ModelResource::node& node {nodes.at(node_index)};
		if (node.skin > -1)
		{
			const ModelResource::skin& skin{ skins.at(node.skin) };
			primitive_joint_constants primitive_joint_data{};
			for (size_t joint_index = 0; joint_index < skin.joints.size(); ++joint_index)
			{
				XMStoreFloat4x4(&primitive_joint_data.matrices[joint_index],
					XMLoadFloat4x4(&skin.inverse_bind_matrices.at(joint_index)) *
					XMLoadFloat4x4(&nodes.at(skin.joints.at(joint_index)).global_transform) *
					XMMatrixInverse(NULL, XMLoadFloat4x4(&node.global_transform))
				);
			}
			immediate_context->UpdateSubresource(primitive_joint_cbuffer.Get(), 0, 0, &primitive_joint_data, 0, 0);
			immediate_context->VSSetConstantBuffers(2, 1, primitive_joint_cbuffer.GetAddressOf());
		}
		if (node.mesh > -1)
		{
			const ModelResource::mesh& mesh{ meshes.at(node.mesh) };
			for (std::vector<ModelResource::mesh::primitive>::const_reference primitive : mesh.primitives)
			{
				ID3D11Buffer* vertex_buffers[]
				{
					primitive.vertex_buffer_views.at("POSITION").buffer.Get(),
					primitive.vertex_buffer_views.at("NORMAL").buffer.Get(),
					primitive.vertex_buffer_views.at("TANGENT").buffer.Get(),
					primitive.vertex_buffer_views.at("TEXCOORD_0").buffer.Get(),
					primitive.vertex_buffer_views.at("JOINTS_0").buffer.Get(),
					primitive.vertex_buffer_views.at("WEIGHTS_0").buffer.Get(),
				};
				UINT strides[]{
					static_cast<UINT>(primitive.vertex_buffer_views.at("POSITION").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("NORMAL").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("TANGENT").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("TEXCOORD_0").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("JOINTS_0").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("WEIGHTS_0").stride_in_bytes),
				};
				UINT offsets[_countof(vertex_buffers)]{ 0 };
				immediate_context->IASetVertexBuffers(0, _countof(vertex_buffers), vertex_buffers, strides, offsets);
				immediate_context->IASetIndexBuffer(primitive.index_buffer_view.buffer.Get(), primitive.index_buffer_view.format, 0);

				primitive_constants primitive_data{};
				primitive_data.material = primitive.material;
				primitive_data.has_tangent = primitive.vertex_buffer_views.at("TANGENT").buffer != NULL;
				primitive_data.skin = node.skin;

				DirectX::XMStoreFloat4x4(&primitive_data.world, DirectX::XMLoadFloat4x4(&node.global_transform) * DirectX::XMLoadFloat4x4(&world));

				immediate_context->UpdateSubresource(primitive_cbuffer.Get(), 0, 0, &primitive_data, 0, 0);
				immediate_context->VSSetConstantBuffers(0, 1, primitive_cbuffer.GetAddressOf());
				immediate_context->PSSetConstantBuffers(0, 1, primitive_cbuffer.GetAddressOf());

				immediate_context->PSSetShaderResources(0, 1, material_resource_view.GetAddressOf());

				const ModelResource::material& material{ materials.at(primitive.material) };
				const int texture_indices[]
				{
					material.data.pbr_metallic_roughness.basecolor_texture.index,
					material.data.pbr_metallic_roughness.metallic_roughness_texture.index,
					material.data.normal_texture.index,
					material.data.emissive_texture.index,
					material.data.occlusion_texture.index,
				};
				ID3D11ShaderResourceView* null_shader_resource_view{};
				std::vector<ID3D11ShaderResourceView*> shader_resource_views(_countof(texture_indices));
				for (int texture_index = 0; texture_index < shader_resource_views.size(); ++texture_index)
				{
					shader_resource_views.at(texture_index) = texture_indices[texture_index] > -1 ?
						texture_resource_views.at(textures.at(texture_indices[texture_index]).source).Get() :
						null_shader_resource_view;
				}
				immediate_context->PSSetShaderResources(1, static_cast<UINT>(shader_resource_views.size()), shader_resource_views.data());

				immediate_context->DrawIndexed(static_cast<UINT>(primitive.index_buffer_view.count()), 0, 0);
			}
		}

		for (std::vector<int>::value_type child_index : node.children)
		{
			traverse(child_index);
		}
		} };

	for (std::vector<int>::value_type node_index : scenes.at(0).nodes)
	{
		traverse(node_index);
	}
}

//void NewModelDX11::renderDX12(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4 world, const std::vector<node>& animated_nodes)
//{
//	const std::vector<node>& nodes{ animated_nodes.size() > 0 ? animated_nodes : NewModelDX11::nodes };
//
//	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
//	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
//	immediate_context->IASetInputLayout(input_layout.Get());
//	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	std::function<void(int)> traverse{ [&](int node_index)->void {
//		const node& node {nodes.at(node_index)};
//		if (node.skin > -1)
//		{
//			const skin& skin{ skins.at(node.skin) };
//			primitive_joint_constants primitive_joint_data{};
//			for (size_t joint_index = 0; joint_index < skin.joints.size(); ++joint_index)
//			{
//				XMStoreFloat4x4(&primitive_joint_data.matrices[joint_index],
//					XMLoadFloat4x4(&skin.inverse_bind_matrices.at(joint_index)) *
//					XMLoadFloat4x4(&nodes.at(skin.joints.at(joint_index)).global_transform) *
//					XMMatrixInverse(NULL, XMLoadFloat4x4(&node.global_transform))
//				);
//			}
//			immediate_context->UpdateSubresource(primitive_joint_cbuffer.Get(), 0, 0, &primitive_joint_data, 0, 0);
//			immediate_context->VSSetConstantBuffers(2, 1, primitive_joint_cbuffer.GetAddressOf());
//		}
//		if (node.mesh > -1)
//		{
//			const mesh& mesh{ meshes.at(node.mesh) };
//			for (std::vector<mesh::primitive>::const_reference primitive : mesh.primitives)
//			{
//				ID3D11Buffer* vertex_buffers[]
//				{
//					primitive.vertex_buffer_views.at("POSITION").buffer.Get(),
//					primitive.vertex_buffer_views.at("NORMAL").buffer.Get(),
//					primitive.vertex_buffer_views.at("TANGENT").buffer.Get(),
//					primitive.vertex_buffer_views.at("TEXCOORD_0").buffer.Get(),
//					primitive.vertex_buffer_views.at("JOINTS_0").buffer.Get(),
//					primitive.vertex_buffer_views.at("WEIGHTS_0").buffer.Get(),
//				};
//				UINT strides[]{
//					static_cast<UINT>(primitive.vertex_buffer_views.at("POSITION").stride_in_bytes),
//					static_cast<UINT>(primitive.vertex_buffer_views.at("NORMAL").stride_in_bytes),
//					static_cast<UINT>(primitive.vertex_buffer_views.at("TANGENT").stride_in_bytes),
//					static_cast<UINT>(primitive.vertex_buffer_views.at("TEXCOORD_0").stride_in_bytes),
//					static_cast<UINT>(primitive.vertex_buffer_views.at("JOINTS_0").stride_in_bytes),
//					static_cast<UINT>(primitive.vertex_buffer_views.at("WEIGHTS_0").stride_in_bytes),
//				};
//				UINT offsets[_countof(vertex_buffers)]{ 0 };
//				immediate_context->IASetVertexBuffers(0, _countof(vertex_buffers), vertex_buffers, strides, offsets);
//				immediate_context->IASetIndexBuffer(primitive.index_buffer_view.buffer.Get(), primitive.index_buffer_view.format, 0);
//
//				primitive_constants primitive_data{};
//				primitive_data.material = primitive.material;
//				primitive_data.has_tangent = primitive.vertex_buffer_views.at("TANGENT").buffer != NULL;
//				primitive_data.skin = node.skin;
//
//				DirectX::XMStoreFloat4x4(&primitive_data.world, DirectX::XMLoadFloat4x4(&node.global_transform) * DirectX::XMLoadFloat4x4(&world));
//
//				immediate_context->UpdateSubresource(primitive_cbuffer.Get(), 0, 0, &primitive_data, 0, 0);
//				immediate_context->VSSetConstantBuffers(0, 1, primitive_cbuffer.GetAddressOf());
//				immediate_context->PSSetConstantBuffers(0, 1, primitive_cbuffer.GetAddressOf());
//
//				immediate_context->PSSetShaderResources(0, 1, material_resource_view.GetAddressOf());
//
//				const material& material{ materials.at(primitive.material) };
//				const int texture_indices[]
//				{
//					material.data.pbr_metallic_roughness.basecolor_texture.index,
//					material.data.pbr_metallic_roughness.metallic_roughness_texture.index,
//					material.data.normal_texture.index,
//					material.data.emissive_texture.index,
//					material.data.occlusion_texture.index,
//				};
//				ID3D11ShaderResourceView* null_shader_resource_view{};
//				std::vector<ID3D11ShaderResourceView*> shader_resource_views(_countof(texture_indices));
//				for (int texture_index = 0; texture_index < shader_resource_views.size(); ++texture_index)
//				{
//					shader_resource_views.at(texture_index) = texture_indices[texture_index] > -1 ?
//						texture_resource_views.at(textures.at(texture_indices[texture_index]).source).Get() :
//						null_shader_resource_view;
//				}
//				immediate_context->PSSetShaderResources(1, static_cast<UINT>(shader_resource_views.size()), shader_resource_views.data());
//
//				immediate_context->DrawIndexed(static_cast<UINT>(primitive.index_buffer_view.count()), 0, 0);
//			}
//		}
//
//		for (std::vector<int>::value_type child_index : node.children)
//		{
//			traverse(child_index);
//		}
//		} };
//
//	for (std::vector<int>::value_type node_index : scenes.at(0).nodes)
//	{
//		traverse(node_index);
//	}
//}

void NewModelDX11::UpdateTransform(const DirectX::XMFLOAT4X4& worldTransform)
{
	world = worldTransform;
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
	this->animate(0, this->time, nodes);
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
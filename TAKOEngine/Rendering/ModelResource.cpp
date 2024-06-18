#include <filesystem>
#include <fstream>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/Graphics.h"
#include "TAKOEngine/Rendering/ModelResource.h"
#include "TAKOEngine/Rendering/ResourceManager.h"
#include "TAKOEngine/Rendering/GpuResourceUtils.h"
#include "TAKOEngine/Tool/AssimpImporter.h"
#include "TAKOEngine/Tool/Logger.h"

namespace DirectX
{
	template<class Archive>
	void serialize(Archive& archive, XMUINT4& v)
	{
		archive(
			cereal::make_nvp("X", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z),
			cereal::make_nvp("w", v.w)
		);
	}
	template<class Archive>
	void serialize(Archive& archive, XMFLOAT2& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y)
		);
	}
	template<class Archive>
	void serialize(Archive& archive, XMFLOAT3& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z)
		);
	}
	template<class Archive>
	void serialize(Archive& archive, XMFLOAT4& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z),
			cereal::make_nvp("w", v.w)
		);
	}
	template<class Archive>
	void serialize(Archive& archive, XMFLOAT4X4& m)
	{
		archive(
			cereal::make_nvp("_11", m._11),
			cereal::make_nvp("_12", m._12),
			cereal::make_nvp("_13", m._13),
			cereal::make_nvp("_14", m._14),
			cereal::make_nvp("_21", m._21),
			cereal::make_nvp("_22", m._22),
			cereal::make_nvp("_23", m._23),
			cereal::make_nvp("_24", m._24),
			cereal::make_nvp("_31", m._31),
			cereal::make_nvp("_32", m._32),
			cereal::make_nvp("_33", m._33),
			cereal::make_nvp("_34", m._34),
			cereal::make_nvp("_41", m._41),
			cereal::make_nvp("_42", m._42),
			cereal::make_nvp("_43", m._43),
			cereal::make_nvp("_44", m._44)
		);
	}
}

template<class Archive>
void ModelResource::Node::serialize(Archive& archive)
{
	archive(
		CEREAL_NVP(name),
		CEREAL_NVP(path),
		CEREAL_NVP(parentIndex),
		CEREAL_NVP(position),
		CEREAL_NVP(rotation),
		CEREAL_NVP(scale)
	);
}
template<class Archive>
void ModelResource::Material::serialize(Archive& archive)
{
	archive(
		CEREAL_NVP(name),
		CEREAL_NVP(diffuseTextureFileName),
		CEREAL_NVP(normalTextureFileName),
		CEREAL_NVP(color)
	);
}
template<class Archive>
void ModelResource::Vertex::serialize(Archive& archive)
{
	archive(
		CEREAL_NVP(position),
		CEREAL_NVP(boneWeight),
		CEREAL_NVP(boneIndex),
		CEREAL_NVP(texcoord),
		CEREAL_NVP(color),
		CEREAL_NVP(normal),
		CEREAL_NVP(tangent)
	);
}
template<class Archive>
void ModelResource::Bone::serialize(Archive& archive)
{
	archive(
		CEREAL_NVP(nodeIndex),
		CEREAL_NVP(offsetTransform)
	);
}
template<class Archive>
void ModelResource::Mesh::serialize(Archive& archive)
{
	archive(
		CEREAL_NVP(vertices),
		CEREAL_NVP(indices),
		CEREAL_NVP(bones),
		CEREAL_NVP(nodeIndex),
		CEREAL_NVP(materialIndex)
	);
}
template<class Archive>
void ModelResource::VectorKeyframe::serialize(Archive& archive)
{
	archive(
		CEREAL_NVP(seconds),
		CEREAL_NVP(value)
	);
}
template<class Archive>
void ModelResource::QuaternionKeyframe::serialize(Archive& archive)
{
	archive(
		CEREAL_NVP(seconds),
		CEREAL_NVP(value)
	);
}
template<class Archive>
void ModelResource::NodeAnim::serialize(Archive& archive)
{
	archive(
		CEREAL_NVP(positionKeyframes),
		CEREAL_NVP(rotationKeyframes),
		CEREAL_NVP(scaleKeyframes)
	);
}
template<class Archive>
void ModelResource::Animation::serialize(Archive& archive)
{
	archive(
		CEREAL_NVP(name),
		CEREAL_NVP(secondsLength),
		CEREAL_NVP(nodeAnims)
	);
}

// �ǂݍ���
void ModelResource::Load(ID3D11Device* device, const char* filename)
{
	std::filesystem::path filepath(filename);
	std::filesystem::path dirpath(filepath.parent_path());

	// �Ǝ��`���̃��f���t�@�C���̑��݊m�F
	filepath.replace_extension(".cereal");
	if (std::filesystem::exists(filepath))
	{
		// �Ǝ��`���̃��f���t�@�C���̓ǂݍ���
		Deserialize(filepath.string().c_str());
	}
	else
	{
		// �ėp���f���t�@�C���ǂݍ���
		AssimpImporter importer(filename);

		// �}�e���A���f�[�^�ǂݎ��
		importer.LoadMaterials(materials);

		// �m�[�h�f�[�^�ǂݎ��
		importer.LoadNodes(nodes);

		// ���b�V���f�[�^�ǂݎ��
		importer.LoadMeshes(meshes, nodes);

		// �A�j���[�V�����f�[�g�ǂݎ��
		importer.LoadAnimations(animations, nodes);

		// �Ǝ��`���̃��f���t�@�C����ۑ�
		Serialize(filepath.string().c_str());
	}

	//// �m�[�h�\�z
	//for (size_t nodeIndex = 0; nodeIndex < nodes.size(); nodeIndex++)
	//{
	//	Node& node = nodes.at(nodeIndex);

	//	//// �e�q�֌W���\�z
	//	//node.parent = node.parentIndex >= 0 ? &nodes.at(node.parentIndex) : nullptr;
	//	//if (node.parent != nullptr)
	//	//{
	//	//	node.parent->children.emplace_back(&node);
	//	//}
	//}

	// �}�e���A���\�z
	for (Material& material : materials)
	{
		if (material.diffuseTextureFileName.empty())
		{
			// �_�~�[�e�N�X�`���쐬
			HRESULT hr = GpuResourceUtils::CreateDummyTexture(device, 0xFFFFFFFF, material.diffuseMap.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
		else
		{
			// �f�B�t���[�Y�e�N�X�`���ǂݍ���
			std::filesystem::path diffuseTexturePath(dirpath / material.diffuseTextureFileName);
			HRESULT hr = GpuResourceUtils::LoadTexture(device, diffuseTexturePath.string().c_str(), material.diffuseMap.GetAddressOf());

			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
		if (material.normalTextureFileName.empty())
		{
			// �@���_�~�[�e�N�X�`���쐬
			HRESULT hr = GpuResourceUtils::CreateDummyTexture(device, 0xFFFF7F7F, material.normalMap.GetAddressOf());

			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
		else
		{
			// �@���e�N�X�`���ǂݍ���
			std::filesystem::path texturePath(dirpath / material.normalTextureFileName);
			HRESULT hr = GpuResourceUtils::LoadTexture(device, texturePath.string().c_str(), material.normalMap.GetAddressOf());
		}
	}

	// ���b�V���\�z
	for (Mesh& mesh : meshes)
	{
		// �Q�ƃm�[�h�ݒ�
		mesh.node = &nodes.at(mesh.nodeIndex);

		// �Q�ƃ}�e���A���ݒ�
		mesh.material = &materials.at(mesh.materialIndex);

		// ���_�o�b�t�@
		{
			D3D11_BUFFER_DESC bufferDesc = {};
			D3D11_SUBRESOURCE_DATA subresourceData = {};

			bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * mesh.vertices.size());
			bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
			subresourceData.pSysMem = mesh.vertices.data();
			subresourceData.SysMemPitch = 0;
			subresourceData.SysMemSlicePitch = 0;

			HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.vertexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}

		// �C���f�b�N�X�o�b�t�@
		{
			D3D11_BUFFER_DESC bufferDesc = {};
			D3D11_SUBRESOURCE_DATA subresourceData = {};

			bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
			bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
			subresourceData.pSysMem = mesh.indices.data();
			subresourceData.SysMemPitch = 0;
			subresourceData.SysMemSlicePitch = 0;

			HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.indexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}

		// �{�[���\�z
		for (Bone& bone : mesh.bones)
		{
			// �Q�ƃm�[�h�ݒ�
			bone.node = &nodes.at(bone.nodeIndex);
		}
	}
}

void ModelResource::Load(const char* filename)
{
	// �f�B���N�g���p�X�擾
	char drive[32], dir[256], dirname[256];
	::_splitpath_s(filename, drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
	::_makepath_s(dirname, sizeof(dirname), drive, dir, nullptr, nullptr);

	// �f�V���A���C�Y
	//Deserialize(filename);

	// ���f���\�z
	BuildModel(dirname, filename);
}

void ModelResource::BuildModel(const char* dirname, const char* filename)
{
	Graphics& graphics = Graphics::Instance();

	ID3D12Device* d3d_device = graphics.GetDeviceDX12();

	HRESULT hr = S_OK;

	// �ėp���f���t�@�C���ǂݍ���
	AssimpImporter importer(filename);

	// �}�e���A���f�[�^�ǂݎ��
	importer.LoadMaterials(materials);

	// �m�[�h�f�[�^�ǂݎ��
	importer.LoadNodes(nodes);

	// ���b�V���f�[�^�ǂݎ��
	importer.LoadMeshes(meshes, nodes);

	// �A�j���[�V�����f�[�g�ǂݎ��
	importer.LoadAnimations(animations, nodes);

	// �}�e���A��
	for (Material& material : materials)
	{
		// ���΃p�X�̉���
		char filename[256];
		::_makepath_s(filename, 256, nullptr, dirname, material.diffuseTextureFileName.c_str(), nullptr);

		// �}���`�o�C�g�������烏�C�h�����֕ϊ�
		wchar_t wfilename[256];
		::MultiByteToWideChar(CP_ACP, 0, filename, -1, wfilename, 256);

		// �e�N�X�`���ǂݍ���
		hr = graphics.LoadTexture(filename, material.d3d_srv_resource.GetAddressOf());
		if (FAILED(hr))
		{
			// �ǂݍ��ݎ��s������_�~�[�e�N�X�`�������
			LOG("load failed : %s\n", filename);

			hr = graphics.CreateDummyTexture(material.d3d_srv_resource.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}

		// �V�F�[�_�[���\�[�X�r���[�̐���
		{
			D3D12_RESOURCE_DESC d3d_resource_desc = material.d3d_srv_resource->GetDesc();

			// �V�F�[�_�[���\�[�X�r���[�̐ݒ�
			D3D12_SHADER_RESOURCE_VIEW_DESC d3d_srv_desc = {};
			d3d_srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			d3d_srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			d3d_srv_desc.Format = d3d_resource_desc.Format;
			d3d_srv_desc.Texture2D.MipLevels = d3d_resource_desc.MipLevels;
			d3d_srv_desc.Texture2D.MostDetailedMip = 0;

			// �f�B�X�N���v�^�擾
			material.srv_descriptor = graphics.GetShaderResourceDescriptorHeap()->PopDescriptor();

			// �V�F�[�_���\�[�X�r���[�𐶐�.
			d3d_device->CreateShaderResourceView(
				material.d3d_srv_resource.Get(),
				&d3d_srv_desc,
				material.srv_descriptor->GetCpuHandle()
			);
		}

		// �萔�o�b�t�@�̐���
		{
			// �q�[�v�v���p�e�B�̐ݒ�
			D3D12_HEAP_PROPERTIES d3d_heap_props{};
			d3d_heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
			d3d_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			d3d_heap_props.CreationNodeMask = 1;
			d3d_heap_props.VisibleNodeMask = 1;

			// ���\�[�X�̐ݒ�
			D3D12_RESOURCE_DESC d3d_resource_desc{};
			d3d_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			d3d_resource_desc.Alignment = 0;
			d3d_resource_desc.Width = ((sizeof(CbLambertMaterial)) + 255) & ~255;	// 256�o�C�g�A���C�����g�ɂ���
			d3d_resource_desc.Height = 1;
			d3d_resource_desc.DepthOrArraySize = 1;
			d3d_resource_desc.MipLevels = 1;
			d3d_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
			d3d_resource_desc.SampleDesc.Count = 1;
			d3d_resource_desc.SampleDesc.Quality = 0;
			d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			d3d_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

			// �萔�o�b�t�@�̐���
			hr = d3d_device->CreateCommittedResource(
				&d3d_heap_props,
				D3D12_HEAP_FLAG_NONE,
				&d3d_resource_desc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(material.d3d_cbv_resource.GetAddressOf())
			);
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			material.d3d_cbv_resource->SetName(L"ModelResourceMaterialConstantBuffer");

			// �f�B�X�N���v�^�擾
			material.cbv_descriptor = graphics.GetShaderResourceDescriptorHeap()->PopDescriptor();

			// �萔�o�b�t�@�r���[�̐���
			D3D12_CONSTANT_BUFFER_VIEW_DESC d3d_cbv_desc;
			d3d_cbv_desc.BufferLocation = material.d3d_cbv_resource->GetGPUVirtualAddress();
			d3d_cbv_desc.SizeInBytes = static_cast<UINT>(d3d_resource_desc.Width);
			d3d_device->CreateConstantBufferView(
				&d3d_cbv_desc,
				material.cbv_descriptor->GetCpuHandle()
			);

			// �f�[�^�ݒ�
			CbLambertMaterial* cb_data;
			hr = material.d3d_cbv_resource->Map(0, nullptr, reinterpret_cast<void**>(&cb_data));
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

			cb_data->color = material.color;

			material.d3d_cbv_resource->Unmap(0, nullptr);
		}
	}

	// ���b�V��
	for (Mesh& mesh : meshes)
	{
		mesh.node = &nodes.at(mesh.nodeIndex);

		// �Q�ƃ}�e���A���ݒ�
		mesh.material = &materials.at(mesh.materialIndex);

		// ���_�o�b�t�@
		{
			// �q�[�v�v���p�e�B�̐ݒ�
			D3D12_HEAP_PROPERTIES d3d_heap_props{};
			d3d_heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
			d3d_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			d3d_heap_props.CreationNodeMask = 1;
			d3d_heap_props.VisibleNodeMask = 1;

			// ���\�[�X�̐ݒ�
			D3D12_RESOURCE_DESC d3d_resource_desc{};
			d3d_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			d3d_resource_desc.Alignment = 0;
			d3d_resource_desc.Width = sizeof(Vertex) * mesh.vertices.size();
			d3d_resource_desc.Height = 1;
			d3d_resource_desc.DepthOrArraySize = 1;
			d3d_resource_desc.MipLevels = 1;
			d3d_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
			d3d_resource_desc.SampleDesc.Count = 1;
			d3d_resource_desc.SampleDesc.Quality = 0;
			d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			d3d_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

			// �A�b�v���[�h�p���_�o�b�t�@����
			Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_upload_resource;
			hr = d3d_device->CreateCommittedResource(
				&d3d_heap_props,
				D3D12_HEAP_FLAG_NONE,
				&d3d_resource_desc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(d3d_upload_resource.GetAddressOf())
			);
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

			// �A�b�v���[�h�p���_�o�b�t�@�Ƀf�[�^���e���R�s�[
			void* vb = nullptr;
			hr = d3d_upload_resource->Map(0, nullptr, &vb);
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			::memcpy(vb, mesh.vertices.data(), sizeof(Vertex) * mesh.vertices.size());
			d3d_upload_resource->Unmap(0, nullptr);

			// �`��p���_�o�b�t�@����
			d3d_heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
			hr = d3d_device->CreateCommittedResource(
				&d3d_heap_props,
				D3D12_HEAP_FLAG_NONE,
				&d3d_resource_desc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(mesh.d3d_vb_resource.GetAddressOf())
			);
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			mesh.d3d_vb_resource->SetName(L"ModelResourceVertexBuffer");

			// ���_�o�b�t�@�r���[�ݒ�
			mesh.d3d_vbv.BufferLocation = mesh.d3d_vb_resource->GetGPUVirtualAddress();
			mesh.d3d_vbv.SizeInBytes = static_cast<UINT>(sizeof(Vertex) * mesh.vertices.size());
			mesh.d3d_vbv.StrideInBytes = sizeof(Vertex);

			// �A�b�v���[�h�p�o�b�t�@��`��p�o�b�t�@�ɃR�s�[
			graphics.CopyBuffer(d3d_upload_resource.Get(), mesh.d3d_vb_resource.Get());
		}

		// �C���f�b�N�X�o�b�t�@
		{
			// �q�[�v�v���p�e�B�̐ݒ�
			D3D12_HEAP_PROPERTIES d3d_heap_props{};
			d3d_heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
			d3d_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			d3d_heap_props.CreationNodeMask = 1;
			d3d_heap_props.VisibleNodeMask = 1;

			// ���\�[�X�̐ݒ�
			D3D12_RESOURCE_DESC d3d_resource_desc{};
			d3d_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			d3d_resource_desc.Alignment = 0;
			d3d_resource_desc.Width = sizeof(UINT) * mesh.indices.size();
			d3d_resource_desc.Height = 1;
			d3d_resource_desc.DepthOrArraySize = 1;
			d3d_resource_desc.MipLevels = 1;
			d3d_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
			d3d_resource_desc.SampleDesc.Count = 1;
			d3d_resource_desc.SampleDesc.Quality = 0;
			d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			d3d_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

			// �A�b�v���[�h�p���_�o�b�t�@����
			Microsoft::WRL::ComPtr<ID3D12Resource> d3d_upload_resource;
			hr = d3d_device->CreateCommittedResource(
				&d3d_heap_props,
				D3D12_HEAP_FLAG_NONE,
				&d3d_resource_desc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(d3d_upload_resource.GetAddressOf())
			);
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

			// �A�b�v���[�h�p�C���f�b�N�X�o�b�t�@�Ƀf�[�^���e���R�s�[
			void* ib = nullptr;
			hr = d3d_upload_resource->Map(0, nullptr, &ib);
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			::memcpy(ib, mesh.indices.data(), sizeof(UINT) * mesh.indices.size());
			d3d_upload_resource->Unmap(0, nullptr);

			// �`��p�C���f�b�N�X�o�b�t�@����
			d3d_heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
			hr = d3d_device->CreateCommittedResource(
				&d3d_heap_props,
				D3D12_HEAP_FLAG_NONE,
				&d3d_resource_desc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(mesh.d3d_ib_resource.GetAddressOf())
			);
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			mesh.d3d_vb_resource->SetName(L"ModelResourceIndexBuffer");

			// �C���f�b�N�X�o�b�t�@�r���[�ݒ�
			mesh.d3d_ibv.BufferLocation = mesh.d3d_ib_resource->GetGPUVirtualAddress();
			mesh.d3d_ibv.SizeInBytes = static_cast<UINT>(sizeof(UINT) * mesh.indices.size());
			mesh.d3d_ibv.Format = DXGI_FORMAT_R32_UINT;

			// �A�b�v���[�h�p�o�b�t�@��`��p�o�b�t�@�ɃR�s�[
			graphics.CopyBuffer(d3d_upload_resource.Get(), mesh.d3d_ib_resource.Get());
		}
	}
}

// �V���A���C�Y
void ModelResource::Serialize(const char* filename)
{
	std::ofstream ostream(filename, std::ios::binary);
	if (ostream.is_open())
	{
		cereal::BinaryOutputArchive archive(ostream);

		try
		{
			archive(
				CEREAL_NVP(nodes),
				CEREAL_NVP(materials),
				CEREAL_NVP(meshes),
				CEREAL_NVP(animations)
			);
		}
		catch (...)
		{
			_ASSERT_EXPR_A(false, "Model serialize failed.");
		}
	}
}

// �f�V���A���C�Y
void ModelResource::Deserialize(const char* filename)
{
	std::ifstream istream(filename, std::ios::binary);
	if (istream.is_open())
	{
		cereal::BinaryInputArchive archive(istream);
		try
		{
			archive(
				CEREAL_NVP(nodes),
				CEREAL_NVP(materials),
				CEREAL_NVP(meshes),
				CEREAL_NVP(animations)
			);
		}
		catch (...)
		{
			_ASSERT_EXPR_A(false, "Model deserialize failed.");
		}
	}
	else
	{
		_ASSERT_EXPR_A(false, "Model file not found.");
	}
}

void ModelResource::ImportAnimations(const char* filename)
{
	animations = ResourceManager::Instance().LoadModelResource(filename)->animations;
}

int ModelResource::FindNodeIndex(NodeId nodeId) const
{
	int nodeCount = static_cast<int>(nodes.size());
	for (int i = 0; i < nodeCount; ++i)
	{
		if (nodes[i].id == nodeId)
		{
			return i;
		}
	}
	return -1;
}
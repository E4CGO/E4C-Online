#include "ModelDX12.h"
#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/Graphics.h"

// コンストラクタ
ModelDX12::ModelDX12(const char* filename)
{
	Graphics& graphics = Graphics::Instance();

	ID3D12Device* d3d_device = graphics.GetDeviceDX12();

	HRESULT hr = S_OK;

	// リソース読み込み
	m_resource = std::make_shared<ModelResource>();
	m_resource->Load(filename);

	// ノード
	{
		const std::vector<ModelResource::Node>& res_nodes = m_resource->GetNodes();

		m_nodes.resize(res_nodes.size());
		for (size_t nodeIndex = 0; nodeIndex < m_nodes.size(); ++nodeIndex)
		{
			auto&& src = res_nodes.at(nodeIndex);
			auto&& dst = m_nodes.at(nodeIndex);

			dst.name = src.name.c_str();
			dst.parent = src.parentIndex >= 0 ? &m_nodes.at(src.parentIndex) : nullptr;
			dst.scale = src.scale;
			dst.rotate = src.rotation;
			dst.translate = src.position;

			if (dst.parent != nullptr)
			{
				dst.parent->children.emplace_back(&dst);
			}
		}
	}

	// メッシュ
	{
		const std::vector<ModelResource::Mesh>& res_meshes = m_resource->GetMeshes();

		m_meshes.resize(res_meshes.size());
		for (size_t mesh_index = 0; mesh_index < m_meshes.size(); ++mesh_index)
		{
			auto&& src_mesh = res_meshes.at(mesh_index);
			auto&& dst_mesh = m_meshes.at(mesh_index);

			dst_mesh.mesh = &src_mesh;
			dst_mesh.vertex_count = RoundUp(SkinningCSThreadNum, static_cast<UINT>(src_mesh.vertices.size()));
			dst_mesh.frame_resources.resize(graphics.GetBufferCount());

			// フレームリソース
			for (Mesh::FrameResource& frame_resource : dst_mesh.frame_resources)
			{
				UINT bone_num = static_cast<UINT>(src_mesh.bones.size());
				if (bone_num == 0) bone_num = 1;

				// ヒーププロパティの設定
				D3D12_HEAP_PROPERTIES d3d_heap_props{};
				d3d_heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
				d3d_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
				d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
				d3d_heap_props.CreationNodeMask = 1;
				d3d_heap_props.VisibleNodeMask = 1;

				// リソースの設定
				D3D12_RESOURCE_DESC d3d_resource_desc{};
				d3d_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
				d3d_resource_desc.Alignment = 0;
				d3d_resource_desc.Width = ((sizeof(DirectX::XMFLOAT4X4) * bone_num) + 255) & ~255;	// 256バイトアライメントにする
				d3d_resource_desc.Height = 1;
				d3d_resource_desc.DepthOrArraySize = 1;
				d3d_resource_desc.MipLevels = 1;
				d3d_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
				d3d_resource_desc.SampleDesc.Count = 1;
				d3d_resource_desc.SampleDesc.Quality = 0;
				d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
				d3d_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

				// 定数バッファの生成
				hr = d3d_device->CreateCommittedResource(
					&d3d_heap_props,
					D3D12_HEAP_FLAG_NONE,
					&d3d_resource_desc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(frame_resource.d3d_cbv_resource.GetAddressOf())
				);
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
				frame_resource.d3d_cbv_resource->SetName(L"ModelConstantBuffer");

				// ディスクリプタ取得
				frame_resource.cbv_descriptor = graphics.GetShaderResourceDescriptorHeap()->PopDescriptor();

				// 定数バッファビューの生成
				D3D12_CONSTANT_BUFFER_VIEW_DESC d3d_cbv_desc;
				d3d_cbv_desc.BufferLocation = frame_resource.d3d_cbv_resource->GetGPUVirtualAddress();
				d3d_cbv_desc.SizeInBytes = static_cast<UINT>(d3d_resource_desc.Width);
				d3d_device->CreateConstantBufferView(
					&d3d_cbv_desc,
					frame_resource.cbv_descriptor->GetCpuHandle()
				);

				// マップしておく
				hr = frame_resource.d3d_cbv_resource->Map(0, nullptr, reinterpret_cast<void**>(&frame_resource.cbv_data));
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

				//スキニング計算用頂点バッファ
				if (dst_mesh.mesh->bones.size() > 0)
				{
					UINT stride = sizeof(ModelResource::Vertex);
					UINT buffer_size = static_cast<UINT>(stride * dst_mesh.vertex_count);

					// ヒーププロパティの設定
					D3D12_HEAP_PROPERTIES d3d_heap_props{};
					d3d_heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
					d3d_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
					d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
					d3d_heap_props.CreationNodeMask = 1;
					d3d_heap_props.VisibleNodeMask = 1;

					// リソースの設定
					D3D12_RESOURCE_DESC d3d_resource_desc{};
					d3d_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
					d3d_resource_desc.Alignment = 0;
					d3d_resource_desc.Width = ((sizeof(DirectX::XMFLOAT4X4) * bone_num) + 255) & ~255;	// 256バイトアライメントにする
					d3d_resource_desc.Height = 1;
					d3d_resource_desc.DepthOrArraySize = 1;
					d3d_resource_desc.MipLevels = 1;
					d3d_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
					d3d_resource_desc.SampleDesc.Count = 1;
					d3d_resource_desc.SampleDesc.Quality = 0;
					d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
					d3d_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

					// 定数バッファの生成
					hr = d3d_device->CreateCommittedResource(
						&d3d_heap_props,
						D3D12_HEAP_FLAG_NONE,
						&d3d_resource_desc,
						D3D12_RESOURCE_STATE_GENERIC_READ,
						nullptr,
						IID_PPV_ARGS(frame_resource.d3d_vbv_uav_resource.GetAddressOf()));
					_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
					frame_resource.d3d_vbv_uav_resource->SetName(L"VertexBuffer");

					// 頂点バッファビュー設定
					frame_resource.d3d_vbv.BufferLocation = frame_resource.d3d_vbv_uav_resource->GetGPUVirtualAddress();
					frame_resource.d3d_vbv.SizeInBytes = buffer_size;
					frame_resource.d3d_vbv.StrideInBytes = stride;

					// ディスクリプタ取得
					frame_resource.uav_descriptor = graphics.GetShaderResourceDescriptorHeap()->PopDescriptor();

					// アンオーダードアクセスビューの設定
					D3D12_UNORDERED_ACCESS_VIEW_DESC d3d_uav_desc{};
					d3d_uav_desc.Format = DXGI_FORMAT_UNKNOWN;
					d3d_uav_desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
					d3d_uav_desc.Buffer.FirstElement = 0;
					d3d_uav_desc.Buffer.NumElements = dst_mesh.vertex_count;
					d3d_uav_desc.Buffer.StructureByteStride = stride;
					d3d_uav_desc.Buffer.CounterOffsetInBytes = 0;
					d3d_uav_desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

					// アンオーダードアクセスビューの生成
					d3d_device->CreateUnorderedAccessView(
						frame_resource.d3d_vbv_uav_resource.Get(),
						nullptr,
						&d3d_uav_desc,
						frame_resource.uav_descriptor->GetCpuHandle());
				}
			}
		}
	}

	// 行列計算
	const DirectX::XMFLOAT4X4 transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	UpdateTransform(transform);
}

ModelDX12::~ModelDX12()
{
	Graphics& graphics = Graphics::Instance();
	DescriptorHeap* descriptor_heap = graphics.GetShaderResourceDescriptorHeap();

	for (Mesh& mesh : m_meshes)
	{
		for (Mesh::FrameResource& frame_resource : mesh.frame_resources)
		{
			if (frame_resource.uav_descriptor != nullptr)
			{
				descriptor_heap->PushDescriptor(frame_resource.uav_descriptor);
			}
			if (frame_resource.cbv_descriptor != nullptr)
			{
				descriptor_heap->PushDescriptor(frame_resource.cbv_descriptor);
			}
			if (frame_resource.cbv_data != nullptr)
			{
				frame_resource.d3d_cbv_resource->Unmap(0, nullptr);
			}
		}
	}
}

// 変換行列計算
void ModelDX12::UpdateTransform(const DirectX::XMFLOAT4X4& transform)
{
	DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);

	for (Node& node : m_nodes)
	{
		// ローカル行列算出
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
		DirectX::XMMATRIX LocalTransform = S * R * T;

		// ワールド行列算出
		DirectX::XMMATRIX ParentTransform;
		if (node.parent != nullptr)
		{
			ParentTransform = DirectX::XMLoadFloat4x4(&node.parent->world_transform);
		}
		else
		{
			ParentTransform = Transform;
		}
		DirectX::XMMATRIX WorldTransform = LocalTransform * ParentTransform;

		// 計算結果を格納
		DirectX::XMStoreFloat4x4(&node.local_transform, LocalTransform);
		DirectX::XMStoreFloat4x4(&node.world_transform, WorldTransform);
	}

	Graphics& graphics = Graphics::Instance();
	for (Mesh& mesh : m_meshes)
	{
		const ModelResource::Mesh* res_mesh = mesh.mesh;
		Mesh::FrameResource& frame_resource = mesh.frame_resources.at(graphics.GetCurrentBufferIndex());

		if (res_mesh->nodeIndices.size() > 0)
		{
			for (size_t i = 0; i < res_mesh->nodeIndices.size(); ++i)
			{
				DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&m_nodes.at(res_mesh->nodeIndices.at(i)).world_transform);
				DirectX::XMMATRIX OffsetTransform = DirectX::XMLoadFloat4x4(&res_mesh->offsetTransforms.at(i));
				DirectX::XMMATRIX BoneTransform = OffsetTransform * WorldTransform;
				DirectX::XMStoreFloat4x4(&frame_resource.cbv_data[i].bone_transforms[i], BoneTransform);
			}
		}
		else
		{
			frame_resource.cbv_data[0].world_transform = m_nodes.at(res_mesh->nodeIndex).world_transform;
		}
	}
}

// アニメーション再生
void ModelDX12::PlayAnimation(int animationIndex, bool loop)
{
	m_current_animation = animationIndex;
	m_loop_animation = loop;
	m_end_animation = false;
	m_current_seconds = 0.0f;
}

// アニメーション停止
void ModelDX12::StopAnimation()
{
	m_current_animation = -1;
}

// アニメーション計算
void ModelDX12::UpdateAnimation(float elapsedTime)
{
	if (m_current_animation < 0) return;

	if (m_resource->GetAnimations().empty()) return;

	if (!IsPlayAnimation()) return;

	const ModelResource::Animation& animation = m_resource->GetAnimations().at(m_current_animation);

	for (size_t nodeIndex = 0; nodeIndex < animation.nodeAnims.size(); nodeIndex++)
	{
		Node& node = m_nodes.at(nodeIndex);

		const ModelResource::NodeAnim& nodeAnim = animation.nodeAnims.at(nodeIndex);

		for (size_t index = 0; index < nodeAnim.positionKeyframes.size() - 1; index++)
		{
			const ModelResource::VectorKeyframe& keyframe0 = nodeAnim.positionKeyframes.at(index);
			const ModelResource::VectorKeyframe& keyframe1 = nodeAnim.positionKeyframes.at(index + 1);
			if (m_current_seconds >= keyframe0.seconds && m_current_seconds < keyframe1.seconds)
			{
				float rate = (m_current_seconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);
				DirectX::XMVECTOR V0 = DirectX::XMLoadFloat3(&keyframe0.value);
				DirectX::XMVECTOR V1 = DirectX::XMLoadFloat3(&keyframe1.value);
				DirectX::XMVECTOR V = DirectX::XMVectorLerp(V0, V1, rate);
				DirectX::XMStoreFloat3(&node.translate, V);
			}
		}

		for (size_t index = 0; index < nodeAnim.rotationKeyframes.size() - 1; index++)
		{
			const ModelResource::QuaternionKeyframe& keyframe0 = nodeAnim.rotationKeyframes.at(index);
			const ModelResource::QuaternionKeyframe& keyframe1 = nodeAnim.rotationKeyframes.at(index + 1);
			if (m_current_seconds >= keyframe0.seconds && m_current_seconds < keyframe1.seconds)
			{
				float rate = (m_current_seconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);
				DirectX::XMVECTOR Q0 = DirectX::XMLoadFloat4(&keyframe0.value);
				DirectX::XMVECTOR Q1 = DirectX::XMLoadFloat4(&keyframe1.value);
				DirectX::XMVECTOR Q = DirectX::XMQuaternionSlerp(Q0, Q1, rate);
				DirectX::XMStoreFloat4(&node.rotate, Q);
			}
		}
		for (size_t index = 0; index < nodeAnim.scaleKeyframes.size() - 1; index++)
		{
			const ModelResource::VectorKeyframe& keyframe0 = nodeAnim.scaleKeyframes.at(index);
			const ModelResource::VectorKeyframe& keyframe1 = nodeAnim.scaleKeyframes.at(index + 1);
			if (m_current_seconds >= keyframe0.seconds && m_current_seconds < keyframe1.seconds)
			{
				float rate = (m_current_seconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);
				DirectX::XMVECTOR V0 = DirectX::XMLoadFloat3(&keyframe0.value);
				DirectX::XMVECTOR V1 = DirectX::XMLoadFloat3(&keyframe1.value);
				DirectX::XMVECTOR V = DirectX::XMVectorLerp(V0, V1, rate);
				DirectX::XMStoreFloat3(&node.scale, V);
			}
		}
	}

	// 最終フレーム処理
	if (m_end_animation)
	{
		m_end_animation = false;
		m_current_animation = -1;
		return;
	}

	// 時間経過
	m_current_seconds += elapsedTime;
	if (m_current_seconds >= animation.secondsLength)
	{
		if (m_loop_animation)
		{
			m_current_seconds -= animation.secondsLength;
		}
		else
		{
			m_current_seconds = animation.secondsLength;
			m_end_animation = true;
		}
	}
}

void ModelDX12::ComputeAnimation(float elapsedTime)
{
}

void ModelDX12::ComputeBlending(float elapsedTime)
{
}

void ModelDX12::ComputeWorldBounds()
{
}
#include "ModelDX12.h"
#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/Graphics.h"
#include "TAKOEngine/Rendering/ResourceManager.h"

// コンストラクタ
ModelDX12::ModelDX12(ID3D12Device* device, const char* filename, float scaling, int modelType) :scaling(scaling)
{
	Graphics& graphics = Graphics::Instance();

	HRESULT hr = S_OK;

	// リソース読み込み
	m_resource = ResourceManager::Instance().LoadModelDX12Resource(filename);

	// ノード
	{
		const std::vector<ModelResource::Node>& res_nodes = m_resource->GetNodes();
		nodeCaches.resize(res_nodes.size());
		m_nodes.resize(res_nodes.size());
		for (size_t nodeIndex = 0; nodeIndex < m_nodes.size(); ++nodeIndex)
		{
			auto&& src = res_nodes.at(nodeIndex);
			auto&& dst = m_nodes.at(nodeIndex);

			dst.name = src.name.c_str();
			dst.parent = src.parentIndex >= 0 ? &m_nodes.at(src.parentIndex) : nullptr;
			dst.scale = src.scale;
			dst.rotation = src.rotation;
			dst.position = src.position;

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

			dst_mesh.mesh         = &src_mesh;
			dst_mesh.vertex_count = RoundUp(SkinningCSThreadNum, static_cast<UINT>(src_mesh.vertices.size()));
			dst_mesh.node         = &m_nodes.at(src_mesh.nodeIndex);
			dst_mesh.frame_resources.resize(graphics.GetBufferCount());

			//ボーン
			dst_mesh.bones.resize(src_mesh.nodeIndices.size());
			for (size_t bone_index = 0; bone_index < dst_mesh.bones.size(); ++bone_index)
			{
				auto& dst_bone            = dst_mesh.bones.at(bone_index);
				dst_bone.node             = &m_nodes.at(src_mesh.nodeIndices.at(bone_index));
				dst_bone.offset_transform = src_mesh.offsetTransforms.at(bone_index);
			}

			// フレームリソース
			for (Mesh::FrameResource& frame_resource : dst_mesh.frame_resources)
			{
				UINT buffer_size = (sizeof(Mesh::Constants) + 255) & ~255;

				// ヒーププロパティの設定
				D3D12_HEAP_PROPERTIES d3d_heap_props{};
				d3d_heap_props.Type                 = D3D12_HEAP_TYPE_UPLOAD;
				d3d_heap_props.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
				d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
				d3d_heap_props.CreationNodeMask     = 1;
				d3d_heap_props.VisibleNodeMask      = 1;

				// リソースの設定
				D3D12_RESOURCE_DESC d3d_resource_desc{};
				d3d_resource_desc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
				d3d_resource_desc.Alignment          = 0;
				d3d_resource_desc.Width              = buffer_size;
				d3d_resource_desc.Height             = 1;
				d3d_resource_desc.DepthOrArraySize   = 1;
				d3d_resource_desc.MipLevels          = 1;
				d3d_resource_desc.Format             = DXGI_FORMAT_UNKNOWN;
				d3d_resource_desc.SampleDesc.Count   = 1;
				d3d_resource_desc.SampleDesc.Quality = 0;
				d3d_resource_desc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
				d3d_resource_desc.Flags              = D3D12_RESOURCE_FLAG_NONE;

				// 定数バッファの生成
				hr = device->CreateCommittedResource(
					&d3d_heap_props,
					D3D12_HEAP_FLAG_NONE,
					&d3d_resource_desc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(frame_resource.d3d_cbv_resource.GetAddressOf()));
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
				frame_resource.d3d_cbv_resource->SetName(L"ModelConstantBuffer");

				// ディスクリプタ取得
				frame_resource.cbv_descriptor = graphics.GetShaderResourceDescriptorHeap()->PopDescriptor();

				// 定数バッファビューの生成
				D3D12_CONSTANT_BUFFER_VIEW_DESC d3d_cbv_desc;
				d3d_cbv_desc.BufferLocation = frame_resource.d3d_cbv_resource->GetGPUVirtualAddress();
				d3d_cbv_desc.SizeInBytes    = buffer_size;
				device->CreateConstantBufferView(
					&d3d_cbv_desc,
					frame_resource.cbv_descriptor->GetCpuHandle());

				// マップしておく
				hr = frame_resource.d3d_cbv_resource->Map(0, nullptr, reinterpret_cast<void**>(&frame_resource.cbv_data));
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

				//スキニング計算用頂点バッファ
				if (dst_mesh.bones.size() > 0)
				{
					UINT stride = sizeof(ModelResource::Vertex);
					UINT buffer_size = static_cast<UINT>(stride * dst_mesh.vertex_count);

					// ヒーププロパティの設定
					D3D12_HEAP_PROPERTIES d3d_heap_props{};
					d3d_heap_props.Type                 = D3D12_HEAP_TYPE_DEFAULT;
					d3d_heap_props.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
					d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
					d3d_heap_props.CreationNodeMask     = 1;
					d3d_heap_props.VisibleNodeMask      = 1;

					// リソースの設定
					D3D12_RESOURCE_DESC d3d_resource_desc{};
					d3d_resource_desc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
					d3d_resource_desc.Alignment          = 0;
					d3d_resource_desc.Width              = buffer_size;
					d3d_resource_desc.Height             = 1;
					d3d_resource_desc.DepthOrArraySize   = 1;
					d3d_resource_desc.MipLevels          = 1;
					d3d_resource_desc.Format             = DXGI_FORMAT_UNKNOWN;
					d3d_resource_desc.SampleDesc.Count   = 1;
					d3d_resource_desc.SampleDesc.Quality = 0;
					d3d_resource_desc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
					d3d_resource_desc.Flags              = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

					// 定数バッファの生成
					hr = device->CreateCommittedResource(
						&d3d_heap_props,
						D3D12_HEAP_FLAG_NONE,
						&d3d_resource_desc,
						D3D12_RESOURCE_STATE_COMMON,
						nullptr,
						IID_PPV_ARGS(frame_resource.d3d_vbv_uav_resource.GetAddressOf()));
					_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
					frame_resource.d3d_vbv_uav_resource->SetName(L"VertexBuffer");

					// 頂点バッファビュー設定
					frame_resource.d3d_vbv.BufferLocation = frame_resource.d3d_vbv_uav_resource->GetGPUVirtualAddress();
					frame_resource.d3d_vbv.SizeInBytes    = buffer_size;
					frame_resource.d3d_vbv.StrideInBytes  = stride;

					// ディスクリプタ取得
					frame_resource.uav_descriptor = graphics.GetShaderResourceDescriptorHeap()->PopDescriptor();

					// アンオーダードアクセスビューの設定
					D3D12_UNORDERED_ACCESS_VIEW_DESC d3d_uav_desc{};
					d3d_uav_desc.Format                      = DXGI_FORMAT_UNKNOWN;
					d3d_uav_desc.ViewDimension               = D3D12_UAV_DIMENSION_BUFFER;
					d3d_uav_desc.Buffer.FirstElement         = 0;
					d3d_uav_desc.Buffer.NumElements          = dst_mesh.vertex_count;
					d3d_uav_desc.Buffer.StructureByteStride  = stride;
					d3d_uav_desc.Buffer.CounterOffsetInBytes = 0;
					d3d_uav_desc.Buffer.Flags                = D3D12_BUFFER_UAV_FLAG_NONE;

					// アンオーダードアクセスビューの生成
					device->CreateUnorderedAccessView(
						frame_resource.d3d_vbv_uav_resource.Get(),
						nullptr,
						&d3d_uav_desc,
						frame_resource.uav_descriptor->GetCpuHandle());
				}
			}
		}
	}
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
void ModelDX12::UpdateTransform(const DirectX::XMFLOAT4X4& worldTransform)
{
	for (Node& node : m_nodes)
	{
		// ローカル行列算出
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotation));
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.position.x, node.position.y, node.position.z);
		DirectX::XMMATRIX LocalTransform = S * R * T;
		DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);

		// ワールド行列算出
		if (node.parent != nullptr)
		{
			DirectX::XMMATRIX ParentTransform = DirectX::XMLoadFloat4x4(&node.parent->globalTransform);
			DirectX::XMMATRIX GlobalTransform = LocalTransform * ParentTransform;
			DirectX::XMStoreFloat4x4(&node.globalTransform, GlobalTransform);
		}
		else
		{
			DirectX::XMStoreFloat4x4(&node.globalTransform, LocalTransform);
		}
	}

	// バウンディングボックス計算
	ComputeWorldBounds();

	// フレームリソース更新処理
	UpdateFrameResource(worldTransform);
}

// フレームリソース更新処理
void ModelDX12::UpdateFrameResource(const DirectX::XMFLOAT4X4 transform)
{
	Graphics& graphics = Graphics::Instance();
	DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&transform);

	//メッシュの更新
	for (Mesh& mesh : m_meshes)
	{
		Mesh::FrameResource& frame_resource = mesh.frame_resources.at(graphics.GetCurrentBufferIndex());

		if (mesh.bones.size() > 0)
		{
			for (size_t bone_index = 0; bone_index < mesh.bones.size(); ++bone_index)
			{
				const Bone& bone = mesh.bones.at(bone_index);
				DirectX::XMMATRIX GlobalTransform = DirectX::XMLoadFloat4x4(&bone.node->globalTransform);
				DirectX::XMMATRIX OffsetTransform = DirectX::XMLoadFloat4x4(&bone.offset_transform);
				DirectX::XMMATRIX BoneTransform   = OffsetTransform * GlobalTransform;
				DirectX::XMStoreFloat4x4(&frame_resource.cbv_data->bone_transforms[bone_index], BoneTransform);
			}
			frame_resource.cbv_data->world_transform = transform;
		}
		else
		{
			DirectX::XMMATRIX GlobalTransform = DirectX::XMLoadFloat4x4(&mesh.node->globalTransform);
			DirectX::XMStoreFloat4x4(&frame_resource.cbv_data->world_transform, GlobalTransform * WorldTransform);
		}
		
		bool updateBuffers = true; 
		for (const ModelResource::Subset& subset : mesh.mesh->subsets)
		{
			if (updateBuffers)
			{
				frame_resource.instancingCount = 0; 
				for (int i = 0; i < InstancingMax; ++i)
				{
					if (!exist[i])  continue;

					frame_resource.cbv_data->transform[frame_resource.instancingCount++] = m_transform[i];
				}
				updateBuffers = false;
			}
		}
	} 
}

// デバック
void ModelDX12::DrawDebugGUI()
{
}

// バウンディングボックス計算
void ModelDX12::ComputeWorldBounds()
{
	Graphics& graphics = Graphics::Instance(); 

	// バウンディングボックス
	m_bounds.Center = m_bounds.Extents = { 0, 0, 0 };
	for (Mesh& mesh : m_meshes) 
	{
		const Mesh::FrameResource& frame_resource = mesh.frame_resources.at(graphics.GetCurrentBufferIndex());

		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&frame_resource.cbv_data->world_transform);
		mesh.mesh->localBounds.Transform(mesh.worldBounds, WorldTransform); 
		DirectX::BoundingBox::CreateMerged(m_bounds, m_bounds, mesh.worldBounds);
	}
}

// ノード検索
ModelDX12::Node* ModelDX12::FindNode(const char* name)
{
	// 全てのノードを総当たりで名前比較する
	int nodeCount = static_cast<int>(m_nodes.size());
	for (Node& node : m_nodes)
	{
		if (node.name == name)
		{
			return &node;
		}
	}

	// 見つからなかった
	return nullptr;
}

// ノードコピー
void ModelDX12::CopyNodes(iModel* model)
{
	m_resource->SetNodes(model->GetResource()->GetNodes());
}

// アニメーション再生中か
bool ModelDX12::IsPlayAnimation() const
{
	if (m_current_animation_Index < 0) return false;
	if (m_current_animation_Index > m_resource->GetAnimations().size()) return false;

	return m_playing_animation;
}

// アニメーション再生
void ModelDX12::PlayAnimation(int animationIndex, bool loop, float blendSeconds)
{
	m_current_animation_Index = animationIndex;
	m_current_seconds = 0;
	m_loop_animation = loop;
	m_playing_animation = true;

	// ブレンドパラーメータ
	m_animationBlending = blendSeconds > 0.0f;
	m_currentAnimationBlendSeconds = 0.0f;
	m_animationBlendSecondsLength = blendSeconds;

	// 現在の姿勢をキャッシュする
	for (size_t i = 0; i < nodes.size(); ++i)
	{
		const ModelDX12::Node& src = nodes.at(i);
		NodeCache& dst = nodeCaches.at(i);

		dst.position = src.position;
		dst.rotation = src.rotation;
		dst.scale = src.scale;
	}
}

// アニメーション停止
void ModelDX12::StopAnimation()
{
	m_current_animation_Index = -1;
}

// アニメーション計算
void ModelDX12::UpdateAnimation(float elapsedTime)
{
	if (m_current_animation_Index < 0) return;

	if (m_resource->GetAnimations().empty()) return;

	ComputeAnimation(elapsedTime);
	ComputeBlending(elapsedTime);
}

// アニメーション計算処理
void ModelDX12::ComputeAnimation(float elapsedTime)
{
	if (!IsPlayAnimation()) return;

	// 指定のアニメーションデータ処理
	const ModelResource::Animation& animation = m_resource->GetAnimations().at(m_current_animation_Index);

	// ノード毎のアニメーションデータ処理
	for (size_t nodeIndex = 0; nodeIndex < animation.nodeAnims.size(); nodeIndex++)
	{
		Node& node = m_nodes.at(nodeIndex);

		const ModelResource::NodeAnim& nodeAnim = animation.nodeAnims.at(nodeIndex);

		// 位置
		for (size_t index = 0; index < nodeAnim.positionKeyframes.size() - 1; index++)
		{
			//現在の時間がどのキーフレームの間にいるか判明する
			const ModelResource::VectorKeyframe& keyframe0 = nodeAnim.positionKeyframes.at(index);
			const ModelResource::VectorKeyframe& keyframe1 = nodeAnim.positionKeyframes.at(index + 1);
			if (m_current_seconds >= keyframe0.seconds && m_current_seconds < keyframe1.seconds)
			{
				// 再生時間とキーフレームの時間から補完率を算出する
				float rate = (m_current_seconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

				// 前のキーフレームと次のキーフレームの姿勢を補完
				DirectX::XMVECTOR V0 = DirectX::XMLoadFloat3(&keyframe0.value);
				DirectX::XMVECTOR V1 = DirectX::XMLoadFloat3(&keyframe1.value);
				DirectX::XMVECTOR V = DirectX::XMVectorLerp(V0, V1, rate);

				// 計算結果をノードに格納
				DirectX::XMStoreFloat3(&node.position, V);
			}
		}
		// 回転
		for (size_t index = 0; index < nodeAnim.rotationKeyframes.size() - 1; index++)
		{
			//現在の時間がどのキーフレームの間にいるか判明する
			const ModelResource::QuaternionKeyframe& keyframe0 = nodeAnim.rotationKeyframes.at(index);
			const ModelResource::QuaternionKeyframe& keyframe1 = nodeAnim.rotationKeyframes.at(index + 1);
			if (m_current_seconds >= keyframe0.seconds && m_current_seconds < keyframe1.seconds)
			{
				// 再生時間とキーフレームの時間から補完率を算出する
				float rate = (m_current_seconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);
				
				// 前のキーフレームと次のキーフレームの姿勢を補完
				DirectX::XMVECTOR Q0 = DirectX::XMLoadFloat4(&keyframe0.value);
				DirectX::XMVECTOR Q1 = DirectX::XMLoadFloat4(&keyframe1.value);
				DirectX::XMVECTOR Q = DirectX::XMQuaternionSlerp(Q0, Q1, rate);

				// 計算結果をノードに格納
				DirectX::XMStoreFloat4(&node.rotation, Q);
			}
		}
		// スケール
		for (size_t index = 0; index < nodeAnim.scaleKeyframes.size() - 1; index++)
		{
			//現在の時間がどのキーフレームの間にいるか判明する
			const ModelResource::VectorKeyframe& keyframe0 = nodeAnim.scaleKeyframes.at(index);
			const ModelResource::VectorKeyframe& keyframe1 = nodeAnim.scaleKeyframes.at(index + 1);
			if (m_current_seconds >= keyframe0.seconds && m_current_seconds < keyframe1.seconds)
			{
				// 再生時間とキーフレームの時間から補完率を算出する
				float rate = (m_current_seconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);
				
				// 前のキーフレームと次のキーフレームの姿勢を補完
				DirectX::XMVECTOR V0 = DirectX::XMLoadFloat3(&keyframe0.value);
				DirectX::XMVECTOR V1 = DirectX::XMLoadFloat3(&keyframe1.value);
				DirectX::XMVECTOR V = DirectX::XMVectorLerp(V0, V1, rate);

				// 計算結果をノードに格納
				DirectX::XMStoreFloat3(&node.scale, V);
			}
		}
	}

	// 時間経過
	m_current_seconds += elapsedTime;

	// 再生時間が終端時間を超えたら
	if (m_current_seconds >= animation.secondsLength)
	{
		if (m_loop_animation)
		{
			// 再生時間を巻き戻す
			m_current_seconds -= animation.secondsLength;
		}
		else
		{
			// 再生終了時間にする
			m_current_seconds = animation.secondsLength;
			m_playing_animation = false;
		}
	}
}

// ブレンディング計算処理
void ModelDX12::ComputeBlending(float elapsedTime)
{
	if (m_animationBlending) return;

	// ブレンド率の計算
	float rate = m_currentAnimationBlendSeconds / m_animationBlendSecondsLength;

	// ブレンド計算
	int count = static_cast<int>(m_nodes.size());
	for (int i = 0; i < count; i++)
	{
		const NodeCache& cache = nodeCaches.at(i);
		Node& node = m_nodes.at(i);

		DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&cache.scale);
		DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&node.scale);
		DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&cache.rotation);
		DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&node.rotation);
		DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&cache.position);
		DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&node.position);

		DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, rate);
		DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, rate);
		DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, rate);

		DirectX::XMStoreFloat3(&node.scale, S);
		DirectX::XMStoreFloat4(&node.rotation, R);
		DirectX::XMStoreFloat3(&node.position, T);
	}

	// 時間経過
	m_currentAnimationBlendSeconds += elapsedTime;
	if (m_currentAnimationBlendSeconds >= m_animationBlendSecondsLength)
	{
		m_currentAnimationBlendSeconds = m_animationBlendSecondsLength;
		m_animationBlending = false;
	}
}

// アニメーションコピー
void ModelDX12::CopyAnimations(iModel* model)
{
	m_resource->SetAnimations(model->GetResource()->GetAnimations());
}

//割り当てられた番号を返す
int ModelDX12::AllocateInstancingIndex()
{
	for (int i = 0; i < InstancingMax; ++i)
	{
		if (!exist[i])
		{
			exist[i] = true;
			return i;
		}
	}
	return -1;
}

//割り当てられた番号を解放する
void ModelDX12::FreeInstancingIndex(int instancingIndex)
{
	if (0 <= instancingIndex && instancingIndex < InstancingMax)
		exist[instancingIndex] = false;
}

//行列計算
void ModelDX12::UpdateTransform(int instancingIndex, const DirectX::XMFLOAT4X4& transform)
{
	if (0 <= instancingIndex && instancingIndex < InstancingMax)
		this->m_transform[instancingIndex] = transform;
}

//現在の姿勢行列を取得
const DirectX::XMFLOAT4X4& ModelDX12::GetTransform(int instancingIndex) const
{
	if (0 <= instancingIndex && instancingIndex < InstancingMax)
		return m_transform[instancingIndex];
	return m_transform[0];
}

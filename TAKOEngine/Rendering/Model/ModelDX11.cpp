#include <filesystem>
#include <fstream>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include "TAKOEngine/Rendering/Misc.h"
#include "ModelDX11.h"
#include "TAKOEngine/Rendering/ResourceManager.h"

// コンストラクタ
ModelDX11::ModelDX11(ID3D11Device* device, const char* filename, float scaling, int modelType) : scaling(scaling)
{
	resource = ResourceManager::Instance().LoadModelResource(filename);
	
	// モデルの名前設定
	extractBaseName(filename);

	// ノードキャッシュ
	const std::vector<ModelResource::Node>& resNodes = resource->GetNodes();
	nodeCaches.resize(resNodes.size());
	nodes.resize(resNodes.size());
	for (size_t nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex)
	{
		auto&& src = resNodes.at(nodeIndex);
		auto&& dst = nodes.at(nodeIndex);

		dst.name = src.name.c_str();
		dst.scale = src.scale;
		dst.rotation = src.rotation;
		dst.position = src.position;

		dst.parent = src.parentIndex >= 0 ? &nodes.at(src.parentIndex) : nullptr;
		if (dst.parent != nullptr)
		{
			dst.parent->children.emplace_back(&dst);
		}
	}

	//メッシュ構築
	const std::vector<ModelResource::Mesh>& resMeshes = resource->GetMeshes();
	m_meshes.resize(resMeshes.size());
	for (size_t meshIndex = 0; meshIndex < m_meshes.size(); ++meshIndex)
	{
		const ModelResource::Mesh& resMesh = resMeshes.at(meshIndex);
		Mesh& mesh = m_meshes.at(meshIndex);
		mesh.mesh = &resMesh;
	}
}

ModelDX11::~ModelDX11()
{
}

// アニメーション再生
void ModelDX11::PlayAnimation(int index, bool loop, float blendSeconds)
{
	currentAnimationIndex = index;
	currentAnimationSeconds = 0;
	animationLoop = loop;
	animationPlaying = true;

	// ブレンドパラメータ
	animationBlending = blendSeconds > 0.0f;
	currentAnimationBlendSeconds = 0.0f;
	animationBlendSecondsLength = blendSeconds;

	// 現在の姿勢をキャッシュする
	for (size_t i = 0; i < nodes.size(); ++i)
	{
		const ModelDX11::Node& src = nodes.at(i);
		NodeCache& dst = nodeCaches.at(i);

		dst.position = src.position;
		dst.rotation = src.rotation;
		dst.scale = src.scale;
	}
}

// アニメーション再生中か
bool ModelDX11::IsPlayAnimation() const
{
	if (currentAnimationIndex < 0) return false;
	if (currentAnimationIndex > resource->GetAnimations().size()) return false;
	return animationPlaying;
}

// アニメーション更新処理
void ModelDX11::UpdateAnimation(float elapsedTime)
{
	ComputeAnimation(elapsedTime);
	ComputeBlending(elapsedTime);
}

void ModelDX11::CopyAnimations(iModel* model)
{
	resource->SetAnimations(model->GetResource()->GetAnimations());
}

void ModelDX11::CopyNodes(iModel* model)
{
	resource->SetNodes(model->GetResource()->GetNodes());
}

void ModelDX11::ComputeAnimation(float elapsedTime)
{
	if (!IsPlayAnimation()) return;

	// 指定のアニメーションデータを取得
	const ModelResource::Animation& animation = resource->GetAnimations().at(currentAnimationIndex);

	// ノード毎のアニメーションデータ処理
	for (size_t nodeIndex = 0; nodeIndex < animation.nodeAnims.size(); nodeIndex++)
	{
		Node& node = nodes.at(nodeIndex);

		const ModelResource::NodeAnim& nodeAnim = animation.nodeAnims.at(nodeIndex);

		// 位置
		for (size_t index = 0; index < nodeAnim.positionKeyframes.size() - 1; index++)
		{
			//現在の時間がどのキーフレームの間にいるか判明する
			const ModelResource::VectorKeyframe& keyframe0 = nodeAnim.positionKeyframes.at(index);
			const ModelResource::VectorKeyframe& keyframe1 = nodeAnim.positionKeyframes.at(index + 1);
			if (currentAnimationSeconds >= keyframe0.seconds && currentAnimationSeconds < keyframe1.seconds)
			{
				// 再生時間とキーフレームの時間から補完率を算出する
				float rate = (currentAnimationSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);
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
			if (currentAnimationSeconds >= keyframe0.seconds && currentAnimationSeconds < keyframe1.seconds)
			{
				// 再生時間とキーフレームの時間から補完率を算出する
				float rate = (currentAnimationSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);
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
			if (currentAnimationSeconds >= keyframe0.seconds && currentAnimationSeconds < keyframe1.seconds)
			{
				// 再生時間とキーフレームの時間から補完率を算出する
				float rate = (currentAnimationSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);
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
	currentAnimationSeconds += elapsedTime;

	// 再生時間が終端時間を超えたら
	if (currentAnimationSeconds >= animation.secondsLength)
	{
		if (animationLoop)
		{
			// 再生時間を巻き戻す
			currentAnimationSeconds -= animation.secondsLength;
		}
		else
		{
			// 再生終了時間にする
			currentAnimationSeconds = animation.secondsLength;
			animationPlaying = false;
		}
	}
}

// ブレンディング計算処理
void ModelDX11::ComputeBlending(float elapsedTime)
{
	if (!animationBlending) return;

	// ブレンド率の計算
	float rate = currentAnimationBlendSeconds / animationBlendSecondsLength;
	// ブレンド計算
	int count = static_cast<int>(nodes.size());
	for (int i = 0; i < count; i++)
	{
		const NodeCache& cache = nodeCaches.at(i);
		Node& node = nodes.at(i);

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
	currentAnimationBlendSeconds += elapsedTime;
	if (currentAnimationBlendSeconds >= animationBlendSecondsLength)
	{
		currentAnimationBlendSeconds = animationBlendSecondsLength;
		animationBlending = false;
	}
}

//トランスフォーム更新処理
void ModelDX11::UpdateTransform(const DirectX::XMFLOAT4X4& worldTransform)
{
	DirectX::XMMATRIX ParentWorldTransform = DirectX::XMLoadFloat4x4(&worldTransform);

	// 右手座標系から左手座標系へ変換する行列
	DirectX::XMMATRIX CoordinateSystemTransform = DirectX::XMMatrixScaling(-scaling, scaling, scaling);

	for (Node& node : nodes)
	{
		if (!node.visible) node.scale = {};

		// ローカル行列算出
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(node.rotation.x, node.rotation.y, node.rotation.z, node.rotation.w));
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.position.x, node.position.y, node.position.z);
		DirectX::XMMATRIX LocalTransform = S * R * T;

		// グローバル行列算出
		DirectX::XMMATRIX ParentGlobalTransform;
		if (node.parent != nullptr)
		{
			ParentGlobalTransform = DirectX::XMLoadFloat4x4(&node.parent->globalTransform);
		}
		else
		{
			ParentGlobalTransform = DirectX::XMMatrixIdentity();
		}
		DirectX::XMMATRIX GlobalTransform = LocalTransform * ParentGlobalTransform;

		// ワールド行列算出
		DirectX::XMMATRIX WorldTransform = GlobalTransform * CoordinateSystemTransform * ParentWorldTransform;

		// 計算結果を格納
		DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);
		DirectX::XMStoreFloat4x4(&node.globalTransform, GlobalTransform);
		DirectX::XMStoreFloat4x4(&node.worldTransform, WorldTransform);
	}

	ComputeWorldBounds();
}

// ワールドバウンディングボックス計算
void ModelDX11::ComputeWorldBounds()
{
	// バウンディングボックス
	bounds.Center = bounds.Extents = { 0, 0, 0 };
	for (Mesh& mesh : m_meshes)
	{
		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.mesh->nodeIndex).worldTransform);
		mesh.mesh->localBounds.Transform(mesh.worldBounds, WorldTransform);
		DirectX::BoundingBox::CreateMerged(bounds, bounds, mesh.worldBounds);
	}
}

ModelDX11::Node* ModelDX11::FindNode(const char* name)
{
	// 全てのノードを総当たりで名前比較する
	int nodeCount = static_cast<int>(nodes.size());
	for (Node& node : nodes)
	{
		if (node.name == name)
		{
			return &node;
		}
	}
	// 見つからなかった
	return nullptr;
}

//デバッグ情報
void ModelDX11::DrawDebugGUI()
{
}
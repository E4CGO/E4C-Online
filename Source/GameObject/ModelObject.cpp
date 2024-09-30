#include "ModelObject.h"

#include "TAKOEngine/Physics/UnrotatedBoxCollider.h"
#include "TAKOEngine/Physics/BoundingBoxCollider.h"
#include "TAKOEngine/Physics/SphereCollider.h"
#include "TAKOEngine/Physics/ModelCollider.h"
#include "TAKOEngine/Physics/MapCollider.h"

#include "TAKOEngine/Rendering/Model/ModelDX11.h"
#include "TAKOEngine/Rendering/Model/NewModelDX11.h"
#include "TAKOEngine/Rendering/Model/ModelDX12.h"

ModelObject::ModelObject(const char* filename, float scaling, std::string renderMode)
{
	if(renderMode == "DX11")
	model = std::make_unique<ModelDX11>(T_GRAPHICS.GetDevice(), filename, scaling);

	if(renderMode == "DX11GLTF")
	//model = std::make_unique<gltf_model>(T_GRAPHICS.GetDevice(), filename);

	if(renderMode == "DX12");
}

void ModelObject::SetShader(ModelShaderId id)
{
	shaderId = id;
}

void ModelObject::SetAnimation(int index, bool loop, float blendSeconds)
{
	model->PlayAnimation(index, loop, blendSeconds);
}

void ModelObject::Update(float elapsedTime)
{
	// 行列更新
	UpdateTransform();

	// アニメーション更新
	model->UpdateAnimation(elapsedTime * animationSpeed);

	// トランスフォーム更新
	model->UpdateTransform(transform);
}

void ModelObject::Render(const RenderContext& rc)
{
	if (!visible) return;
	
	// 描画
	ModelShader* shader = T_GRAPHICS.GetModelShader(shaderId);
	shader->Begin(rc);
	shader->Draw(rc, model.get(), color);
	shader->End(rc);
}

void ModelObject::SetCollider(Collider::COLLIDER_TYPE collider)
{
	switch (collider)
	{
	case Collider::COLLIDER_TYPE::UNROTATED_BOX:
		this->collider = std::make_unique<UnrotatedBoxCollider>();
		break;
	case Collider::COLLIDER_TYPE::MODEL:
		this->collider = std::make_unique<ModelCollider>(model.get());
		break;
	case Collider::COLLIDER_TYPE::SPHERE:
		this->collider = std::make_unique<SphereCollider>();
		break;
	case Collider::COLLIDER_TYPE::BOUNDING_BOX:
		this->collider = std::make_unique<BoundingBoxCollider>(model.get());
		break;
	case Collider::COLLIDER_TYPE ::MAP:
		//this->collider = std::make_unique<ModelCollider>(model.get());
		this->collider = std::make_unique<MapCollider>(model.get());
		break;
	default:
		this->collider = nullptr;
		break;
	}
}

DirectX::XMFLOAT3 ModelObject::GetNodePosition(const char* nodeName, const DirectX::XMFLOAT3& offset)
{
	DirectX::XMFLOAT3 pos = {};
	iModel::Node* node = model->FindNode(nodeName);
	if (node == nullptr) return pos;

	DirectX::XMVECTOR Offset = DirectX::XMLoadFloat3(&offset);
	DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&node->worldTransform);
	DirectX::XMStoreFloat3(&pos, DirectX::XMVector3Transform(Offset, W));

	return pos;
}

DirectX::XMFLOAT3 ModelObject::GetNodePosition(const DirectX::XMFLOAT3& offset)
{
	DirectX::XMFLOAT3 pos = {};
	iModel::Node* node = model->GetRootNode();

	DirectX::XMVECTOR Offset = DirectX::XMLoadFloat3(&offset);
	DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&node->worldTransform);
	DirectX::XMStoreFloat3(&pos, DirectX::XMVector3Transform(Offset, W));

	return pos;
}
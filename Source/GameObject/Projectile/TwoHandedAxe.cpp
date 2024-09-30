#include "TwoHandedAxe.h"

#include "TAKOEngine/Effects/EffectManager.h"

TwoHandedAxe::TwoHandedAxe(Character* owner) : Projectile("Data/Model/Object/axe_2handed.gltf", 1.0f, owner)
{
	collisionTarget |= COLLISION_TARGET::ENEMY | COLLISION_TARGET::STAGE;

	atk = 20;
	speed = 25.0f;
	gravity = -0.3f;
	front = { 1.0f, 0.0f, 0.0f };

	power = true;
	force = 8.0f;

	// 衝突判定
	SetCollider(Collider::COLLIDER_TYPE::SPHERE);
	collider->SetScale({ 0.5f, 0.5f, 0.5f });
}

void TwoHandedAxe::Update(float elapsedTime)
{
	float Angle = rotateAngle * elapsedTime;

	iModel::Node* node = model->GetRootNode();
	DirectX::XMVECTOR Q = DirectX::XMLoadFloat4(&node->rotation);
	DirectX::XMVECTOR R = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0, 0, 1, 0), Angle);
	Q = DirectX::XMQuaternionMultiply(R, Q);
	DirectX::XMStoreFloat4(&node->rotation, Q);

	Projectile::Update(elapsedTime);

	timer -= elapsedTime;
	if (timer < 0.0f)
	{
		Destory();
	}
}

void TwoHandedAxe::DrawDebugPrimitive()
{
	DirectX::XMFLOAT3 Axis = XMFLOAT3Normalize(XMFLOAT3Corss({ 0, 1, 0 }, direction));
	T_GRAPHICS.GetLineRenderer()->AddVertex(position, { 1, 0, 0, 1 });
	T_GRAPHICS.GetLineRenderer()->AddVertex(position + Axis, { 1, 0, 0, 1 });
}

void TwoHandedAxe::UpdateColliders()
{
	collider->SetPosition(GetNodePosition(DirectX::XMFLOAT3{ 0.0f, 0.6f, 0.0f }));
}

void TwoHandedAxe::OnHitWall(HitResult& hit)
{
	position += hit.normal * hit.distance;
	direction = hit.normal;
	speed /= 2.0f;
	rotateAngle *= -1.0f;
}

void TwoHandedAxe::OnHitEnemy(HitResult& hit)
{
	EFFECTS.GetEffect(EffectManager::EFFECT_IDX::HIT_SLASH_EFFECT)->Play(hit.position, 0.4f);
}
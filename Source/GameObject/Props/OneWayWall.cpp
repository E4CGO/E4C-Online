//! @file OneWayWall.cpp
//! @note

#include "GameObject/Props/OneWayWall.h"
#include "TAKOEngine/Physics/AABBCollider.h"
#include "TAKOEngine/Physics/CollisionManager.h"

/**************************************************************************//**
	@brief		コンストラクタ
	@param[in]    scaling	スケール
	@param[in]    renderMode	レンダリング設定
*//***************************************************************************/
OneWayWall::OneWayWall(WayDirection dir, float scale, const char* filename) : ModelObject(filename, 1.0f, ModelObject::DX12), m_Scaling(scale)
{
	SetScale({ m_Scaling , m_Scaling , 1.0f});
	angle.y = DirectX::XM_PIDIV2 * static_cast<float>(dir);
	AABB trigger{ {0, 0, -1.0f}, { m_Scaling * fabsf(cosf(angle.y)) + 0.5f * fabsf(sinf(angle.y)), m_Scaling, 0.5f * fabsf(cosf(angle.y)) + m_Scaling * fabsf(sinf(angle.y)) } };
	AABB wall{ {0, 0, -0.25f}, { m_Scaling * fabsf(cosf(angle.y)) + 0.25f * fabsf(sinf(angle.y)), m_Scaling, 0.25f * fabsf(cosf(angle.y)) + m_Scaling * fabsf(sinf(angle.y))}};
	//trigger.radii = trigger.radii * 2.0f;
	//wall.radii = wall.radii * 2.0f;
	m_pColliders[0] = new AABBCollider(Collider::COLLIDER_OBJ::OBSTRUCTION, &transform);
	m_pColliders[0]->SetOwner(this);
	m_pColliders[0]->SetParam(trigger);
	m_pColliders[0]->SetHittableOBJ(Collider::COLLIDER_OBJ::PLAYER);
	m_pColliders[0]->SetCollisionFunction([&](Collider* myCol, Collider* otherCol) {CollisionTrigger(myCol, otherCol); });
	COLLISIONS.Register(m_pColliders[0]);

	m_pColliders[1] = new AABBCollider(Collider::COLLIDER_OBJ::OBSTRUCTION, &transform);
	m_pColliders[1]->SetOwner(this);
	m_pColliders[1]->SetParam(wall);
	m_pColliders[1]->SetHittableOBJ(Collider::COLLIDER_OBJ::PLAYER | Collider::COLLIDER_OBJ::ENEMY);
	m_pColliders[1]->SetCollisionFunction([&](Collider* myCol, Collider* otherCol) {CollisionWall(myCol, otherCol); });
	COLLISIONS.Register(m_pColliders[1]);

	SetShader("Plane", ModelShaderDX12Id::MagicPlane);
}

void OneWayWall::Update(float elapsedTime)
{
	m_pColliders[1]->SetHittableOBJ(Collider::COLLIDER_OBJ::PLAYER | Collider::COLLIDER_OBJ::ENEMY);
	ModelObject::Update(elapsedTime);
}

void OneWayWall::CollisionTrigger(Collider* myCol, Collider* otherCol)
{
	m_pColliders[1]->SetHittableOBJ(Collider::COLLIDER_OBJ::ENEMY);
}

void OneWayWall::CollisionWall(Collider* myCol, Collider* otherCol)
{
	HitResult hit;
	myCol->Collision(otherCol, {}, hit);
	hit.normal.y = 0;
	otherCol->GetOwner()->SetPosition(otherCol->GetOwner()->GetPosition() - hit.normal * hit.distance);
}

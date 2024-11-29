#include "Projectile.h"

#include "TAKOEngine/Physics/CollisionDataManager.h"

#include "GameObject/Projectile/ProjectileManager.h"
#include "GameObject/Character/Enemy/EnemyManager.h"
#include "GameObject/Character/Player/PlayerCharacterManager.h"

#include "Map/MapTileManager.h"

#define THRESHOLD 0.001f

void Projectile::Update(float elapsedTime)
{
	DirectX::XMFLOAT3 velocity = direction * speed;
	// 重力
	velocity.y += gravity * 60.0f * elapsedTime; // elapsedFrame;
	position += velocity * elapsedTime;

	direction = XMFLOAT3Normalize(velocity);
	ModelObject::Update(elapsedTime);
	UpdateColliders();
}

void Projectile::Render(const RenderContext& rc)
{
	ModelObject::Render(rc);
#ifdef _DEBUG
	if (collider) collider->DrawDebugPrimitive({ 1, 1, 1, 1 });
#endif // _DEBUG
}

void Projectile::PointTo(const DirectX::XMFLOAT3& target)
{
	DirectX::XMFLOAT3 targetDirect = XMFLOAT3Normalize(target - position);
	DirectX::XMMATRIX R = AnglesToMatrix({});

	// 水平角度
	DirectX::XMFLOAT3 frontH = XMFLOAT3Normalize(front - DirectX::XMFLOAT3{ 0, front.y, 0.0f });
	if (XMFLOAT3LengthSq(frontH) > 0.0f)
	{
		DirectX::XMFLOAT3 targetDirectH = XMFLOAT3Normalize(targetDirect - DirectX::XMFLOAT3{ 0.0f, targetDirect.y, 0.0f });

		float AngleH = acosf(XMFLOAT3Dot(targetDirectH, frontH));
		DirectX::XMFLOAT3 Axis = XMFLOAT3Normalize(XMFLOAT3Corss(frontH, targetDirectH));
		DirectX::XMMATRIX RotationH = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&Axis), AngleH);
		R *= RotationH;
		angle = MatrixToAngles(R);
	}

	// 垂直角度
	if (front.y != 0.0f)
	{
		DirectX::XMFLOAT3 frontV = XMFLOAT3Normalize(front - DirectX::XMFLOAT3{ front.x, 0.0f, front.z });
		float AngleV = acosf(XMFLOAT3Dot(targetDirect, frontV));
		DirectX::XMFLOAT3 Axis = XMFLOAT3Normalize(XMFLOAT3Corss(frontV, targetDirect));
		DirectX::XMMATRIX RotationV = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&Axis), AngleV);
		R *= RotationV;
		angle = MatrixToAngles(R);
	}
}

void Projectile::Destory()
{
	OnDestory();
	PROJECTILES.Remove(this);
}

void Projectile::Collision()
{
	if (collider == nullptr) return;

	if ((collisionTarget & COLLISION_TARGET::STAGE) > 0)
	{
		Collider* col = collider.get();
		for (ModelObject*& map : MAPTILES.GetAll())
		{
			HitResult hit;
			DirectX::XMFLOAT3 direction = {};
			if (map->GetCollider()->Collision(col, {}, hit))
			{
				if (fabsf(hit.normal.y) < 0.01f)
				{
					OnHitWall(hit);
				}
				else
				{
					OnHitGround(hit);
				}
			}
		}
	}
	if ((collisionTarget & COLLISION_TARGET::ENEMY) > 0)
	{
		Enemy* hitTarget = nullptr;
		int hitCollider = -1;
		HitResult hit;
		for (Enemy*& enemy : ENEMIES.GetAll())
		{
			for (std::pair<int, Collider*> enemyCollider : enemy->GetColliders())
			{
				HitResult temp;
				if (collider->Collision(enemyCollider.second, {}, temp)) // 衝突
				{
					if (temp.distance < hit.distance) // 一番接近計算
					{
						hit = temp;
						hitTarget = enemy;
						hitCollider = enemyCollider.first;
					}
					if (pierce) // 貫通処理：当たった敵全部
					{
						if (atk > 0 && owner == PlayerCharacterManager::Instance().GetPlayerCharacterById()) // クライアントの攻撃しか処理しない
						{
							OnHitEnemy(hit);
							SendCollision(hitTarget, enemyCollider.first);
						}
					}
					continue; // 次の敵
				}
			}
		}
		if (hitTarget != nullptr && !pierce) // 非貫通処理：衝突の一番近い目標
		{
			if (atk > 0 && owner == PlayerCharacterManager::Instance().GetPlayerCharacterById()) // クライアントの攻撃しか処理しない
			{
				OnHitEnemy(hit);
				SendCollision(hitTarget, hitCollider);
			}
			Destory();
		}
	}
}

void Projectile::SendCollision(Enemy* target, int colider_id)
{
	ENEMYCOLLISIONS.Register(target, colider_id, atk, power, direction * force);
	target->SetHurtCoolTime(coolTime);
}
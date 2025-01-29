#include "BeamObject.h"

#include "TAKOEngine/Physics/CollisionDataManager.h"
#include "GameObject/Character/Enemy/EnemyManager.h"
#include "GameObject/Character/Player/PlayerCharacterManager.h"

/**************************************************************************//**
	@brief    コンテクスト
	@param[in]    owner	プレイヤー
*//***************************************************************************/
BeamObject::BeamObject(PlayerCharacter* owner) : Projectile("Data/Model/Object/EffectBeam.glb", 1.0f, owner)
{
	collisionTarget |= COLLISION_TARGET::ENEMY | COLLISION_TARGET::STAGE;

	atk = 7;
	speed = 40.0f;
	gravity = 0.0f;
	front = { 0.0f, -1.0f, 0.0f };
	pierce = false;

	force = 10.0f;
	coolTime = 0.1f;

	DirectX::XMFLOAT3 startPosition = { owner->GetPosition().x, owner->GetPosition().y + 1.0f, owner->GetPosition().z };
	DirectX::XMVECTOR vStartPosition = DirectX::XMLoadFloat3(&startPosition);
	DirectX::XMFLOAT3 front = owner->GetFront();
	DirectX::XMVECTOR direction = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front));
	direction = DirectX::XMVectorScale(direction, m_distance);
	DirectX::XMVECTOR newPosition = XMVectorAdd(vStartPosition, direction);
	DirectX::XMStoreFloat3(&startPosition, newPosition);

	m_beam = std::make_unique<ModelObject>("Data/Model/Object/EffectBeam.glb", 1.0f, ModelObject::DX12, ModelObject::LHS_TOON);
	m_beam->SetShader("EffectBeam", ModelShaderDX12Id::Beam);

	m_beam->SetPosition(startPosition);

	m_beam->SetAngle(owner->GetAngle());
	m_beam->RotateAxis({ 0.0f, 1.0f, 0.0f }, XMConvertToRadians(180.0f));
}

/**************************************************************************//**
	@brief		変更
	@param[in]    elapsedTime
*//***************************************************************************/
void BeamObject::Update(float elapsedTime)
{
	tempPosition = position; // 1フレーム前の位置更新

	Projectile::Update(elapsedTime); // 位置更新

	m_beam->Update(elapsedTime);

	timer -= elapsedTime;
	if (timer < 0.0f)
	{
		Destory();
	}
}

///**************************************************************************//**
//	@brief	当たり判定処理
//*//***************************************************************************/
//void BeamObject::Collision()
//{
//	//Collider* col = collider.get();
//	//for (ModelObject*& map : MAPTILES.GetAll())
//	//{
//	//	HitResult hit;
//	//	DirectX::XMFLOAT3 direction = {};
//	//	if (map->GetCollider()->RayCast(tempPosition, position, hit))
//	//	{
//	//		OnHitGround(hit);
//	//		break;
//	//	}
//	//}
//
//	Enemy* hitTarget = nullptr;
//	int hitCollider = -1;
//	HitResult hit;
//	for (Enemy*& enemy : ENEMIES.GetAll())
//	{
//		for (std::pair<int, Collider*> enemyCollider : enemy->GetColliders())
//		{
//			HitResult temp;
//			if (enemyCollider.second->RayCast(tempPosition, position, temp)) // レイ衝突
//			{
//				if (temp.distance < hit.distance) // 一番接近計算
//				{
//					hit = temp;
//					hitTarget = enemy;
//					hitCollider = enemyCollider.first;
//				}
//				if (pierce) // 貫通処理：当たった敵全部
//				{
//					if (owner == PlayerCharacterManager::Instance().GetPlayerCharacterById(GAME_DATA.GetClientId())) // クライアントの攻撃しか処理しない
//					{
//						OnHitEnemy(hit);
//						SendCollision(hitTarget, enemyCollider.first);
//					}
//				}
//				continue; // 次の敵
//			}
//		}
//	}
//	if (hitTarget != nullptr && !pierce) // 非貫通処理：衝突の一番近い目標
//	{
//		if (owner == PlayerCharacterManager::Instance().GetPlayerCharacterById(GAME_DATA.GetClientId())) // クライアントの攻撃しか処理しない
//		{
//			OnHitEnemy(hit);
//			SendCollision(hitTarget, hitCollider);
//		}
//		Destory();
//	}
//}

/**************************************************************************//**
	@brief		ダメージ計算
	@param[in]    hit	相手の当たり判定
*//***************************************************************************/
void BeamObject::OnHitEnemy(HitResult& hit)
{
}

/**************************************************************************//**
	@brief		レンダリング
	@param[in]    rc	レンダリングコンテクスト
*//***************************************************************************/
void BeamObject::RenderDX12(const RenderContextDX12& rc)
{
	m_beam->RenderDX12(rc);
}
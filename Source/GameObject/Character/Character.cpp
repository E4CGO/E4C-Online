#include "Character.h"
#include <iostream>
#include "Map/MapTileManager.h"

#define POS_ERROR 0.0001f  // 誤差調整

/**************************************************************************//**
	@brief		移動処理
	@param[in]	X移動量
	@param[in]	Y移動量
	@param[in]	スピード
	@return なし
*//***************************************************************************/
void Character::Move(float vx, float vz, float speed) {
	// 移動
	moveVecX = vx;
	moveVecZ = vz;

	maxMoveSpeed = speed;
}

/**************************************************************************//**
	@brief		回転処理
	@param[in]	elapsedTime	経過時間
	@param[in]	vx			X移動量
	@param[in]	vz			Z移動量
	@param[in]	speed		回転スピード
	@return		なし
*//***************************************************************************/
void Character::Turn(float elapsedTime, float vx, float vz, float speed) {
	speed *= elapsedTime;

	float vecLength = vx * vx + vz * vz;
	// 進行ベクトルがゼロベクトルの場合は処理する必要なし
	if (vecLength < POS_ERROR) return;

	vecLength = sqrtf(vecLength);

	// 進行ベクトルを単位ベクトル化
	vx /= vecLength;
	vz /= vecLength;

	// 自身の回転値から前方向を求める
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	// 左右判定を行うために２つの単位ベクトルの外積を計算する
	float cross = (frontZ * vx) - (frontX * vz);

	float dot = (frontX * vx) + (frontZ * vz);
	float rot = 1.0f - dot;
	if (rot > speed) {
		rot = speed;
	}

	// 2Dの外積値が正の場合か負の場合によって左右判定が行える
	// 左右判定を行うことによって左右回転を選択する

	if (cross > 0.0f)
	{
		angle.y += rot;
	}
	else
	{
		angle.y -= rot;
	}

	while (angle.y > DirectX::XM_PI) angle.y -= DirectX::XM_2PI;
	while (angle.y < -DirectX::XM_PI) angle.y += DirectX::XM_2PI;
}
/**************************************************************************//**
	@brief		移動処理
	@param[in]	elapsedtime	経過時間
	@param[in]	target		ターゲット方向
	@param[in]	moveSpeed	移動速度
	@param[in]	turnSpeed	回転埴土
	@return		目的地判定
*//***************************************************************************/
bool Character::MoveTo(float elapsedtime, const DirectX::XMFLOAT3& target, float moveSpeed, float turnSpeed)
{
	// Horizon
	float vx = target.x - position.x;
	float vz = target.z - position.z;

	if (vx * vx + vz * vz < moveSpeed * elapsedtime * moveSpeed * elapsedtime) return true;

	Move(vx, vz, moveSpeed);
	Turn(elapsedtime, vx, vz, turnSpeed);

	return false;
}

/**************************************************************************//**
	@brief	跳躍勝利
	@param[in]	speed	上スピード
*//***************************************************************************/
void Character::Jump(float speed)
{
	velocity.y = speed;
	isGround = false;
}
/**************************************************************************//**
	@brief		速力
	@param[in]	elapsedTime	経過時間
	@return		なし
*//***************************************************************************/
void Character::UpdateVelocity(float elapsedTime)
{
	// 経過フレーム
	float elapsedFrame = 60.0f * elapsedTime;

	if (!isKinematic)
	{
		// 垂直速力更新処理
		UpdateVerticalVelocity(elapsedFrame);
		// 水平速力更新処理
		UpdateHorizontalVelocity(elapsedFrame);

		// 水平移動更新処理
		UpdateHorizontalMove(elapsedTime);
		// 垂直移動更新処理
		UpdateVerticalMove(elapsedTime);
	}

	// 衝突判定更新
	UpdateColliders();
}
/**************************************************************************//**
	@brief		垂直速力更新処理
	@param[in]	elapsedFrame	経過時間
	@return		なし
*//***************************************************************************/
void Character::UpdateVerticalVelocity(float elapsedFrame)
{
	velocity.y += gravity * elapsedFrame;
}
/**************************************************************************//**
	@brief		水平速力更新処理
	@param[in]	elapsedFrame	経過時間
	@return		なし
*//***************************************************************************/
void Character::UpdateHorizontalVelocity(float elapsedFrame)
{
	float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (length > 0.0f)
	{
		// 摩擦力
		float friction = this->friction * elapsedFrame;
		if (!isGround) friction *= airControl;

		if (length > friction)
		{
			velocity.x -= friction * velocity.x / length;
			velocity.z -= friction * velocity.z / length;
		}
		else
		{
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}
	}
	if (length <= maxMoveSpeed)
	{
		float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
		if (moveVecLength > 0.0f)
		{
			// 加速力
			float acceleration = this->acceleration * elapsedFrame;

			// 移動ベクトルによる加速処理
			velocity.x += acceleration * moveVecX / moveVecLength;
			velocity.z += acceleration * moveVecZ / moveVecLength;

			float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
			if (length > maxMoveSpeed)
			{
				velocity.x = maxMoveSpeed * velocity.x / length;
				velocity.z = maxMoveSpeed * velocity.z / length;
			}
		}
	}
	moveVecX = 0.0f;
	moveVecZ = 0.0f;
}

/**************************************************************************//**
	@brief		垂直速力更新処理
	@param[in]	elapsedFrame	経過時間
	@return		なし
*//***************************************************************************/
void Character::UpdateVerticalMove(float elapsedTime)
{
	// 垂直方向の移動量
	float my = velocity.y * elapsedTime;

	// キャラクターのY軸方向となる法線ベクトル
	DirectX::XMFLOAT3 normal = { 0.0f, 1.0f, 0.0f };

	// 落下中
	if (my < 0.0f)
	{
		// レイの開始位置は足元より少し上
		DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
		// レイの終点位置は移動後の位置
		DirectX::XMFLOAT3 end = { position.x, position.y + my, position.z };
		// レイキャストによる地面判定
		HitResult hit;
		if (MAPTILES.RayCast(start, end, hit))
		{
			// 地面に接地している
			position = hit.position;
			// 回転
			//angle.x += hit.rotation.x;
			//angle.y += hit.rotation.y;
			//angle.z += hit.rotation.z;

			normal = hit.normal;

			// 着地した
			if (!isGround)
			{
				OnLanding();
			}
			isGround = true;
			//velocity.y = -1.0f;
			velocity.y = 0.0f;
		}
		else
		{
			// 空中に浮いている
			position.y += my;
			if (velocity.y < -10.0f)
			{
				isGround = false;
			}
		}
	}
	// 上昇中
	else if (my > 0.0f)
	{// レイの開始位置は頭頂部より少し下
		DirectX::XMFLOAT3 start = { position.x, position.y + height - stepOffset, position.z };
		// レイの終点位置は移動後の位置
		DirectX::XMFLOAT3 end = { position.x, position.y + height + my, position.z };
		// レイキャストによる地面判定
		HitResult hit;
		if (MAPTILES.RayCast(start, end, hit))
		{
			// 地面に接地している
			//position = hit.position;
			// 回転
			//angle.x += hit.rotation.x;
			//angle.y += hit.rotation.y;
			//angle.z += hit.rotation.z;

			normal = hit.normal;

			velocity.y = 0.0f;
		}
		else {
			position.y += my;
		}
	}

	// 地面の向きに沿うようにXZ軸回転
	{
		// Y軸が法線ベクトル方向に向くオイラー角回転を算出する
		//angle.x = Mathf::Lerp(angle.x, atan2f(normal.z, normal.y), 0.2f);
		//angle.z = Mathf::Lerp(angle.z, -atan2f(normal.x, normal.y), 0.2f);
	}
}

/**************************************************************************//**
	@brief		水平速力更新処理
	@param[in]	elapsedFrame	経過時間
	@return		なし
*//***************************************************************************/
void Character::UpdateHorizontalMove(float elapsedTime)
{
	// 水平速力量計算
	float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (velocityLengthXZ > 0.0f)
	{
		// 水平移動地
		float mx = velocity.x * elapsedTime;
		float mz = velocity.z * elapsedTime;

		// マップ
		if (collider != nullptr) {
			//collider->SetPosition(position + DirectX::XMFLOAT3{ 0, height * 0.5f, 0 } *scale + DirectX::XMFLOAT3{ velocity.x / velocityLengthXZ, 0, velocity.z / velocityLengthXZ } *scale);
			//collider->SetPosition(position + DirectX::XMFLOAT3{ 0, 0.4f, 0 } + DirectX::XMFLOAT3{ velocity.x / velocityLengthXZ, 0, velocity.z / velocityLengthXZ });
			//if (!collider->CollisionVsMap(true))
			//{
			//	isWall = false;
				position.x += mx;
				position.z += mz;
			//}
			//else
			//{
			//	//collider->SetPosition(position + DirectX::XMFLOAT3{ 0, height * 0.5f, 0 } *scale + DirectX::XMFLOAT3{ mx, 0, mz });
			//	collider->SetPosition(position + DirectX::XMFLOAT3{ 0, 0.4f, 0 } + DirectX::XMFLOAT3{ mx, 0, mz });
			//	if (!collider->CollisionVsMap(true))
			//	{
			//		position.x += mx;
			//		position.z += mz;
			//	}
			//	else
			//	{
			//		if (!isWall)
			//		{
			//			OnWall();
			//		}
			//		isWall = true;
			//	}
			//}
		}
		else
		{
			// レイキャスト
			DirectX::XMFLOAT3 start;
			start = position;
			start.y += stepOffset;

			DirectX::XMFLOAT3 end = DirectX::XMFLOAT3(start.x + mx, start.y, start.z + mz);
			start.x -= (mx * 0.01f);
			start.z -= (mz * 0.01f);

			HitResult hit;
			if (MAPTILES.RayCast(start, end, hit))
			{
				// 壁からのベクトル
				DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&hit.position);
				DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
				DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

				// 壁の法線
				DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

				// 入射ベクトルを法線に射影
				DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);
				Dot = DirectX::XMVectorScale(Dot, 1.1f); // 壁めり込み対策

				// 補正位置の計算
				DirectX::XMVECTOR CorrectPosition = DirectX::XMVectorMultiplyAdd(Normal, Dot, End);
				DirectX::XMFLOAT3 correctPosition;
				DirectX::XMStoreFloat3(&correctPosition, CorrectPosition);

				// 壁擦り方向へレイキャスト
				HitResult hit2;
				if (!MAPTILES.RayCast(start, correctPosition, hit2))
				{
					// 壁擦り方向で壁に当たったら補正位置に移動
					position.x = correctPosition.x;
					position.z = correctPosition.z;
				}
				else
				{
					position.x = hit2.position.x;
					position.z = hit2.position.z;
				}
				if (!isWall)
				{
					OnWall();
				}
				isWall = true;
			}
			else
			{
				isWall = false;
				position.x += mx;
				position.z += mz;
			}
		}
	}
}

void Character::ModifyHp(int hp)
{
	this->hp += hp;
	if (this->hp > maxHp) this->hp = maxHp;
	if (this->hp < 0) this->hp = 0;
}

/**************************************************************************//**
	@brief		更新処理
	@param[in]	elapsedTime	経過時間
	@return		なし
*//***************************************************************************/
void Character::Update(float elapsedTime)
{
	if (hurtCoolTime > 0.0f) hurtCoolTime -= elapsedTime;

	UpdateVelocity(elapsedTime);			// 移動更新
	ModelObject::Update(elapsedTime);
}
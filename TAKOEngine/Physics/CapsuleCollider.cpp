﻿//! @file CapsuleCollider.cpp
//! @note カプセルコリダー

#include "CapsuleCollider.h"

//#include <DirectXCollision.h>
#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "Source/Map/MapTileManager.h"

/**************************************************************************//**
		@brief		コンストラクタ
		@param[in]	なし
		@return		なし
*//***************************************************************************/
CapsuleCollider::CapsuleCollider()
{
	type = COLLIDER_TYPE::CAPSULE;
}

/**************************************************************************//**
		@brief		Capsule用パラメータセット
		@param[in]	Capsule capsule
		@return		なし
*//***************************************************************************/
void CapsuleCollider::SetParam(Capsule capsule)
{
	position = capsule.position;
	direction = capsule.direction;
	radius = capsule.radius;
	length = capsule.length;
}

/**************************************************************************//**
		@brief		デバッグ描画
		@param[in]	color
		@return		なし
*//***************************************************************************/
void CapsuleCollider::DrawDebugPrimitive(DirectX::XMFLOAT4 color)
{
	if (!enable) return;
	T_GRAPHICS.GetDebugRenderer()->DrawCapsule(position, radius, length, color);
}

/**************************************************************************//**
		@brief		マップとの当たり判定
		@param[in]	なし
		@return		衝突判定
*//***************************************************************************/
bool CapsuleCollider::CollisionVsMap()
{
	Capsule capsule;
	capsule.position = position;
	capsule.direction = direction;
	capsule.radius = radius;
	capsule.length = length;

	if (MAPTILES.IntersectCapsuleVsMap(capsule))
	{
		position = capsule.position;
		return true;
	}
	return false;
};
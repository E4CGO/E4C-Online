//! @file GameObject.cpp
//! @note 

#include "GameObject.h"

/**************************************************************************//**
		@brief		トランスフォーム更新
		@param[in]	なし
		@return		なし
*//***************************************************************************/
void GameObject::UpdateTransform() {
	// スケール行列生成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	// 回転行列生成
	DirectX::XMMATRIX R = AnglesToMatrix(angle);
	// 位置行列生成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	DirectX::XMMATRIX W = S * R * T;

	DirectX::XMStoreFloat4x4(&transform, W);
}

/**************************************************************************//**
 	@brief		ローカルポジティブからワールドへ
	@param[in]	v ビュー位置
	@return	ワールドポジション
*//***************************************************************************/
DirectX::XMFLOAT3 GameObject::LocalPositiontoWorld(DirectX::XMFLOAT3 v)
{
	DirectX::XMFLOAT3 right = DirectX::XMFLOAT3(transform._11, transform._12, transform._13) * v.x;
	DirectX::XMFLOAT3 up = DirectX::XMFLOAT3(transform._21, transform._22, transform._23) * v.y;
	DirectX::XMFLOAT3 front = DirectX::XMFLOAT3(transform._31, transform._32, transform._33) * v.z;
	return right + up + front;
}

/**************************************************************************//**
 	@brief		オブジェクトのスクリーン座標を取得（既知ビューポート、ビュープロジェクション、ワールド）
	@param[in]	viewport	ビューポート参照
	@param[in]	View		ビュー行列参照
	@param[in]	Projection	プロジェクション行列参照
	@param[in]	World		ワールド行列参照
	@return		スクリーン座標
*//***************************************************************************/
DirectX::XMFLOAT3 GameObject::GetScreenPosition(
	const D3D11_VIEWPORT& viewport,
	const  DirectX::XMMATRIX& View,
	const  DirectX::XMMATRIX& Projection,
	const DirectX::XMMATRIX World
)
{
	return T_GRAPHICS.GetScreenPosition(this->position, viewport, View, Projection, World);
}

/**************************************************************************//**
 	@brief		オブジェクトのスクリーン座標を取得（ビューポート、ビュープロジェクション、ワールド再計算）
	@param[in]	なし
	@return		スクリーン座標
*//***************************************************************************/
DirectX::XMFLOAT3 GameObject::GetScreenPosition()
{
	return T_GRAPHICS.GetScreenPosition(this->position);
}

/**************************************************************************//**
 	@brief		回転軸と角度を利用して回転する
	@param[in]	Axis	回転軸
	@param[in]	Angle	角度
*//***************************************************************************/
void GameObject::RotateAxis(const DirectX::XMFLOAT3 Axis, float Angle)
{
	// 回転行列生成
	DirectX::XMVECTOR Q = DirectX::XMQuaternionRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMVECTOR R = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&Axis), Angle);
	Q = DirectX::XMQuaternionMultiply(R, Q);

	DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(Q);
	angle = MatrixToAngles(m);
}
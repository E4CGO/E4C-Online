//! @file GameObject.cpp
//! @note 

#include "GameObject.h"

/**************************************************************************//**
		@brief		�g�����X�t�H�[���X�V
		@param[in]	�Ȃ�
		@return		�Ȃ�
*//***************************************************************************/
void GameObject::UpdateTransform() {
	// �X�P�[���s�񐶐�
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	// ��]�s�񐶐�
	DirectX::XMMATRIX R = AnglesToMatrix(angle);
	// �ʒu�s�񐶐�
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	DirectX::XMMATRIX W = S * R * T;

	DirectX::XMStoreFloat4x4(&transform, W);
}

/**************************************************************************//**
 	@brief		���[�J���|�W�e�B�u���烏�[���h��
	@param[in]	v �r���[�ʒu
	@return	���[���h�|�W�V����
*//***************************************************************************/
DirectX::XMFLOAT3 GameObject::LocalPositiontoWorld(DirectX::XMFLOAT3 v)
{
	DirectX::XMFLOAT3 right = DirectX::XMFLOAT3(transform._11, transform._12, transform._13) * v.x;
	DirectX::XMFLOAT3 up = DirectX::XMFLOAT3(transform._21, transform._22, transform._23) * v.y;
	DirectX::XMFLOAT3 front = DirectX::XMFLOAT3(transform._31, transform._32, transform._33) * v.z;
	return right + up + front;
}

/**************************************************************************//**
 	@brief		�I�u�W�F�N�g�̃X�N���[�����W���擾�i���m�r���[�|�[�g�A�r���[�v���W�F�N�V�����A���[���h�j
	@param[in]	viewport	�r���[�|�[�g�Q��
	@param[in]	View		�r���[�s��Q��
	@param[in]	Projection	�v���W�F�N�V�����s��Q��
	@param[in]	World		���[���h�s��Q��
	@return		�X�N���[�����W
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
 	@brief		�I�u�W�F�N�g�̃X�N���[�����W���擾�i�r���[�|�[�g�A�r���[�v���W�F�N�V�����A���[���h�Čv�Z�j
	@param[in]	�Ȃ�
	@return		�X�N���[�����W
*//***************************************************************************/
DirectX::XMFLOAT3 GameObject::GetScreenPosition()
{
	return T_GRAPHICS.GetScreenPosition(this->position);
}

/**************************************************************************//**
 	@brief		��]���Ɗp�x�𗘗p���ĉ�]����
	@param[in]	Axis	��]��
	@param[in]	Angle	�p�x
*//***************************************************************************/
void GameObject::RotateAxis(const DirectX::XMFLOAT3 Axis, float Angle)
{
	// ��]�s�񐶐�
	DirectX::XMVECTOR Q = DirectX::XMQuaternionRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMVECTOR R = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&Axis), Angle);
	Q = DirectX::XMQuaternionMultiply(R, Q);

	DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(Q);
	angle = MatrixToAngles(m);
}
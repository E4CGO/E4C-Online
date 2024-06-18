#include "GameObject.h"

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

DirectX::XMFLOAT3 GameObject::LocalPositiontoWorld(DirectX::XMFLOAT3 v)
{
	DirectX::XMFLOAT3 right = DirectX::XMFLOAT3(transform._11, transform._12, transform._13) * v.x;
	DirectX::XMFLOAT3 up = DirectX::XMFLOAT3(transform._21, transform._22, transform._23) * v.y;
	DirectX::XMFLOAT3 front = DirectX::XMFLOAT3(transform._31, transform._32, transform._33) * v.z;
	return right + up + front;
}

DirectX::XMFLOAT3 GameObject::GetScreenPosition(
	const D3D11_VIEWPORT& viewport,
	const  DirectX::XMMATRIX& View,
	const  DirectX::XMMATRIX& Projection,
	const DirectX::XMMATRIX World
)
{
	return T_GRAPHICS.GetScreenPosition(this->position, viewport, View, Projection, World);
}
DirectX::XMFLOAT3 GameObject::GetScreenPosition()
{
	return T_GRAPHICS.GetScreenPosition(this->position);
}

void GameObject::RotateAxis(const DirectX::XMFLOAT3 Axis, float Angle)
{
	// 回転行列生成
	//DirectX::XMMATRIX R = AnglesToMatrix(angle);
	//DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	//DirectX::XMMATRIX Rotate = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&Axis), Angle);
	//R = Rotate * R;

	//angle = MatrixToAngles(R);

	DirectX::XMVECTOR Q = DirectX::XMQuaternionRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMVECTOR R = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&Axis), Angle);
	Q = DirectX::XMQuaternionMultiply(R, Q);

	DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(Q);
	angle = MatrixToAngles(m);

	//angle.x = atanf(2.0f * (q.x * q.y + q.z * q.w) / (1 - 2.0f * (q.x * q.x + q.y * q.y)));
	//angle.y = asinf(2.0f * (q.x * q.z - q.w * q.x));
	//angle.z = atanf(2.0f * (q.x * q.w + q.y * q.z) / (1 - 2.0f * (q.z * q.z + q.w * q.w)));
}
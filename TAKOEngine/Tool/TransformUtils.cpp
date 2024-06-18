#include "TransformUtils.h"

// 行列からヨー、ピッチ、ロールを行列を計算する
bool TransformUtils::MatrixToRollPitchYaw(const DirectX::XMFLOAT4X4& m, float& pitch, float& yaw, float& roll)
{
	float xRadian = asinf(-m._32);
	pitch = xRadian;
	if (xRadian < DirectX::XM_PI / 2.0f)
	{
		if (xRadian > -DirectX::XM_PI / 2.0f)
		{
			roll = atan2f(m._12, m._22);
			yaw = atan2f(m._31, m._33);
			return true;
		}
		else
		{
			roll = (float)-atan2f(m._13, m._11);
			yaw = 0.0f;
			return false;
		}
	}
	else
	{
		roll = (float)atan2f(m._13, m._11);
		yaw = 0.0f;
		return false;
	}

	return false;
}

// クォータニオンからヨー、ピッチ、ロールを行列を計算する
bool TransformUtils::QuaternionToRollPitchYaw(const DirectX::XMFLOAT4& q, float& pitch, float& yaw, float& roll)
{
	DirectX::XMVECTOR Q = DirectX::XMLoadFloat4(&q);
	DirectX::XMMATRIX M = DirectX::XMMatrixRotationQuaternion(Q);
	DirectX::XMFLOAT4X4 m;
	DirectX::XMStoreFloat4x4(&m, M);
	return MatrixToRollPitchYaw(m, pitch, yaw, roll);
}
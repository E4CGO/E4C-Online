#pragma once

using namespace DirectX;

inline XMFLOAT3 operator+(XMFLOAT3 v1, XMFLOAT3 v2) { return XMFLOAT3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); }
inline XMFLOAT3 operator-(XMFLOAT3 v1, XMFLOAT3 v2) { return XMFLOAT3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z); }
inline XMFLOAT3 operator*(XMFLOAT3 v1, XMFLOAT3 v2) { return XMFLOAT3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z); }
inline XMFLOAT3 operator/(XMFLOAT3 v1, XMFLOAT3 v2) { return XMFLOAT3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z); }
inline void operator+=(XMFLOAT3& v1, const XMFLOAT3& v2) { v1.x += v2.x; v1.y += v2.y; v1.z += v2.z; }
inline void operator-=(XMFLOAT3& v1, const XMFLOAT3& v2) { v1.x -= v2.x; v1.y -= v2.y; v1.z -= v2.z; }
inline void operator*=(XMFLOAT3& v1, const XMFLOAT3& v2) { v1.x *= v2.x; v1.y *= v2.y; v1.z *= v2.z; }
inline void operator/=(XMFLOAT3& v1, const XMFLOAT3& v2) { v1.x /= v2.x; v1.y /= v2.y; v1.z /= v2.z; }

inline XMFLOAT3 operator+(XMFLOAT3 v, float num) { return XMFLOAT3(v.x + num, v.y + num, v.z + num); }
inline XMFLOAT3 operator-(XMFLOAT3 v, float num) { return XMFLOAT3(v.x - num, v.y - num, v.z - num); }
inline XMFLOAT3 operator*(XMFLOAT3 v, float num) { return XMFLOAT3(v.x * num, v.y * num, v.z * num); }
inline XMFLOAT3 operator/(XMFLOAT3 v, float num) { return XMFLOAT3(v.x / num, v.y / num, v.z / num); }

inline void operator+=(XMFLOAT3& v, const float& num) { v.x += num; v.y += num; v.z += num; }
inline void operator-=(XMFLOAT3& v, const float& num) { v.x -= num; v.y -= num; v.z -= num; }
inline void operator*=(XMFLOAT3& v, const float& num) { v.x *= num; v.y *= num; v.z *= num; }
inline void operator/=(XMFLOAT3& v, const float& num) { v.x /= num; v.y /= num; v.z /= num; }
inline bool operator==(const XMFLOAT3& v1, const XMFLOAT3& v2) { return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z; }
inline bool operator!=(const XMFLOAT3& v1, const XMFLOAT3& v2) { return !(v1 == v2); }

inline float XMFLOAT3Length(const XMFLOAT3 v) { return XMVectorGetX(XMVector3Length(XMLoadFloat3(&v))); }
inline float XMFLOAT3LengthSq(const XMFLOAT3 v) { return XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&v))); }
inline XMFLOAT3 XMFLOAT3Normalize(const XMFLOAT3 v)
{
	XMFLOAT3 n;
	XMStoreFloat3(&n, XMVector3Normalize(XMLoadFloat3(&v)));
	return n;
}
inline XMFLOAT3 XMFLOAT3Corss(const XMFLOAT3& v1, const XMFLOAT3& v2)
{
	XMFLOAT3 n;
	XMStoreFloat3(&n, XMVector3Cross(XMLoadFloat3(&v1), XMLoadFloat3(&v2)));
	return n;
}
inline float XMFLOAT3Dot(const XMFLOAT3& v1, const XMFLOAT3& v2) { return  XMVectorGetX(XMVector3Dot(XMLoadFloat3(&v1), XMLoadFloat3(&v2))); }
inline float AngleBetweenXMFLOAT3(const XMFLOAT3& v1, const XMFLOAT3& v2) { return XMVectorGetX(XMVector3AngleBetweenVectors(XMLoadFloat3(&v1), XMLoadFloat3(&v2))); }

inline XMMATRIX AnglesToMatrix(XMFLOAT3 angle) {
	DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
	DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
	DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
	return Y * X * Z;
}

inline XMFLOAT3 MatrixToAngles(XMFLOAT4X4& m)
{
	//XMFLOAT3 angle = {};
	//angle.x = asinf(-m._32);						// Pitch
	//if (cosf(angle.x) > 0.0001f)							// Not at poles
	//{
	//	angle.y = atan2f(m._31, m._33);	// Yaw
	//	angle.z = atan2f(m._12, m._22);	// Roll
	//}
	//else
	//{
	//	angle.y = 0.0f;										// Yaw
	//	angle.z = atan2f(-m._21, m._11);	// Roll
	//}
	//return angle;

	XMFLOAT3 angle = {};
	float xRadian = asinf(-m._32);
	angle.x = xRadian;

	if (xRadian < DirectX::XM_PI / 2.0f)
	{
		if (xRadian > -DirectX::XM_PI / 2.0f)
		{
			angle.y = atan2f(m._31, m._33);
			angle.z = atan2f(m._12, m._22);
		}
		else
		{
			angle.y = 0.0f;
			angle.z = (float)-atan2f(m._13, m._11);
		}
	}
	else
	{
		angle.y = 0.0f;
		angle.z = (float)atan2f(m._13, m._11);
	}

	return angle;
}

inline XMFLOAT3 MatrixToAngles(XMMATRIX& quaternion)
{
	DirectX::XMFLOAT4X4 mat;
	DirectX::XMStoreFloat4x4(&mat, quaternion);
	return MatrixToAngles(mat);
}

inline XMFLOAT2 operator+(XMFLOAT2 v1, XMFLOAT2 v2) { return XMFLOAT2(v1.x + v2.x, v1.y + v2.y); }
inline XMFLOAT2 operator-(XMFLOAT2 v1, XMFLOAT2 v2) { return XMFLOAT2(v1.x - v2.x, v1.y - v2.y); }
inline XMFLOAT2 operator*(XMFLOAT2 v1, XMFLOAT2 v2) { return XMFLOAT2(v1.x * v2.x, v1.y * v2.y); }
inline XMFLOAT2 operator/(XMFLOAT2 v1, XMFLOAT2 v2) { return XMFLOAT2(v1.x / v2.x, v1.y / v2.y); }
inline void operator+=(XMFLOAT2& v1, const XMFLOAT2& v2) { v1.x += v2.x; v1.y += v2.y; }
inline void operator-=(XMFLOAT2& v1, const XMFLOAT2& v2) { v1.x -= v2.x; v1.y -= v2.y; }
inline void operator*=(XMFLOAT2& v1, const XMFLOAT2& v2) { v1.x *= v2.x; v1.y *= v2.y; }
inline void operator/=(XMFLOAT2& v1, const XMFLOAT2& v2) { v1.x /= v2.x; v1.y /= v2.y; }

inline XMFLOAT2 operator+(XMFLOAT2 v, float num) { return XMFLOAT2(v.x + num, v.y + num); }
inline XMFLOAT2 operator-(XMFLOAT2 v, float num) { return XMFLOAT2(v.x - num, v.y - num); }
inline XMFLOAT2 operator*(XMFLOAT2 v, float num) { return XMFLOAT2(v.x * num, v.y * num); }
inline XMFLOAT2 operator/(XMFLOAT2 v, float num) { return XMFLOAT2(v.x / num, v.y / num); }

inline void operator+=(XMFLOAT2& v, const float& num) { v.x += num; v.y += num; }
inline void operator-=(XMFLOAT2& v, const float& num) { v.x -= num; v.y -= num; }
inline void operator*=(XMFLOAT2& v, const float& num) { v.x *= num; v.y *= num; }
inline void operator/=(XMFLOAT2& v, const float& num) { v.x /= num; v.y /= num; }
inline bool operator==(const XMFLOAT2& v1, const XMFLOAT2& v2) { return v1.x == v2.x && v1.y == v2.y; }
inline bool operator!=(const XMFLOAT2& v1, const XMFLOAT2& v2) { return !(v1 == v2); }

inline float XMFLOAT2Length(const XMFLOAT2 v) { return XMVectorGetX(XMVector2Length(XMLoadFloat2(&v))); };
inline XMFLOAT2 XMFLOAT2Normalize(const XMFLOAT2 v)
{
	XMFLOAT2 n;
	XMStoreFloat2(&n, XMVector2Normalize(XMLoadFloat2(&v)));
	return n;
}

#pragma once

#include <DirectXMath.h>
#include <vector>
// カメラ

class Camera
{
public:
	Camera() {};
	~Camera() {};
public:
	// 指定方向を向く
	void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);
	
	// パースペクティブ設定
	void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);
	
	// ビュー行列取得
	const DirectX::XMFLOAT4X4& GetView() const { return view; }

	// プロジェクション行列取得
	const DirectX::XMFLOAT4X4& GetProjection() const { return projection; }
	
	// 視点取得
	const DirectX::XMFLOAT3& GetEye() const { return eye; }
	
	// 注視点取得
	const DirectX::XMFLOAT3& GetFocus() const { return focus; }
	
	// 上方向取得
	const DirectX::XMFLOAT3& GetUp() const { return up; }
	
	// 前方向取得
	const DirectX::XMFLOAT3& GetFront() const { return front; }
	
	// 右方向取得
	const DirectX::XMFLOAT3& GetRight() const { return right; }

	//視野角取得
	const float& GetFovY() { return fovY; }

	//アスペクト比取得
	const float& GetAspect() { return aspect; }

	//遠平面までの距離を取得
	const float& GetFarZ() { return farZ; }

	//近平面までの距離を取得
	const float& GetNearZ() { return nearZ; }

	//インデックス取得
	int& GetSegment() { return currentSegment; }

	//動くカメラ
	void MoveTo(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, float transitiontime, float transitionDuration);
	//回転するカメラ
	void RotateTo(const DirectX::XMFLOAT3& target, float& angle, float radius, float speed, float elapsedTime);
	//始点、中点、終点のポイントを経由して移動するカメラ
	void MoveByPoints(const std::vector<DirectX::XMFLOAT3>& positions, const std::vector<DirectX::XMFLOAT3>& focusPoints,float transitionTime, float transitionDuration);
	//始点、終点のポイントを経由して移動するカメラ
	void Move2PointToCamera(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT3& startFocus, const DirectX::XMFLOAT3& endFocus, float transitionTime, float transitionDuration);
private:
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;

	DirectX::XMFLOAT3 eye;
	DirectX::XMFLOAT3 focus;

	DirectX::XMFLOAT3 up;
	DirectX::XMFLOAT3 front;
	DirectX::XMFLOAT3 right;

	float fovY;
	float aspect;
	float nearZ;
	float farZ;

	std::vector<DirectX::XMFLOAT3> cameraPositions;  // カメラの各ポイントの位置
	std::vector<DirectX::XMFLOAT3> cameraFocusPoints;  // カメラの各ポイントの注視点
	
	int currentSegment = 0;
};

#pragma once

#include "TAKOEngine/Editor/Camera/Camera.h"

class CameraController
{
public:
	CameraController() = default;
	// カメラからコントローラーヘパラメータを同期する
	void SyncCameraToController(Camera* camera);
	// コントローラーからカメラへパラメータを同期する
	void SyncContrllerToCamera(Camera* camera);
	// 更新処理
	virtual void Update(float elapsedTime = 0.0f) {};

	float GetDistance() { return distance; }
	virtual void SetDistance(float d) { distance = d; };


	bool isEnable() { return enable; }
	void SetEnable(bool enable) { this->enable = enable; }
protected:
	bool enable = true;

	DirectX::XMFLOAT3 eye;
	DirectX::XMFLOAT3 focus;
	DirectX::XMFLOAT3 up;
	DirectX::XMFLOAT3 right;

	float angleX;
	float angleY;
	float distance;
};

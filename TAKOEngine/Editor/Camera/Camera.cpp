#include "Camera.h"
#include "TAKOEngine/Tool/Mathf.h"
#include <stdlib.h>
#include "TAKOEngine/Tool/PerlinNoise.h"
#include "GameObject/Character/Character.h"
#include "ThridPersonCameraController.h"
// 指定方向を向く
void Camera::SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{

	// 視点、注視点、上方向からビュー行列を作成
	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
	DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	DirectX::XMStoreFloat4x4(&view, View);

	// ビューを逆行列化し、ワールド行列に戻す
	DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, World);

	// カメラの方向を取り出す
	this->right.x = world._11;
	this->right.y = world._12;
	this->right.z = world._13;

	this->up.x = world._21;
	this->up.y = world._22;
	this->up.z = world._23;

	this->front.x = world._31;
	this->front.y = world._32;
	this->front.z = world._33;

	// 視点、注視点を保存
	this->eye = eye;
	this->focus = focus;
}

void Camera::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
	// 画角、画面比率、クリップ距離からプロジェクション行列を作成
	DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
	DirectX::XMStoreFloat4x4(&projection, Projection);

	// パラメーターだけ保存
	this->fovY   = fovY;
	this->aspect = aspect;
	this->nearZ  = nearZ;
	this->farZ   = farZ;
}
void Camera::CameraShake(float shakeAmplitude, float shakeTime,float elapsedTime)
{
	if (shakeTimer < shakeTime) {
		shakeAmplitude = 1.f * (shakeTime - shakeTimer);
	}
	else {
		shakeAmplitude = 0.f;
	}
	if (m_shake)
	{
		shakeTimer += elapsedTime;

		shakeOffset.x = (NoiseGenerator::PerlinNoise({ shakeTimer * 10.f, 0.0f, 0.f }, 1) - 0.5f) * shakeAmplitude;
		shakeOffset.y = (NoiseGenerator::PerlinNoise({ 0.0f, shakeTimer * 10.f, 0.0f }, 1) - 0.5f) * shakeAmplitude;
	}
	if (shakeTimer > shakeTime)
	{
		m_shake = false;
		shakeTimer = 0;
	}
}
void Camera::MoveTo(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, float transitiontime, float transitionDuration)
{
	// 遷移時間を増加
	if (transitiontime > transitionDuration)
	{
		transitiontime = transitionDuration;
	}

	// 補間係数を計算（0.0〜1.0の範囲）
	float t = transitiontime / transitionDuration;

	// カメラの位置と注視点を補間
	DirectX::XMFLOAT3 interpolatedEye{};
	interpolatedEye.x = Mathf::Lerp(this->GetEye().x, eye.x, t);
	interpolatedEye.y = Mathf::Lerp(this->GetEye().y, eye.y, t);
	interpolatedEye.z = Mathf::Lerp(this->GetEye().z, eye.z, t);

	DirectX::XMFLOAT3 interpolatedFocus;
	interpolatedFocus.x = Mathf::Lerp(this->GetFocus().x, focus.x, t);
	interpolatedFocus.y = Mathf::Lerp(this->GetFocus().y, focus.y, t);
	interpolatedFocus.z = Mathf::Lerp(this->GetFocus().z, focus.z, t);


	


	// 補間結果をカメラに設定
	this->SetLookAt(interpolatedEye, interpolatedFocus, { 0.0f, 1.0f, 0.0f });
}

void Camera::RotateTo(const DirectX::XMFLOAT3& target, float& angle, float radius, float speed, float elapsedTime)
{

	// 回転の進行
	angle += speed * elapsedTime;

	// カメラの新しい位置を計算（円運動）
	float x = target.x + radius * cos(angle);
	float y = target.y + 10;
	float z = target.z + radius * sin(angle);



	// カメラの位置と注視点を更新
	this->SetLookAt(
		{ x,y,z },    // カメラの新しい位置
		target,         // 注視点は変わらない
		DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f)  // 上方向
	);

	// 360度回転したら角度をリセット
	if (angle >= DirectX::XM_2PI)  // 2π = 360度
	{
		angle = 0.0f;
	}
}
void Camera::Move2PointToCamera(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT3& startFocus, const DirectX::XMFLOAT3& endFocus, float transitionTime, float transitionDuration)
{
	// 遷移時間の更新
	if (transitionTime > transitionDuration)
	{
		transitionTime = transitionDuration;
	}

	// 補間係数を計算（0.0〜1.0の範囲でクランプ）
	float t = transitionTime / transitionDuration;
	t = min(t, 1.0f);

	// カメラの位置と注視点を補間
	DirectX::XMFLOAT3 interpolatedEye{};
	interpolatedEye.x = Mathf::Lerp(start.x, end.x, t);
	interpolatedEye.y = Mathf::Lerp(start.y, end.y, t);
	interpolatedEye.z = Mathf::Lerp(start.z, end.z, t);

	DirectX::XMFLOAT3 interpolatedFocus{};
	interpolatedFocus.x = Mathf::Lerp(startFocus.x, endFocus.x, t);
	interpolatedFocus.y = Mathf::Lerp(startFocus.y, endFocus.y, t);
	interpolatedFocus.z = Mathf::Lerp(startFocus.z, endFocus.z, t);

	// 補間結果をカメラに設定
	this->SetLookAt(interpolatedEye, interpolatedFocus, { 0.0f, 1.0f, 0.0f });

	// 遷移が終了したら、最終位置を固定する（オプション）
	if (t >= 1.0f)
	{
		// 最終位置に到達した場合の処理
		transitionTime = 0.0f;  // 遷移をリセット
	}


}

void Camera::MoveByPoints(const std::vector<DirectX::XMFLOAT3>& positions, const std::vector<DirectX::XMFLOAT3>& focusPoints, float transitionTime, float transitionDuration)
{
	if (positions.size() < 2 || focusPoints.size() < 2 || positions.size() != focusPoints.size())
	{
		// 少なくとも2つのポイントが必要
		return;
	}

	// 遷移時間の更新
	if (transitionTime > transitionDuration)
	{
		transitionTime = transitionDuration;
	}
	// 現在のセグメントのスタートとエンドのポイントを取得
	int nextSegment = (currentSegment + 1) % positions.size();

	DirectX::XMFLOAT3 start = positions[currentSegment];
	DirectX::XMFLOAT3 end = positions[nextSegment];

	DirectX::XMFLOAT3 startFocus = focusPoints[currentSegment];
	DirectX::XMFLOAT3 endFocus = focusPoints[nextSegment];

	// 補間係数を計算（0.0〜1.0の範囲でクランプ）
	float t = transitionTime / transitionDuration;
	t = (t > 1.0f) ? 1.0f : t;

	// カメラの位置と注視点を補間
	DirectX::XMFLOAT3 interpolatedEye{};
	interpolatedEye.x = Mathf::Lerp(start.x, end.x, t);
	interpolatedEye.y = Mathf::Lerp(start.y, end.y, t);
	interpolatedEye.z = Mathf::Lerp(start.z, end.z, t);

	DirectX::XMFLOAT3 interpolatedFocus{};
	interpolatedFocus.x = Mathf::Lerp(startFocus.x, endFocus.x, t);
	interpolatedFocus.y = Mathf::Lerp(startFocus.y, endFocus.y, t);
	interpolatedFocus.z = Mathf::Lerp(startFocus.z, endFocus.z, t);

	// 補間結果をカメラに設定
	this->SetLookAt(interpolatedEye, interpolatedFocus, { 0.0f, 1.0f, 0.0f });

	// 遷移が終了したら、次のセグメントに進む
	if (t >= 1.0f)
	{
		transitionTime = 0.0f;  // 遷移をリセット
		currentSegment = nextSegment;  // 次のセグメントに移動
	}
}


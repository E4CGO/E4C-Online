#include "ThridPersonCameraController.h"
#include "TAKOEngine/Tool/Mathf.h"

#include <iostream>
#include <imgui.h>
#include <algorithm>

#include "UI/Widget/WidgetCrosshair.h"

#include "Map/MapTileManager.h"

#include "GameData.h"

#include "TAKOEngine/Tool/PerlinNoise.h"

static ThridPersonCameraController* instance = nullptr;

// インスタンス取得
ThridPersonCameraController& ThridPersonCameraController::Instance()
{
	return *instance;
}

ThridPersonCameraController::ThridPersonCameraController()
{
	// インスタンスポインタ設定
	instance = this;

	distance = (distanceMax + distanceMin) * 0.5f;
	distanceLerp = distance;
	offsetLerp = offset;
}

// 更新処理
void ThridPersonCameraController::Update(float elapsedTime)
{
	if (player == nullptr) return; // 更新しないフラグ
	if (!TentacleLib::isFocus()) return; // フォーカスしない

	shakeTimer -= elapsedTime;
	if (shakeTimer < 0.0f) shakeTimer = 0.0f;
	DirectX::XMFLOAT3 PlayerPostion = player->GetPosition() + DirectX::XMFLOAT3(0, player->GetHeight() * 1.1f, 0);

	float moveX = 0.0f;
	float moveY = 0.0f;
	float wheelScroll = 0.0f;

	if (!TentacleLib::isShowCursor())
	{
		//if (GAME_SETTING.KeyboardInput)
		{
			// マウス入力値を使ってカメラ操作する。
			wheelScroll = T_INPUT.GetMouseScrollY();

			// マウスカーソルの移動量を求める
			moveX = T_INPUT.GetMouseDeltaX() / SCREEN_W * GAME_SETTING.CameraSensitivity;
			moveY = T_INPUT.GetMouseDeltaY() / SCREEN_H * GAME_SETTING.CameraSensitivity;
		}
		//else
		{
			if (T_INPUT.GamePadKeyPress(GAME_PAD_BTN::DPAD_DOWN)) wheelScroll += -1.0f;
			if (T_INPUT.GamePadKeyPress(GAME_PAD_BTN::DPAD_UP)) wheelScroll += +1.0f;

			if (T_INPUT.GetGamePadRAxis().x != 0.0f) moveX = T_INPUT.GetGamePadRAxis().x * 0.01f * GAME_SETTING.CameraSensitivity;
			if (T_INPUT.GetGamePadRAxis().y != 0.0f) moveY = -T_INPUT.GetGamePadRAxis().y * 0.01f * GAME_SETTING.CameraSensitivity;
		}
	}

	if (!enable)
	{
		moveX = moveY = wheelScroll = 0.0f;
	}

	if (wheelScroll != 0.0f)
	{
		distanceLerp -= wheelScroll * scrollSpeed;
		if (distanceLerp > distanceMax) distanceLerp = distanceMax;
		if (distanceLerp < distanceMin) distanceLerp = distanceMin;
	}
	distance = std::lerp(distance, distanceLerp, lerpT);

	// 反転設定
	if (GAME_SETTING.CamerainvertX) moveX *= -1.0f;
	if (GAME_SETTING.CamerainvertY) moveY *= -1.0f;

	// Y軸回転
	angleYLerp += moveX;
	angleY = std::lerp(angleY, angleYLerp, lerpT);

	// X軸回転
	angleXLerp += moveY;

	angleXLerp = std::clamp(angleXLerp, -angleXMax, angleXMax);
	angleX = std::lerp(angleX, angleXLerp, lerpT);

	// OFFSET
	if (offset.x != offsetLerp.x) offset.x = std::lerp(offset.x, offsetLerp.x, lerpT);
	if (offset.y != offsetLerp.y) offset.y = std::lerp(offset.y, offsetLerp.y, lerpT);

	float sx = ::sinf(angleX);
	float cx = ::cosf(angleX);
	float sy = ::sinf(angleY);
	float cy = ::cosf(angleY);

	// カメラの方向を算出
	DirectX::XMVECTOR Front = DirectX::XMVectorSet(-cx * sy, -sx, -cx * cy, 0.0f);
	DirectX::XMVECTOR Right = DirectX::XMVectorSet(cy, 0, -sy, 0.0f);
	DirectX::XMVECTOR Up = DirectX::XMVector3Cross(Right, Front);

	// offset調整
	DirectX::XMFLOAT3 Offset;
	DirectX::XMStoreFloat3(&Offset, Up);
	Offset = Offset * offset.y;
	Offset = Offset + (DirectX::XMFLOAT3(cy, 0, -sy) * -offset.x);

	focus = {
		Mathf::Lerp(focus.x, PlayerPostion.x,5.f * elapsedTime),
		Mathf::Lerp(focus.y, PlayerPostion.y,5.f * elapsedTime),
		Mathf::Lerp(focus.z, PlayerPostion.z,5.f * elapsedTime)
	};

	// RayCast 壁対策
	{
		if (Offset.x != 0.0f || Offset.y != 0.0f || Offset.x != 0.0f)
		{
			HitResult hit;
			DirectX::XMFLOAT3 end = focus + Offset;

			if (MAPTILES.RayCast(focus, end, hit, true))
			{
				// カメラが壁に当たった
				focus = hit.position;
			}
			else
			{
				focus = focus + Offset;
			}
		}
	}

	// カメラ揺れ
	if (shakeTimer > 0.0f)
	{
		float shakeY = shakeRange * (shakeTimer);
		shakeY *= shakeModify;
		shakeModify *= -1.0f; // 逆

		focus.y += shakeY;
	}

	// カメラの視点&注視点を算出
	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);

	DirectX::XMVECTOR Distance = DirectX::XMVectorSet(distance, distance, distance, 0);

	DirectX::XMVECTOR Eye = DirectX::XMVectorSubtract(Focus, DirectX::XMVectorMultiply(Front, Distance));

	// RayCast 壁対策
	{
		DirectX::XMFLOAT3 end;
		DirectX::XMStoreFloat3(&end, Eye);
		if (focus != eye)
		{
			HitResult hit;
			if (MAPTILES.RayCast(focus, end, hit, true))
			{
				// カメラが壁に当たった
				float temp = hit.distance;
				if (temp > XMFLOAT3Length(end - focus))
				{
					hit.distance = temp;
				}
				float d = hit.distance * 0.95f;

				if (d < 0.01f) d = 0.01f;
				Eye = DirectX::XMVectorSubtract(Focus, DirectX::XMVectorScale(Front, d));
			}
		}
	}

	// ビュー行列からワールド行列を算出
	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	DirectX::XMMATRIX World = DirectX::XMMatrixTranspose(View);
	// ワールド行列から方向を算出
	Right = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(1, 0, 0, 0), World);
	Up = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 1, 0, 0), World);
	// 結果を格納
	DirectX::XMStoreFloat3(&eye, Eye);
	DirectX::XMStoreFloat3(&up, Up);
	DirectX::XMStoreFloat3(&right, Right);
	CameraManager::Instance().GetCamera()->CameraShake(CameraManager::Instance().GetCamera()->GetShakeAmplitude(), CameraManager::Instance().GetCamera()->GetShakeTime(), elapsedTime);
	shakenTarget = { focus + CameraManager::Instance().GetCamera()->GetShakenOffset() };
	// カメラに設定
	CameraManager::Instance().GetCamera()->SetLookAt(eye, shakenTarget, DirectX::XMFLOAT3(0, 1, 0));
}
DirectX::XMFLOAT3 ThridPersonCameraController::GetTargetPt(float distance)
{
	return eye + (XMFLOAT3Normalize(focus - eye) * distance);
}

void ThridPersonCameraController::AimMode(bool enable)
{
	if (enable)
	{
		distanceCache = distanceLerp;
		offsetLerp = { 0.05f, 0.02f };
		distanceLerp = 3.0f;

		CROSS_HAIR.Show();
	}
	else
	{
		offsetLerp = { 0.0, 0.0f };
		distanceLerp = distanceCache;

		CROSS_HAIR.Hide();
	}
}

void ThridPersonCameraController::Imgui()
{

}

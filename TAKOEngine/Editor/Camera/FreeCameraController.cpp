#include <imgui.h>
#include "FreeCameraController.h"

// 更新処理
void FreeCameraController::Update(float elapsedTime)
{
	if (!enable) return;
	// デバッグウィンド操作中は処理しない
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow)) return;

	// IMGUIのマウス入力値を使ってカメラ操作する。
	ImGuiIO io = ImGui::GetIO();

	// マウスカーソルの移動量を求める
	float moveX = io.MouseDelta.x * 0.02f;
	float moveY = io.MouseDelta.y * 0.02f;

	// マウスボタン押下中
	if (io.MouseDown[ImGuiMouseButton_Left])
	{
		// Y軸回転
		angleY += moveX * 0.5f;
		if (angleY > DirectX::XM_PI)
		{
			angleY -= DirectX::XM_2PI;
		}
		else if (angleY < -DirectX::XM_PI)
		{
			angleY += DirectX::XM_2PI;
		}
		// X軸回転
		angleX += moveY * 0.5f;
		if (angleX > DirectX::XM_PI)
		{
			angleX -= DirectX::XM_2PI;
		}
		else if (angleX < -DirectX::XM_PI)
		{
			angleX += DirectX::XM_2PI;
		}
	}
	// マウス中ボタン押下中
	else if (io.MouseDown[ImGuiMouseButton_Middle])
	{
		// 平行移動
		float s = distance * 0.035f;
		float x = moveX * s;
		float y = moveY * s;

		focus.x -= right.x * x;
		focus.y -= right.y * x;
		focus.z -= right.z * x;

		focus.x += up.x * y;
		focus.y += up.y * y;
		focus.z += up.z * y;
	}
	// マウス右ボタン押下中
	else if (io.MouseDown[ImGuiMouseButton_Right])
	{
		// ズーム
		distance += (-moveY - moveX) * distance * 0.1f;
	}
	// マウスほーいる
	else if (io.MouseWheel != 0)
	{
		// ズーム
		distance -= io.MouseWheel * distance * 0.1f;
	}

	float sx = ::sinf(angleX);
	float cx = ::cosf(angleX);
	float sy = ::sinf(angleY);
	float cy = ::cosf(angleY);

	// カメラの方向を算出
	DirectX::XMVECTOR Front = DirectX::XMVectorSet(-cx * sy, -sx, -cx * cy, 0.0f);
	DirectX::XMVECTOR Right = DirectX::XMVectorSet(cy, 0, -sy, 0.0f);
	DirectX::XMVECTOR Up = DirectX::XMVector3Cross(Right, Front);
	// カメラの視点&注視点を算出
	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
	DirectX::XMVECTOR Distance = DirectX::XMVectorSet(distance, distance, distance, 0);
	DirectX::XMVECTOR Eye = DirectX::XMVectorSubtract(Focus, DirectX::XMVectorMultiply(Front, Distance));
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
}
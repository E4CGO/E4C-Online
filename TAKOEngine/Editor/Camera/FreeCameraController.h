#pragma once

#include "CameraController.h"

class FreeCameraController : public CameraController
{
public:
	// インスタンス取得
	static FreeCameraController& Instance();

	FreeCameraController();

	// 更新処理
	void Update(float elapsedTime = 0.0f) override;
};

#define FRCamera FreeCameraController::Instance()
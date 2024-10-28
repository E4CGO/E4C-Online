#pragma once

#include "CameraController.h"

class FreeCameraController : public CameraController
{
public:
	// �C���X�^���X�擾
	static FreeCameraController& Instance();

	FreeCameraController();

	// �X�V����
	void Update(float elapsedTime = 0.0f) override;
};

#define FRCamera FreeCameraController::Instance()
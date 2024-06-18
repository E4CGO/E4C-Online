#pragma once

#include "CameraController.h"

class FreeCameraController : public CameraController
{
public:
	// XVˆ—
	void Update(float elapsedTime = 0.0f) override;
};

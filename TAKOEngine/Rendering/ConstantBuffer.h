#pragma once

#include <DirectXMath.h>

#include "RenderContext.h"

//TODO::SetCbScene
struct CbScene
{
	DirectX::XMFLOAT4X4	view_projection;
	DirectX::XMFLOAT4   camera_position;

	//���C�g���
	DirectX::XMFLOAT4		ambientLightColor;
	DirectionalLightData	directionalLightData;
	PointLightData			pointLightData[PointLightMax];	// �_�������
	SpotLightData			spotLightData[SpotLightMax];	// �X�|�b�g���C�g���
	int						pointLightCount = 0;			// �_������
	int						spotLightCount = 0;				// �X�|�b�g���C�g��
};

struct CbLambertMaterial
{
	DirectX::XMFLOAT4	color;
};
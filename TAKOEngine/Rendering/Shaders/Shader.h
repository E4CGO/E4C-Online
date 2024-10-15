//! @file Shader.h
//! @note

#ifndef __GRAPHICS_SHADER_H__
#define __GRAPHICS_SHADER_H__

#include <vector>
#include <DirectXMath.h>

#include "TAKOEngine/Rendering/RenderContext.h"
#include "TAKOEngine/Rendering/Model/Model.h"
#include "TAKOEngine/Rendering/Model/ModelDX12.h"
#include "TAKOEngine/Rendering/Sprite.h"
#include "TAKOEngine/Rendering/PipelineState.h"

//********************************************************************
// @class Shader
// @brief シェーダー関連の基底クラス
// @par   [説明]
//********************************************************************
class Shader
{
public:
	Shader() {}
	virtual ~Shader() {}

	// 描画開始
	virtual void Begin(const RenderContext& rc) = 0;
	
	// 描画終了
	virtual void End(const RenderContext& rc)
	{
		ID3D11DeviceContext* dc = rc.deviceContext;
		dc->VSSetShader(nullptr, nullptr, 0);
		dc->PSSetShader(nullptr, nullptr, 0);
		dc->IASetInputLayout(nullptr);

		ID3D11ShaderResourceView* srvs[] = { nullptr, nullptr };
		dc->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
		ID3D11ShaderResourceView* shadowSrvs[] = { nullptr, nullptr, nullptr, nullptr };
		dc->PSSetShaderResources(10, ARRAYSIZE(shadowSrvs), shadowSrvs);
	}
};

#endif // !__GRAPHICS_SHADER_H__
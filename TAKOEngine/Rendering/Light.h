//! @file Light.h
//! @note

#ifndef __LIGHT_LIGHT_H__
#define __LIGHT_LIGHT_H__

#include <DirectXMath.h>

#include "TAKOEngine/Rendering/RenderContext.h"

// 光源タイプ
enum class LightType
{
	Directional,	// 平行光源
	Point,			// 点光源
	Spot,			// スポットライト
};

// 光源クラス
//*********************************************************************
// @class Light
// @brief ライトクラス
// @par   [説明]
//*********************************************************************
class Light
{
public:
	Light(LightType lightType = LightType::Directional);

	// ライト情報をRenderContextに積む
	void PushRenderContext(RenderContext& rc) const;

	// デバッグ情報の表示
	void DrawDebugGUI();

	// デバッグ図形の表示
	void DrawDebugPrimitive();

	// ライトタイプ
	LightType GetLightType() { return lightType; }

	// ライトの座標
	DirectX::XMFLOAT3 GetPosition() { return position; }
	void SetPosition(DirectX::XMFLOAT3 position) { this->position = position; }

	// ライトの向き
	DirectX::XMFLOAT3 GetDirection() { return direction; }
	void SetDirection(DirectX::XMFLOAT3 direction) { this->direction = direction; }
	// 色
	DirectX::XMFLOAT4 GetColor() { return color; }
	void SetColor(DirectX::XMFLOAT4 color) { this->color = color; }

	// ライトの範囲
	float GetRange() { return range; }
	void SetRange(float range) { this->range = range; }

	// インナー
	float GetInnerCorn() { return innerCorn; }
	void SetInnerCorn(float innerCorn) { this->innerCorn = innerCorn; }

	// アウター
	float GetOuterCorn() { return outerCorn; }
	void SetOuterCorn(float outerCorn) { this->outerCorn = outerCorn; }

private:
	LightType lightType = LightType::Directional;				// ライトのタイプ
	DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0, -1, -1); // ライトの座標
	DirectX::XMFLOAT3 direction = DirectX::XMFLOAT3(0, -1, -1); // ライトの向き
	DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1, 1, 1, 1);	// ライトの色
	float	range = 20.0f;		// 範囲
	float	innerCorn = 0.99f;	// インナー
	float	outerCorn = 0.9f;	// アウター
};

#endif // !__LIGHT_LIGHT_H__
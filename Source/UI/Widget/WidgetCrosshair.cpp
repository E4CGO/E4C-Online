//! @file WidgetCrosshair.cpp
//! @note 

#include "WidgetCrosshair.h"
#include "TAKOEngine/Runtime/tentacle_lib.h"
#include <iostream>

static WidgetCrosshair* instance = nullptr;
// インスタンス取得

/**************************************************************************//**
 	@brief	インスタンスを取得
	@return	UIインスタンス
*//***************************************************************************/
WidgetCrosshair& WidgetCrosshair::Instance()
{
	return *instance;
}

/**************************************************************************//**
 	@brief	コンストラクタ
*//***************************************************************************/
WidgetCrosshair::WidgetCrosshair() : WidgetImage("Data/Sprites/crosshair122.png")
{
	// インスタンスポインタ設定
	instance = this;

	m_size.x = m_size.y = SCREEN_W * 0.04f;
	m_position = DirectX::XMFLOAT2{ SCREEN_W * 0.5f - m_size.x * 0.5f, SCREEN_H * 0.5f - m_size.y * 0.5f };
	m_color.w = 0.0f;
}

/**************************************************************************//**
 	@brief		更新処理
	@param[in]	elapsedTime	経過時間
	@return		なし
*//***************************************************************************/
void WidgetCrosshair::Update(float elapsedTime)
{
	m_angle += DirectX::XMConvertToRadians(90) * elapsedTime;
	if (m_angle > DirectX::XM_PI) m_angle -= DirectX::XM_2PI;
	if (m_angle < -DirectX::XM_PI) m_angle += DirectX::XM_2PI;

	if (!m_enable)
	{
		m_color.w -= elapsedTime * 2.0f;
		if (m_color.w < 0.0f)  m_color.w = 0.0f;
	}
	else
	{
		m_color.w += elapsedTime * 2.0f;
		if (m_color.w > 1.0f)  m_color.w = 1.0f;
	}
}
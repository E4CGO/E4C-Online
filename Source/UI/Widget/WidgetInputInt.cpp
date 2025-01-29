//! @file WidgetInputInt.cpp
//! @note

#include "WidgetInputInt.h"
#include "TAKOEngine/Tool/XMFLOAT.h"

template <typename T>
WidgetInputInt<T>::WidgetInputInt(T* integer, T min, T max, bool loop) : m_pInt(integer), m_min(min), m_max(max), m_loopFlag(loop)
{
	const DirectX::XMFLOAT2 size = { 50.0f, 50.0f };
	m_pBtnLeft = new WidgetButtonImage("", "Data/Sprites/UI/Character/left_button.png", [&](WidgetButton*) {
		if ((*m_pInt) > m_min)
		{
			(*m_pInt)--;
		}
		else if (m_loopFlag)
		{
			(*m_pInt) = m_max;
		}
		});
	m_pBtnRight = new WidgetButtonImage("", "Data/Sprites/UI/Character/right_button.png", [&](WidgetButton*) {
		if ((*m_pInt) < m_max)
		{
			(*m_pInt)++;
		}
		else if (m_loopFlag)
		{
			(*m_pInt) = m_min;
		}
		});
	m_pBtnLeft->SetSize(size);
	m_pBtnRight->SetSize(size);

	m_size = { m_pBtnLeft->GetSize().x + m_pBtnRight->GetSize().x, m_pBtnLeft->GetSize().y };
}

template <typename T>
/**************************************************************************//**
	@brief		更新処理
	@param[in]	elapsedTime	経過時間
*//***************************************************************************/
void WidgetInputInt<T>::Update(float elapsedTime)
{
	// 位置更新
	m_pBtnLeft->SetPosition(m_position);
	m_pBtnRight->SetPosition(m_position + DirectX::XMFLOAT2{ m_pBtnLeft->GetSize().x, 0.0f });

	m_pBtnLeft->Update(elapsedTime);
	m_pBtnRight->Update(elapsedTime);
}

template <typename T>
/**************************************************************************//**
	@brief		描画処理
	@param[in]	elapsedTime	経過時間
*//***************************************************************************/
void WidgetInputInt<T>::Render(const RenderContext& rc)
{
	m_pBtnLeft->Render(rc);
	m_pBtnRight->Render(rc);
}
template <typename T>
/**************************************************************************//**
	@brief		描画処理DX12
	@param[in]	elapsedTime	経過時間
*//***************************************************************************/
void WidgetInputInt<T>::RenderDX12(const RenderContextDX12& rc)
{
	m_pBtnLeft->RenderDX12(rc);
	m_pBtnRight->RenderDX12(rc);
}

template <typename T>
/**************************************************************************//**
	@brief	デストラクタ
*//***************************************************************************/
WidgetInputInt<T>::~WidgetInputInt()
{
	delete m_pBtnLeft;
	delete m_pBtnRight;
}

template class WidgetInputInt<int>;
template class WidgetInputInt<uint8_t>;
template class WidgetInputInt<uint16_t>;
template class WidgetInputInt<uint32_t>;
template class WidgetInputInt<uint64_t>;
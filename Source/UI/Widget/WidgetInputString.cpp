//! @file WidgetInputString.cpp
//! @note 

#include "UI/Widget/WidgetInputString.h"
#include <imgui.h>


/**************************************************************************//**
 	@brief		コンストラクタ
	@param[in]	string	文字列参照ポインタ
	@param[in]	limit	文字列字数制限
*//***************************************************************************/
WidgetInputString::WidgetInputString(const char* label, std::string* string, uint32_t limit) : m_label(label), m_pString(string) {
	SetSize({ 500.0f, 100.0f });

	std::string dummy;
	dummy.resize(limit, '\0');
	m_strSize = dummy.size();
}

/**************************************************************************//**
 	@brief		更新処理
	@param[in]	elapsedTime	経過時間
*//***************************************************************************/
void WidgetInputString::Update(float elapsedTime)
{
	//buffer = std::vector<char>(m_pString->begin(), m_pString->end());
}

/**************************************************************************//**
 	@brief		描写処理
	@param[in]	rc	レンダーコンテンツ
*//***************************************************************************/
void WidgetInputString::Render(const RenderContext& rc)
{

}

/**************************************************************************//**
	@brief		DX12描写処理
	@param[in]	rc	レンダーコンテンツ
*//***************************************************************************/
void WidgetInputString::RenderDX12(const RenderContextDX12& rc)
{
	ImVec2 pos = ImGui::GetMainViewport()->Pos;
	ImGui::SetNextWindowPos(ImVec2(pos.x + m_position.x, pos.y + m_position.y), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(500, 100), ImGuiCond_FirstUseEver);
	if (ImGui::Begin(m_label.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::InputText(m_label.c_str(), m_pString->data(), m_strSize);
		ImGui::End();
	}
}

/**************************************************************************//**
 	@brief		デストラクタ
*//***************************************************************************/
WidgetInputString::~WidgetInputString(){}
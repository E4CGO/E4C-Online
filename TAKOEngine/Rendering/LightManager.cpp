//! @file LightManager.cpp
//! @note

#include "LightManager.h"
#include "imgui.h"

//***********************************************************
// @brief       デストラクタ
// @param[in]   なし
// @return      なし
//***********************************************************
LightManager::~LightManager()
{
	Clear();
}

//***********************************************************
// @brief       ライトを登録する
// @param[in]   light  登録する光源を指すポインタ
// @return      なし
//***********************************************************
void LightManager::Register(Light* light)
{
	lights.emplace_back(light);
}

//***********************************************************
// @brief       ライトの登録を解除する
// @param[in]   light  登録する光源を指すポインタ
// @return      なし
//***********************************************************
void LightManager::Remove(Light* light)
{
	std::vector<Light*>::iterator	it = std::find(lights.begin(), lights.end(), light);
	if (it != lights.end())
	{
		lights.erase(it);
		delete	light;
	}
}

//***********************************************************
// @brief       登録済みのライトを全削除する
// @param[in]   なし
// @return      なし
//***********************************************************
void LightManager::Clear()
{
	for (Light* light : lights)
	{
		delete	light;
	}
	lights.clear();
}

//***********************************************************
// @brief       ライト情報をRenderContextに積む
// @param[in]   rc   レンダーコンテキスト
// @return      なし
//***********************************************************
void LightManager::PushRenderContext(RenderContext& rc)
{
	// 環境光の情報を追加
	rc.ambientLightColor = ambientColor;

	// 登録されている光源の情報を設定
	for (Light* light : lights)
	{
		light->PushRenderContext(rc);
	}
}

//***********************************************************
// @brief      デバッグ情報の表示
// @param[in]  なし
// @return     なし
//***********************************************************
void LightManager::DrawDebugGUI()
{
	if (ImGui::TreeNode("Lights"))
	{
		ImGui::ColorEdit3("AmbientColor", &ambientColor.x);
		int nodeId = 0;
		for (Light* light : lights)
		{
			ImGui::PushID(nodeId++);
			light->DrawDebugGUI();
			ImGui::PopID();
		}
		ImGui::TreePop();
	}
}

//***********************************************************
// @brief      デバッグ図形の表示
// @param[in]  なし
// @return     なし
//***********************************************************
void LightManager::DrawDebugPrimitive()
{
	for (Light* light : lights)
	{
		light->DrawDebugPrimitive();
	}
}
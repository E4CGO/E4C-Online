//! @file LightManager.cpp
//! @note

#include "LightManager.h"
#include "imgui.h"

//***********************************************************
// @brief       �f�X�g���N�^
// @param[in]   �Ȃ�
// @return      �Ȃ�
//***********************************************************
LightManager::~LightManager()
{
	Clear();
}

//***********************************************************
// @brief       ���C�g��o�^����
// @param[in]   light  �o�^����������w���|�C���^
// @return      �Ȃ�
//***********************************************************
void LightManager::Register(Light* light)
{
	lights.emplace_back(light);
}

//***********************************************************
// @brief       ���C�g�̓o�^����������
// @param[in]   light  �o�^����������w���|�C���^
// @return      �Ȃ�
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
// @brief       �o�^�ς݂̃��C�g��S�폜����
// @param[in]   �Ȃ�
// @return      �Ȃ�
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
// @brief       ���C�g����RenderContext�ɐς�
// @param[in]   rc   �����_�[�R���e�L�X�g
// @return      �Ȃ�
//***********************************************************
void LightManager::PushRenderContext(RenderContext& rc)
{
	// �����̏���ǉ�
	rc.ambientLightColor = ambientColor;

	// �o�^����Ă�������̏���ݒ�
	for (Light* light : lights)
	{
		light->PushRenderContext(rc);
	}
}

//***********************************************************
// @brief      �f�o�b�O���̕\��
// @param[in]  �Ȃ�
// @return     �Ȃ�
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
// @brief      �f�o�b�O�}�`�̕\��
// @param[in]  �Ȃ�
// @return     �Ȃ�
//***********************************************************
void LightManager::DrawDebugPrimitive()
{
	for (Light* light : lights)
	{
		light->DrawDebugPrimitive();
	}
}
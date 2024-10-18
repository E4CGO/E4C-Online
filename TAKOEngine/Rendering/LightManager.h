//! @file LightManager.h
//! @note

#ifndef __LIGHT_LIGHT_MANAGER_H__
#define __LIGHT_LIGHT_MANAGER_H__

#include <vector>

#include "TAKOEngine/Rendering/Light.h"
#include "TAKOEngine/Rendering/RenderContext.h"

//*********************************************************
// @class LightManager
// @brief ���C�g�Ǘ��N���X
// @par   [����]
//*********************************************************
class LightManager
{
	LightManager() {}
	~LightManager();

public:
	static LightManager& Instance()
	{
		static	LightManager	lightManager;
		return	lightManager;
	}

	// ���C�g��o�^����
	void Register(Light* light);

	// ���C�g�̓o�^����������
	void Remove(Light* light);

	// �o�^�ς݂̃��C�g��S�폜����
	void Clear();

	// ���C�g����RenderContext�ɐς�
	void PushRenderContext(RenderContext& rc);

	// �f�o�b�O���̕\��
	void DrawDebugGUI();

	// �f�o�b�O�v���~�e�B�u�̕\��
	void DrawDebugPrimitive();

	// ����
	DirectX::XMFLOAT4	GetAmbientColor() { return	ambientColor; }
	void	SetAmbientColor(DirectX::XMFLOAT4 ambientColor) { this->ambientColor = ambientColor; }

	// ���C�g�̏��
	int GetLightCount() const { return static_cast<int>(lights.size()); }
	Light* GetLight(int index) const { return lights.at(index); }
	std::vector<Light*> GetAllLight() const { return lights; }

private:
	std::vector<Light*>	lights;
	DirectX::XMFLOAT4	ambientColor = { 0.2f, 0.2f, 0.2f, 1.0f };
};

#endif // !__LIGHT_LIGHT_MANAGER_H__
//! @file CameraManager.cpp
//! @note �J�����Ǘ�
#include "TAKOEngine/Editor/Camera/CameraManager.h"
/**************************************************************************//**
		@brief		�X�V
		@param[in]	�Ȃ�
		@return		�Ȃ�
*//***************************************************************************/
void CameraManager::Update()
{

	//�j������
	for (Camera* camera : removes)
	{
		auto it = std::find(cameras.begin(), cameras.end(), camera);

		if (it != cameras.end())
		{
			cameras.erase(it);
		}
		delete camera;
	}
	removes.clear();

}
/**************************************************************************//**
		@brief		�폜
		@param[in]	�Ȃ�
		@return		�Ȃ�
*//***************************************************************************/
void CameraManager::Remove(Camera* camera)
{
	//�j�����X�g�ɒǉ�
	removes.insert(camera);
}
/**************************************************************************//**
		@brief		�o�^
		@param[in]	�Ȃ�
		@return		�Ȃ�
*//***************************************************************************/
void CameraManager::Register(Camera* camera)
{
	cameras.emplace_back(camera);
}
/**************************************************************************//**
		@brief		�N���A
		@param[in]	�Ȃ�
		@return		�Ȃ�
*//***************************************************************************/
void CameraManager::Clear()
{
	for (Camera* camera : cameras)
	{
		delete camera;
	}
	cameras.clear();
}
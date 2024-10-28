//! @file CameraManager.cpp
//! @note ƒJƒƒ‰ŠÇ—
#include "TAKOEngine/Editor/Camera/CameraManager.h"
/**************************************************************************//**
		@brief		XV
		@param[in]	‚È‚µ
		@return		‚È‚µ
*//***************************************************************************/
void CameraManager::Update()
{

	//”jŠüˆ—
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
		@brief		íœ
		@param[in]	‚È‚µ
		@return		‚È‚µ
*//***************************************************************************/
void CameraManager::Remove(Camera* camera)
{
	//”jŠüƒŠƒXƒg‚É’Ç‰Á
	removes.insert(camera);
}
/**************************************************************************//**
		@brief		“o˜^
		@param[in]	‚È‚µ
		@return		‚È‚µ
*//***************************************************************************/
void CameraManager::Register(Camera* camera)
{
	cameras.emplace_back(camera);
}
/**************************************************************************//**
		@brief		ƒNƒŠƒA
		@param[in]	‚È‚µ
		@return		‚È‚µ
*//***************************************************************************/
void CameraManager::Clear()
{
	for (Camera* camera : cameras)
	{
		delete camera;
	}
	cameras.clear();
}